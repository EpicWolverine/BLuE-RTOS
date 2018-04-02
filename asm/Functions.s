		AREA |.text|, CODE, READONLY, ALIGN=2
        THUMB
        REQUIRE8
        PRESERVE8
		;No idea what the above does
;******************************************************************************
        EXPORT	PendSV_Handler		;Exported to the main Assembly File
		
		EXPORT  DisableInterrupts	
        EXPORT  EnableInterrupts
        EXPORT  StartCritical
        EXPORT  EndCritical
			
		EXTERN	Scheduler
		EXTERN	Switcher
		EXTERN	CurrentStackPointer
		
;Used for Context Switching. Calling/Return from this interrupt affects Registers
;XPSR PC LR R12 R3 R2 R1 R0
PendSV_Handler  PROC
                EXPORT  PendSV_Handler            [WEAK]
				CPSID   I
				PUSH    {R4-R11}	;Push other values to stack. SP is not pushed.
				LDR 	R4, =CurrentStackPointer ;load the address of the C sp to R4
				STR		SP,[R4]					 ;Store the Current Stack Pointer for the task into 
												 ;the global Variable, get saved to the TCB in the start
												 ;of the scheduler function.
			    LDR LR, =ToNextTask
				B   Switcher		;Runs the switch from going from tasks to the scheduler
                ENDP
ToNextTask
			LDR		R4, =CurrentStackPointer ;load Stack Address of the Next Task
			LDR		SP,[R4]					;Load the New Stack pointer
			POP		{R4-R11}				;Pop values
			LDR 	LR, =0xfffffff9			;Exit Handler mode
			CPSIE	I						;enable interrupts
			BX		LR						;Return to normal Functionality
;*********** DisableInterrupts ***************
; disable interrupts
; inputs:  none
; outputs: none
DisableInterrupts
        CPSID  I
        BX     LR

;*********** EnableInterrupts ***************
; disable interrupts
; inputs:  none
; outputs: none
EnableInterrupts
        CPSIE  I
        BX     LR

;*********** StartCritical ************************
; make a copy of previous I bit, disable interrupts
; inputs:  none
; outputs: previous I bit
StartCritical
        MRS    R0, PRIMASK  ; save old status
        CPSID  I            ; mask all (except faults)
        BX     LR

;*********** EndCritical ************************
; using the copy of previous I bit, restore I bit to previous value
; inputs:  previous I bit
; outputs: none
EndCritical
        MSR    PRIMASK, R0
        BX     LR


;******************************************************************************
;
; Make sure the end of this section is aligned.
;
;******************************************************************************
        ALIGN

;******************************************************************************
;
; Tell the assembler that we're done.
;
;******************************************************************************
        END

		AREA |.text|, CODE, READONLY, ALIGN=2
        THUMB
        REQUIRE8
        PRESERVE8
		;No idea what the above does
;******************************************************************************
;The function in this file are usually in startup.s but for ease of editting and debugging they were moved here.
        EXPORT  DisableInterrupts	;Exported to the C Files
        EXPORT  EnableInterrupts
        EXPORT  StartCritical
        EXPORT  EndCritical
		
		;C values Imported
		EXTERN	Scheduler
		EXTERN	Switcher
		EXTERN	CurrentStackPointer
		
;Used for Context Switching. Calling/Return from Handler interrupt automatically PUSHES/POPS Registers
;XPSR PC LR R12 R3 R2 R1 R0
PendSV_Handler PROC
                EXPORT  PendSV_Handler            [WEAK]
				CPSID   I						;Disables Interrupts
				PUSH    {R4-R11}				;Push other values to stack. SP is not pushed.
				LDR 	R4, =CurrentStackPointer;load the address of the C sp to R4
				STR		SP,[R4]					;Store the Current Stack Pointer for the task into 
												;the global Variable. This value gets saved in the
												;Switcher Function.
			    LDR LR, =ToNextTask				;Load Return Address with ToNextTask
				B   Switcher					;Runs the switcher Function For Going Bewteen the 
												;Schedular and the Tasks
                ENDP
ToNextTask
			LDR		R4, =CurrentStackPointer	;load Stack Address of the Next Task
			LDR		SP,[R4]						;Load the New Stack pointer
			POP		{R4-R11}					;Pop values
			LDR 	LR, =0xfffffff9				;Load lR with escape sequence for Handler mode
			CPSIE	I							;enable interrupts
			BX		LR							;Return to normal Functionality

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
        MSR    PRIMASK, R0  ; Restore old status
        BX     LR


        ALIGN
        END

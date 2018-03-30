//****************** EE383lab4.c ***************
// Program written by: Your Names
// Date Created: 3/30/2018
// Based on the project by: Chris Mitchell (cpmitc4@gmail.com)
//
// Brief description of the program:
// We made a RTOS!
//
//*****************************************************************************


//Includes
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
//#include "inc/hw_ints.h"
#include "inc/tm4c123gh6pm.h"

//#define FAULT_SYSTICK           15          // System Tick
#define MAX_TASKS 20
#define STACK_BASE  0x20000000
int numTasks=0;

//Global and OS System Types/Functions
typedef struct tcb_s {
    unsigned int reason;           /* which semaphore woke the task up */
    unsigned int sem;              /* semaphore flag */
		unsigned int priority;				 /*Task Priority*/
    unsigned int *stack_top;       /* top of stack */
    unsigned int *stack_ptr;       /* current stack pointer */
    void *(*task_ptr)(void *);     /* task function pointer */
} tcb_t;


unsigned int os_curr_tid;          /* OS current task ID */
unsigned int os_sem;               /* OS semaphore flag */
tcb_t os_tcb[MAX_TASKS];           /* OS task control blocks */





//Prototypes
void InitConsole(void); //Instructor Provided
void os_switch(void);
void os_context_save(tcb_t *tcb);
void os_context_restore(tcb_t *tcb);
void newTask(void *(*task_ptr)(void *task),int semCount, int priority);


//// The delay parameter is in units of the 80 MHz core clock(12.5 ns) 
//void SysTick_Wait(unsigned long delay){ 
//	NVIC_ST_RELOAD_R = delay-1; // number of counts NVIC_ST_CURRENT_R = 0;
//	while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for flag 
//	}
//}
//// Call this routine to wait for delay*10ms 
//void SysTick_Wait10ms(unsigned long delay){ 
//	unsigned long i; 
//	for(i=0; i<delay; i++){ 
//		SysTick_Wait(800000); // wait 10ms
//	}
//}


//void SysTick_ISR(void){
//	int32_t i32Val, delay;
//	int32_t delayLength = 10000;
//	SysTickDisable();
//	UARTprintf("SysTick interrupt!\n");
//	while (delay<delayLength){
//		delay++;
//	}
//	i32Val = GPIOPinRead(GPIO_PORTF_BASE, (GPIO_PIN_3));
//	GPIOPinWrite(GPIO_PORTF_BASE, (GPIO_PIN_3), (i32Val ^ GPIO_PIN_3));
//	delay=0;
//	while (delay<delayLength){
//		delay++;
//	}
//	SysTickEnable();
//}

//void SysTick_Init(void){ 
////	NVIC_ST_CTRL_R = 0; // 1) disable SysTick during setup 
//	SysTickDisable();
////	NVIC_ST_RELOAD_R = 800000-1; // 2) maximum reload value (0x00FFFFFF)
//	SysTickPeriodSet(800000-1);
//	NVIC_ST_CURRENT_R = 0; // 3) any write to CURRENT clears it 
//	SysTickIntRegister(SysTick_ISR);
//	IntPrioritySet(FAULT_SYSTICK, sysTickPrioity); //0xE0 is the lowest priority
////	NVIC_ST_CTRL_R = 0x00000005; // 4) enable SysTick with core clock	
//	SysTickIntEnable();
//	SysTickEnable();
//	UARTprintf("SysTick initilized.\n");
//}


/*
	Will create a new task by creating a task control block and populating it with the correct values.
*/

void newTask(void *(*task_ptr)(void *task),int semCount, int priority)
{
	numTasks++;
	os_tcb[numTasks-1].reason = 0;
	os_tcb[numTasks-1].sem=0;
	os_tcb[numTasks-1].priority = priority;
	os_tcb[numTasks-1].stack_top = STACK_BASE + numTasks*(500);
	os_tcb[numTasks-1].stack_ptr = STACK_BASE + numTasks*(500);
	os_tcb[numTasks-1].task_ptr = task_ptr;
}


int main(void)
{
    // Set up the serial console to use for displaying messages.  This is just
    // for this lab program and is not needed for ADC operation.
    InitConsole();
    
    // Display the setup on the console.
    UARTprintf("Dearborn RTOS\n");
		UARTprintf("*****************************************************\n");
    
//		SysTick_Init();
		
		UARTprintf("Initialization Complete...\n");

    // Sample the temperature sensor forever.  Display the value on the
    // console.
    while(1)
    {
			
    }
}


void os_switch(void)
{
   unsigned int tid = MAX_TASKS;
   unsigned int reason;

   do {
       reason = os_tcb[--tid].sem & os_sem;
       if (reason || os_tcb[tid].sem == 0) {
          os_context_save(&os_tcb[os_curr_tid]);
          os_curr_tid = tid;
          os_tcb[os_curr_tid].reason = reason;
          os_tcb[os_curr_tid].sem &= ~reason;
          os_sem &= ~reason;
          os_context_restore(&os_tcb[os_curr_tid]);
       }
   } while (tid > 0);
}

void os_context_save(tcb_t *tcb){
	tcb->stack_ptr--;
};

void os_context_restore(tcb_t *tcb){
	
};


//PROVIDED FUNCTIONS.  NO NEED TO MODIFY
//*****************************************************************************
//
// This function sets up UART0 to be used for a console to display information
// as the lab is running.
//
//*****************************************************************************
void InitConsole(void)
{
    // Enable GPIO port A which is used for UART0 pins.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Configure the pin muxing for UART0 functions on port A0 and A1.
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    // Enable UART0 so that we can configure the clock.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Use the internal 16MHz oscillator as the UART clock source.
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
   

		// Select the alternate (UART) function for these pins.   
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Initialize the UART for console I/O. 9600 BAUD
//    UARTStdioConfig(0, 9600, 16000000);
		UARTStdioConfig(0, 11520, 16000000);
}

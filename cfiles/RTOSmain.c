//Includes
#include <stdbool.h>
#include <stdint.h>

#include "../header/genInit.h"	//Functions for Setting Up Buttons, Leds, UART, and Systick
#include "../header/Task.h"		//Contains the Heads to Sema and Flags and Mailboxes

extern Tasks TaskBlocks[MaxTasks];//Not static because needed in Semaphore, Mailbox and Flags
extern unsigned int TaskNum;	//Total number of tasks created
extern volatile unsigned int Ticks;

//Prototypes For tasks
static void FuncRed(void);
static void FuncBlue(void);
static void FuncGreen(void);
static void FuncButton(void);
static void Monitor(void);

//Globals For Sema, Flags, Mailbox, ect...
Sema MySema;
Flag MyFlag;
Mailbox MyBox;
unsigned int delay = 1;


int main(void)
{
		SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ); //Setups up system clock

		Gen_Init(); 							//Setups The Leds and Buttons
		InitConsole(921600);				//Setup USB UART, Pass in Baud Rate
		Systick_Init(1000);				/*Setup Systick, value passed in is the number of Ticks per second
																Number of Ticks Determines how long the delay is. For example with
																the default value 1000, a delay of 1 is 1ms. Value can be changed but 
																1000 is recommend.
																*/
		//Initalize Semaphores, Flags, and Mailboxes
		CreateSema(&MySema,1);
		CreateFlag(&MyFlag,0x1|0x2);//Create Flag for 1 and 2
		CreateMailbox(&MyBox);

		//Add Functions to the RTOS, inputs are Function Address and Priority with 0 being the Highest
		AddFunc(FuncRed,1);
		AddFunc(FuncBlue,2);
		AddFunc(FuncGreen,3);
		AddFunc(FuncButton,~0);
		AddFunc(Monitor,4);
		
		///Start Running the Tasks
		StartRTOS();
}

void FuncButton(void)
{
	int32_t test;
	while(1)
	{
		test = GPIOPinRead(GPIO_PORTF_BASE,0x11);
		if(test == 0x00)delay++;//sw2
//		if(test == 0x01)delay--;//sw1
		if(delay < 2) delay++;
		TaskDelay(4);
	}
}

void FuncRed(void)
{
	int32_t test;
	while(1)
	{
		test = GPIOPinRead(GPIO_PORTF_BASE,14);
		GPIOPinWrite(GPIO_PORTF_BASE,0xE,test^2);
		TaskDelay(1);
		test = GPIOPinRead(GPIO_PORTF_BASE,14);
		GPIOPinWrite(GPIO_PORTF_BASE,0xE,test^2);
		TaskDelay(delay);
	}
}
void FuncBlue(void)
{
	int32_t test;
	while(1)
	{
		test = GPIOPinRead(GPIO_PORTF_BASE,14);
		GPIOPinWrite(GPIO_PORTF_BASE,0xE,test^4);
		TaskDelay(1);
		test = GPIOPinRead(GPIO_PORTF_BASE,14);
		GPIOPinWrite(GPIO_PORTF_BASE,0xE,test^4);
		TaskDelay(delay);
	}
}
void FuncGreen(void)
{
	int32_t test;
	while(1)
	{
		test = GPIOPinRead(GPIO_PORTF_BASE,14);
		GPIOPinWrite(GPIO_PORTF_BASE,0xE,test^8);
		TaskDelay(1);
		test = GPIOPinRead(GPIO_PORTF_BASE,14);
		GPIOPinWrite(GPIO_PORTF_BASE,0xE,test^8);
		TaskDelay(delay);
	}
}

void Monitor(void){
	int32_t i;
	while(1){
		UARTprintf("\x1b[2J");
		UARTprintf("                            BLuE RTOS top\n");
		UARTprintf("System Time: %10u\n", Ticks);
		//Priority,Delay,Semaphore,EventFlag,MailBox
		UARTprintf("|------------------------------------------------------------------|\n");
		UARTprintf("|Task #| Priority |Task Delay|Switches|Semaphore|Event Flag|Mailbox|\n");
		UARTprintf("|------------------------------------------------------------------|\n");
		for(i=0; i < TaskNum; i++){
			UARTprintf("|Task %u|%10u|%10u|%8u| %8X|  %8X|", 
				i, TaskBlocks[i].Priority, TaskBlocks[i].Delay, TaskBlocks[i].TaskSwitches, 
				TaskBlocks[i].Semaphore, TaskBlocks[i].EventFlag);
			if(IsEmpty(TaskBlocks[i].MailBox) == 1){
				UARTprintf("  empty");
			}
			else if(IsFull(TaskBlocks[i].MailBox) == 1){
				UARTprintf("   full");
			}
			else{
				UARTprintf("       ");
			}
			UARTprintf("|\n");
		}
		for(i=TaskNum; i < MaxTasks; i++){
			UARTprintf("\n");
		}
		TaskDelay(1000);
	}
}

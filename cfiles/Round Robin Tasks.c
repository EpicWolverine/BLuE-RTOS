//Includes
#include <stdbool.h>
#include <stdint.h>
#include "../driverlib/gpio.h"
#include "../driverlib/interrupt.h"
#include "../inc/hw_memmap.h"
#include "../header/genInit.h"

//datatypes
typedef struct Functions{
    unsigned int Priority;
		unsigned int Delay;
    unsigned int LastTick;
    void(*FuncPtr)();
		unsigned int * SP;
		//Delay saves the time for when it nexts runs.
		//Semaphore Number/name
		//Blocked
		//Maybe add other stuff for tasks
}Tasks;

//globals
#define MaxTasks 10
#define StackSize 100 //Each Slot Stores 4 Bytes
static unsigned int TaskStacks[MaxTasks][StackSize];
static unsigned int Ticks = 0;
static unsigned int TaskNum = 0;
static unsigned int RunningNum;
unsigned int * CurrentStackPointer; //Not static because used in asm file
static Tasks TaskBlocks[MaxTasks];
static unsigned int IdleStack[16];
static unsigned short Mode = 0; //We start at 1 because going from the main thread to the ROTS scheduler


//prototypes
void Ticker(void);
void FuncRed(void);
void FuncBlue(void);
void FuncGreen(void);
void AddFunc(void(*)(),int,int);
void Scheduler(void);
void Switcher(void);
void StartRTOS(void);
void SetupIdle(void);

//Used for Context Switching
#define PendSV (unsigned int *)0xE000ED04
#define ContextSwitch() { *PendSV = 0x10000000;}

int main(void)
{
		SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
		
		//Button_Init(); //Setup the Buttons
		//Led_Init();    //Setup the Leds
		Gen_Init(); //Setups The Leds and Buttons
		
		Systick_Init(Ticker,1000);/*Setup the systick function
															Ticker is for incrementing a counter when an interrupt happens
															The Second Number is for how many Counter Per Second.
															*/
		///Create Tasks
		///Function name, prio, repeat rate.
		AddFunc(FuncRed,1,1000);
		AddFunc(FuncBlue,2,500);
		AddFunc(FuncGreen,3,250);
		
		///Start running all the tasks
		StartRTOS(); //Just calls the scheduler so ha
}

void AddFunc(void(*Func)(),int Prio,int delay)
{
	unsigned int * Temp;
	TaskBlocks[TaskNum].Priority = Prio;
	TaskBlocks[TaskNum].Delay = delay;
	TaskBlocks[TaskNum].LastTick = 0;
	TaskBlocks[TaskNum].FuncPtr = Func;
	Temp = &TaskStacks[TaskNum][StackSize-1]; //set the stack point to the top of the stack
	*(Temp--) = 0x01000000;//Post Decriment used to point to a blank address. Initalizing from bottom of the stack to the top.
												 //First is set to this because normal operations when returning
	*(Temp--) = (unsigned int)Func;//Load PC with the start of the Function
	*(Temp--) = 0x14141414;//XPSR PC LR R12 R3 R2 R1 R0 then R11-R4 **Stack Pointer is never stored on stack
	*(Temp--) = 0x12121212;
	*(Temp--) = 0x03030303;
	*(Temp--) = 0x02020202;
	*(Temp--) = 0x01010101;
	*(Temp--) = 0x00000000;
	*(Temp--) = 0x11111111;
	*(Temp--) = 0x10101010;
	*(Temp--) = 0x09090909;
	*(Temp--) = 0x08080808;
	*(Temp--) = 0x07070707;
	*(Temp--) = 0x06060606;
	*(Temp--) = 0x05050505;
	*(Temp) = 0x04040404;	//SP points to a full Location
	TaskBlocks[TaskNum].SP = Temp;
	// Add filling the stack pointer. only thing that matters is getting the proper stack pointer and setting the return address/pc to the function pointer.
	TaskNum++;
}

void Scheduler()
{
	int i;
	while(1)
	{
		for(i=0;i<TaskNum;i++)
			{
				if(TaskBlocks[i].Delay + TaskBlocks[i].LastTick < Ticks)
				{
					RunningNum = i;	
					//TaskBlocks[i].FuncPtr();
					ContextSwitch();
				}
			}
	}
}

void Switcher()
{
	if(Mode == 0)
	{//Schedular to Task
		CurrentStackPointer = TaskBlocks[RunningNum].SP;
		Mode = 1;
	}
	else
	{//Tasks to Schedular
		TaskBlocks[RunningNum].SP = CurrentStackPointer;
		TaskBlocks[RunningNum].LastTick = Ticks;
		Mode = 0;
		IdleStack[14] = (unsigned int) Scheduler; //always start the schedular from the begining
		IdleStack[15] = 0x01000000;								//CCR FIX TO NOT BREAK THE BOARD
		CurrentStackPointer = &IdleStack[0];			//set to bottom of the stack.
	}
}

void StartRTOS()
{
		Scheduler(); //Start the schedular and git going
}

void FuncRed(void)
{
	while(1)
	{
		int32_t test;
		test = GPIOPinRead(GPIO_PORTF_BASE,14);
		GPIOPinWrite(GPIO_PORTF_BASE,0xE,test^2);
		ContextSwitch();
	}
}
void FuncBlue(void)
{
	while(1)
	{
		int32_t test;
		test = GPIOPinRead(GPIO_PORTF_BASE,14);
		GPIOPinWrite(GPIO_PORTF_BASE,0xE,test^4);
		ContextSwitch();
	}
}
void FuncGreen(void)
{
	while(1)
	{
		int32_t test;
		test = GPIOPinRead(GPIO_PORTF_BASE,14);
		GPIOPinWrite(GPIO_PORTF_BASE,0xE,test^8);
		ContextSwitch();
	}
}
void SetupIdle(void)
{
	IdleStack[0] = 0x4;
	IdleStack[1] = 0x5;
	IdleStack[2] = 0x6;
	IdleStack[3] = 0x7;
	IdleStack[4] = 0x8;
	IdleStack[5] = 0x9;
	IdleStack[6] = 0x10;
	IdleStack[7] = 0x11;
	IdleStack[8] = 0x0;
	IdleStack[9] = 0x1;
	IdleStack[10] = 0x2;
	IdleStack[11] = 0x3;
	IdleStack[12] = 0x12;
	IdleStack[13] = 0x13; //LR
	IdleStack[14] = (unsigned int) Scheduler; //Function Pointer to the Scheduler
	IdleStack[15] = 0x01000000; //CCR to not break the LaunchPad
	
}
void Ticker(void)
{
	Ticks++;
}

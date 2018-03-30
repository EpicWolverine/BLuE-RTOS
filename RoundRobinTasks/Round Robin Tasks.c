//Includes
#include <stdbool.h>
#include <stdint.h>
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "inc/hw_memmap.h"

//datatypes
typedef struct Functions{
    int Priority;
	int Delay;
    int LastTick;
    void(*FuncPtr)();
}Tasks;

//globals
#define MaxTasks 10
static unsigned int Ticks = 0;
static unsigned int TaskNum = 0;
static Tasks TaskBlocks[MaxTasks];


//prototypes
void Delay(void);
void FuncRed(void);
void FuncBlue(void);
void FuncGreen(void);
void AddFunc(void(*)(),int,int);
void StartSchedule(void);


int main(void)
{
		SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
		
		//I/O Setup 
		GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	
		//Systick
		SysTickEnable();
		SysTickIntRegister(Delay);//enable interrupt function
		SysTickIntEnable(); // enable interrupt
		SysTickPeriodSet(SysCtlClockGet()/1000);// base clock 16mhz every 1ms?
		///Create Tasks
		///Function name, prio, repeat rate.
		AddFunc(FuncRed,1,1000);
		AddFunc(FuncBlue,2,500);
		AddFunc(FuncGreen,3,250);
		
		///Start running all the tasks
		StartSchedule();
}

void AddFunc(void(*Func)(),int Prio,int delay)
{
	TaskBlocks[TaskNum].Priority = Prio;
	TaskBlocks[TaskNum].Delay = delay;
	TaskBlocks[TaskNum].LastTick = 0;
	TaskBlocks[TaskNum].FuncPtr = Func;
	TaskNum++;
}
void StartSchedule()
{
	int i;
	//volatile Tasks * Temp;
	while(1)
	{
		//Temp = 0;
		for(i=0;i<TaskNum;i++)
		{
			if(TaskBlocks[i].Delay + TaskBlocks[i].LastTick < Ticks)
			{
				TaskBlocks[i].FuncPtr();
				TaskBlocks[i].LastTick = Ticks;
				/*if(TaskBlocks[i].Priority < Temp->Priority)
				{
					*Temp = TaskBlocks[i];
					//Temp = TaskBlocks;
				}*/
			}
		}
		/*if (Temp == 0) continue;
		Temp->FuncPtr();
		Temp->LastTick = Ticks;*/
	}
}

void FuncRed(void)
{
	int32_t test;
	test = GPIOPinRead(GPIO_PORTF_BASE,14);
	GPIOPinWrite(GPIO_PORTF_BASE,0xE,test^2);
}
void FuncBlue(void)
{
	int32_t test;
	test = GPIOPinRead(GPIO_PORTF_BASE,14);
	GPIOPinWrite(GPIO_PORTF_BASE,0xE,test^4);
}
void FuncGreen(void)
{
	int32_t test;
	test = GPIOPinRead(GPIO_PORTF_BASE,14);
	GPIOPinWrite(GPIO_PORTF_BASE,0xE,test^8);
}

void Delay(void)
{
	Ticks++;
}

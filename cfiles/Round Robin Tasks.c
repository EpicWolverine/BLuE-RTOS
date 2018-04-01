//Includes
#include <stdbool.h>
#include <stdint.h>
#include "../driverlib/gpio.h"
#include "../driverlib/interrupt.h"
#include "../inc/hw_memmap.h"


#include "../driverlib/uart.h"
#include "../utils/uartstdio.h"
#include "../driverlib/pin_map.h"
#include "../driverlib/sysctl.h"
#include "../inc/tm4c123gh6pm.h"
void InitConsole(void);


#include "../header/genInit.h"
#include "../header/Task.h"
#include "../header/Semaphore.h"

//Prototypes For tasks
void FuncRed(void);
void FuncBlue(void);
void FuncGreen(void);

//Globals For Sema,Flags,Mailbox ect
Sema MySema;


int main(void)
{
		SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

		Gen_Init(); //Led_Init();//Button_Init();//Setups The Leds and Buttons
		
		Systick_Init(Ticker,1000);/*Setup the systick function
															Ticker is for incrementing a counter when an interrupt happens
															The Second Number is for how many Counter Per Second.
															*/
		CreateSema(&MySema,1);
		InitConsole();
	
	
		///Create Tasks
		///Function name, prio, repeat rate.
		AddFunc(FuncRed,1);
		AddFunc(FuncBlue,2);
		AddFunc(FuncGreen,3);
		
		///Start running all the tasks
		StartRTOS(); //Just calls the scheduler so ha
}
extern Tasks TaskBlocks[MaxTasks];
void FuncRed(void)
{
	int32_t test;
	while(1)
	{
		TaskDelay(1000);
		PendSema(&MySema);
		test = GPIOPinRead(GPIO_PORTF_BASE,14);
		GPIOPinWrite(GPIO_PORTF_BASE,0xE,test^2);
		PostSema(&MySema);
		UARTprintf("A:%d\n",(test>>1)&1);
	}
}
void FuncBlue(void)
{
	int32_t test;
	while(1)
	{
		TaskDelay(500);
		PendSema(&MySema);
		test = GPIOPinRead(GPIO_PORTF_BASE,14);
		GPIOPinWrite(GPIO_PORTF_BASE,0xE,test^4);
		PostSema(&MySema);
		UARTprintf("B:%d\n",(test>>2)&1);
	}
}
void FuncGreen(void)
{
	int32_t test;
	while(1)
	{
		TaskDelay(250);
		PendSema(&MySema);
		test = GPIOPinRead(GPIO_PORTF_BASE,14);
		GPIOPinWrite(GPIO_PORTF_BASE,0xE,test^8);
		PostSema(&MySema);
		UARTprintf("C:%d\n",(test>>3)&1);
	}
}

void InitConsole(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTStdioConfig(0, 9600, 16000000);
}

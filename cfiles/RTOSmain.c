//Includes
#include <stdbool.h>
#include <stdint.h>
#include "../driverlib/gpio.h"
#include "../driverlib/interrupt.h"
#include "../inc/hw_memmap.h"

//Uart Includes
#include "../driverlib/uart.h"
#include "../utils/uartstdio.h"
#include "../driverlib/pin_map.h"
#include "../driverlib/sysctl.h"
#include "../inc/tm4c123gh6pm.h"
void InitConsole(void);


#include "../header/genInit.h"
#include "../header/Task.h" //Contains the Heads to Sema and Flags
extern Tasks TaskBlocks[MaxTasks];

//Prototypes For tasks
static void FuncRed(void);
static void FuncBlue(void);
static void FuncGreen(void);
static void FuncButton(void);

//Globals For Sema,Flags,Mailbox ect
Sema MySema;
Flag MyFlag;


int main(void)
{
		SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

		Gen_Init(); //Led_Init();//Button_Init();//Setups The Leds and Buttons
		
		Systick_Init(Ticker,1000);/*Setup the systick function
															Ticker is for incrementing a counter when an interrupt happens
															The Second Number is for how many Counter Per Second.
															*/
		CreateSema(&MySema,1);
		CreateFlag(&MyFlag,0x1|0x2);//Create Flag for 1 and 2
		InitConsole();
	
	
		///Create Tasks
		///Function name, prio
		AddFunc(FuncRed,1);
		AddFunc(FuncBlue,2);
		AddFunc(FuncGreen,3);
		AddFunc(FuncButton,4);
		
		///Start running all the tasks
		StartRTOS(); //Just calls the scheduler so ha
}

void FuncButton(void)
{
	int32_t test,ErrorCode;
	while(1)
	{
		ErrorCode = 0;
		test = GPIOPinRead(GPIO_PORTF_BASE,0x11);
		if(test == 0x10)	
			{
				ErrorCode = PostFlag(&MyFlag,0x1);//sw2
				UARTprintf("Error:%d\n",ErrorCode);
			}
		if(test == 0x01)	
			{
				ErrorCode = PostFlag(&MyFlag,0x2);//sw1
				UARTprintf("Error:%d\n",ErrorCode);
			}
		if(test == 0x0)	
			{
				ErrorCode = PostFlag(&MyFlag,0x1|0x2);//sw1
				UARTprintf("Error:%d\n",ErrorCode);
			}
		TaskDelay(0);
	}
}

void FuncRed(void)
{
	int32_t test;
	while(1)
	{
		TaskDelay(500);
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
		PendFlag(&MyFlag,0x1|0x2,0x1);
		test = GPIOPinRead(GPIO_PORTF_BASE,14);
		GPIOPinWrite(GPIO_PORTF_BASE,0xE,test^4);
		UARTprintf("B:%d\n",(test>>2)&1);
		TaskDelay(100);
	}
}
void FuncGreen(void)
{
	int32_t test;
	while(1)
	{
		TaskDelay(750);
		PendSema(&MySema);
		TaskDelay(750);
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

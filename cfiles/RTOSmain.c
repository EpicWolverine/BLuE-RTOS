//Includes
#include <stdbool.h>
#include <stdint.h>
#include "../driverlib/interrupt.h"

#include "../header/genInit.h"
#include "../header/Task.h" //Contains the Heads to Sema and Flags
extern Tasks TaskBlocks[MaxTasks];

//Prototypes For tasks
static void FuncRed(void);
static void FuncBlue(void);
static void FuncGreen(void);
static void FuncButton(void);
static void FuncReci(void);
static void FuncSend(void);

//Globals For Sema,Flags,Mailbox ect
Sema MySema;
Flag MyFlag;
Mailbox MyBox;


int main(void)
{
		SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

		Gen_Init(); 							//Led_Init();//Button_Init();//Setups The Leds and Buttons
		InitConsole(9600);				//Setup USB UART, Pass in Baud Rate
		Systick_Init(Ticker,1000);/*Setup the systick function
															Ticker is for incrementing a counter when an interrupt happens
															The Second Number is for how many Counter Per Second.
															*/
		CreateSema(&MySema,1);
		CreateFlag(&MyFlag,0x1|0x2);//Create Flag for 1 and 2
		CreateMailbox(&MyBox);
	
		///Create Tasks
		///Function name, prio
		/*AddFunc(FuncRed,5);
		AddFunc(FuncBlue,1);
		AddFunc(FuncGreen,3);
		AddFunc(FuncButton,2);
		*/
		AddFunc(FuncSend,1);
		AddFunc(FuncReci,2);
		///Start running all the tasks
		StartRTOS(); //Just calls the scheduler so ha
}

void FuncButton(void)
{
	int32_t test;
	while(1)
	{
		test = GPIOPinRead(GPIO_PORTF_BASE,0x11);
		if(test == 0x10)PostFlag(&MyFlag,0x1);//sw2
		if(test == 0x01)PostFlag(&MyFlag,0x2);//sw1
		if(test == 0x00)PostFlag(&MyFlag,0x1|0x2);//Both sw1 and sw2 
		TaskDelay(0);
	}
}
void FuncSend(void)
{
	unsigned int value = 0;
	PostMailbox(&MyBox,(void *)value);
	PostMailbox(&MyBox,(void *)value);
	PostMailbox(&MyBox,(void *)value);
	while(1)
	{
		TaskDelay(200);
		if(404 == PostMailbox(&MyBox,(void *)value))UARTprintf("Error MailBox Full\n");
		value++;
	}
}
void FuncReci(void)
{
	unsigned int data;
	unsigned short error;
	while(1)
	{
		TaskDelay(100);
		data = (unsigned int)AcceptMailbox(&MyBox,&error);
		if(error==404) UARTprintf("Error MailBox Empty\n");
		else UARTprintf("Data=%d\n",data);
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
		TaskDelay(1);
		test = GPIOPinRead(GPIO_PORTF_BASE,14);
		GPIOPinWrite(GPIO_PORTF_BASE,0xE,test^4);
		UARTprintf("B:%d\n",(test>>2)&1);
		TaskDelay(9);
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

#include "../header/genInit.h"

void Button_Init(){
	CReg32 * R0;
	int i;
	R0 = (CReg32 *) PORTCLKENABLE_R;//Enable Clock For Port F
	*R0 |= 0x20;
	
	for(i=0;i<10;i++);				//Delay for SeetupTime
	
	R0 = (CReg32 *) PORTF_LOCK_R;	//Unlock Register
	*R0 = 0x4C4F434B;
	
	R0 = (CReg32 *) PORTF_CR_R;		//Allow For commiting
	*R0 |= 0xFF;
	
	R0 = (CReg32 *) PORTF_AMSEL_R;	//Disable AC Function
	*R0 &= ~0x11;
	
	R0 = (CReg32 *) PORTF_PCTL_R;	//Set as GPIO
	*R0 &= ~0x11;
	
	R0 = (CReg32 *) PORTF_DIR_R;	//Set as Input
	*R0 &= ~0x11;
	
	R0 = (CReg32 *) PORTF_AFSEL_R;	//Disable Alt Function
	*R0 &= ~0x11;
	
	R0 = (CReg32 *) PORTF_PUR_R;	//Enable Pullup, Pushing The button Brings the pin Low.
	*R0 |= 0x11;
	
	R0 = (CReg32 *) PORTF_DEN_R;	//Enable Digital Functionality
	*R0 |= 0x11;
	}

void Led_Init(){
	CReg32 * R0;
	int i;
	R0 = (CReg32 *) PORTCLKENABLE_R;//Enable Clock For Port F
	*R0 |= 0x20;
	
	for(i=0;i<10;i++);				//Delay for SeetupTime
	
	R0 = (CReg32 *) PORTF_LOCK_R;	//Unlock Register
	*R0 = 0x4C4F434B;
	
	R0 = (CReg32 *) PORTF_CR_R;		//Allow For commiting
	*R0 |= 0xFF;
	
	R0 = (CReg32 *) PORTF_AMSEL_R;	//Disable AC Function
	*R0 &= ~0x0E;
	
	R0 = (CReg32 *) PORTF_PCTL_R;	//Set as GPIO
	*R0 &= ~0x0E;
	
	R0 = (CReg32 *) PORTF_DIR_R;	//Set as Output
	*R0 |= 0x0E;
	
	R0 = (CReg32 *) PORTF_AFSEL_R;	//Disable Alt Function
	*R0 &= ~0x0E;
	
	R0 = (CReg32 *) PORTF_PUR_R;	//Disable Pullup cause why not
	*R0 &= ~0x0E;
	
	R0 = (CReg32 *) PORTF_DEN_R;	//Enable Digital Functionality
	*R0 |= 0x0E;
	}

void Gen_Init()		//Calls Both Functions
{
	Button_Init();
	Led_Init();
}

void Systick_Init(void(*FuncPtr)(),unsigned int TicksPerSec)
{
	SysTickEnable();				//Enable Systick
	SysTickIntRegister(FuncPtr);	//Enable interrupt function
	SysTickIntEnable(); 			//Enable interrupt
	SysTickPeriodSet(SysCtlClockGet()/TicksPerSec); //Set The Interrupt Rate for Systick
}
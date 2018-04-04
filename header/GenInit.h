#include <stdbool.h>
#include <stdint.h>
#include "../driverlib/sysctl.h"
#include "../driverlib/systick.h"
#include "../driverlib/gpio.h"
#include "../driverlib/pin_map.h"
#include "../driverlib/uart.h"
#include "../utils/uartstdio.h"
#include "../inc/hw_memmap.h"


#define PORTF_DATA_R 0x400253FC
#define PORTF_DIR_R 0x40025400
#define PORTF_AFSEL_R 0x40025420
#define PORTF_PUR_R 0x40025510
#define PORTF_DEN_R 0x4002551C
#define PORTF_LOCK_R 0x40025520
#define PORTF_CR_R 0x40025524
#define PORTF_AMSEL_R 0x40025528
#define PORTF_PCTL_R 0x4002552C
#define PORTCLKENABLE_R 0x400FE608
#define LOCK_KEY 0x4C4F434B
/*
#define SW1 0x10
#define SW2 0x01
#define Red 0x02
#define Blue 0x04
#define Green 0x08
*/
typedef volatile unsigned int CReg32;
void Button_Init(void);
void Led_Init(void);
void Gen_Init(void);
void Systick_Init(void(*FuncPtr)(),unsigned int TicksPerSec);
void InitConsole(unsigned int Baud);

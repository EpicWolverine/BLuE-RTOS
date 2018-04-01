#include "gpio.h"
#define GPortA 0x400040000
#define GPortB 0x400050000
#define GPortC 0x400060000
#define GPortD 0x400070000
#define GPortE 0x400240000
#define GPortF 0x400250000

#define DataReg		0x3FC
#define DirReg		0x400
#define AltFReg		0x420
#define PuReg		0x510
#define DigReg		0x51C
#define LockReg		0x520
#define CommitReg	0x524
#define AnalogReg	0x528
#define GpioReg		0x52c

#define UnlockKey	0x4C4F434B

#define GPIOClkEnable 0x400FE108

void SetRegister(CReg32 Address,CReg32 offset,CReg32 Data)
{
	*((CReg32 *)(Address + offset)) |= (CReg32)(Data);
}
void ClearRegister(CReg32 Address,CReg32 Data,CReg32 offset)
{
	*((CReg32 *)(Address + offset)) &= (CReg32)(~Data);
}
//#include datatypes
//Addresses
#define GPortA 0x400040000
#define GPortB 0x400050000
#define GPortC 0x400060000
#define GPortD 0x400070000
#define GPortE 0x400240000
#define GPortF 0x400250000
//Offsets
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
//Used to enable the clocks, no offset,A=0x1 B=0x2 C=0x4 D=0x8 E=0x10 F=0x20
#define GPIOClkEnable 0x400FE108

typedef volatile unsigned int CReg32;

void SetRegister(CReg32,CReg32,CReg32);
void ClearRegister(CReg32,CReg32,CReg32);
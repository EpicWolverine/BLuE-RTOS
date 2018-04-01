#include "systick.h"

void EnableSystick()
{
	* (CReg32*)systickCSR
}
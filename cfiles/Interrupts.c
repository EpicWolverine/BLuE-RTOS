#include "Interrupts.h"

void CriticalStart(void)
{
	__asm("    cpsid   i\n");
}
void CriticalEnd(void)
{
	__asm("    cpsie   i\n");
}

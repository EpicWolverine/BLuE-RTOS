#include "gpio.h"
//Addresses
#define systickCSR	0xE000E010
#define systickReload	0xE000E014
#define systickCurrent	0xE000E018


void EnableSystick(void);
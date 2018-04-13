#include "OSConfig.h"
#include "Semaphore.h"
#include "Flags.h"
#include "Mailbox.h"

//Used for Context Switching
#define PendSV (unsigned int *)0xE000ED04		//Register for causing Handler Fault
#define ContextSwitch() { *PendSV = 0x10000000;}//Sets the bit to casue a Fault
//Can only see the fault in debugging when running Not stepping the debugger.

typedef struct Functions{
	unsigned int Priority;
	unsigned int Delay;
	void(*FuncPtr)();
	unsigned int * SP;
	unsigned short Blocked;
	Sema * Semaphore;
	Flag * EventFlag;
	Mailbox * MailBox;
}Tasks;

void AddFunc(void(*)(),int);
void Scheduler(void);
void Switcher(void);
void StartRTOS(void);
void TaskDelay(unsigned int);
void EnterCS(void);
void ExitCS(void);
void Ticker(void);

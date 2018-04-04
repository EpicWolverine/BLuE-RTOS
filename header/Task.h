#include "Semaphore.h"
#include "Flags.h"
#include "Mailbox.h"

#define MaxTasks 10
#define StackSize 100 //Each Slot Stores 4 Bytes
//Used for Context Switching
#define PendSV (unsigned int *)0xE000ED04
#define ContextSwitch() { *PendSV = 0x10000000;}

typedef struct Functions{
	unsigned int Priority;
	unsigned int Delay;
	void(*FuncPtr)();
	unsigned int * SP;
	unsigned short Blocked;
	Sema * Semaphore;
	Flag * EventFlag;
	Mailbox * MailBox;
	//MailBox * MBox or something like it.
}Tasks;

void AddFunc(void(*)(),int);
void Scheduler(void);
void Switcher(void);
void StartRTOS(void);
void SetupIdle(void);
void TaskDelay(unsigned int);
void EnterCS(void);
void ExitCS(void);
void Ticker(void);

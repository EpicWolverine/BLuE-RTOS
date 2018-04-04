#include "../header/Task.h"

extern Tasks TaskBlocks[MaxTasks];
extern unsigned int TaskNum;
extern unsigned int RunningNum;



void PendSema(Sema * Semaphore)
{
	EnterCS();
	while(*Semaphore < 1)	//Loop while sema is in use
	{
		TaskBlocks[RunningNum].Blocked |= 0x1; //first bit is for semaphores
		TaskBlocks[RunningNum].Semaphore = Semaphore;
		ExitCS();		//Don't needed to worry of shared data anymore
		ContextSwitch();	//Change Task
		EnterCS();	//Start Critical Again when returned.
	}
	(*Semaphore)--;
	ExitCS();
}
unsigned int AcceptSema(Sema * Semaphore)
{
	EnterCS();
	if(*Semaphore < 1)//If Sema is in use Return Error
	{
		return 1;
	}
	(*Semaphore)--;
	ExitCS();
	return 0;
}
void PostSema(Sema * Semaphore)
{
	int i;
	EnterCS();
	(*Semaphore)++;
	if(*Semaphore > 0) //Check If Function need to be unblocked
	{
		for(i=0;i<TaskNum;i++)
		{
			if(TaskBlocks[i].Semaphore == Semaphore) // If pointing to the same Location
			{
				TaskBlocks[i].Blocked &= ~0x1;			// unblock Tasks that need the current semaphore
			}
		}
	}
	ExitCS();
}
void CreateSema(Sema * Semaphore,unsigned int Count)
{
	*Semaphore = Count;
}

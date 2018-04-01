#include "../header/Semaphore.h"
#include "../header/Task.h"

extern Tasks TaskBlocks[MaxTasks];
extern unsigned int TaskNum;
extern unsigned int RunningNum;



void PendSema(Sema * Semap)
{
	EnterCS();
	while(*Semap < 1)	//Loop while sema is in use
	{
		TaskBlocks[RunningNum].Blocked = 1;
		TaskBlocks[RunningNum].Semaphore = Semap;
		ExitCS();		//Don't needed to worry of shared data anymore
		ContextSwitch();	//Change Task
		EnterCS();	//Start Critical Again when returned.
	}
	(*Semap)--;
	ExitCS();
}
void AcceptSema(Sema * Semaphore)
{
	;
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
				TaskBlocks[i].Blocked = 0;			// unblock Tasks that need the current semaphore
			}
		}
	}
	ExitCS();
}
void CreateSema(Sema * Semaphore,unsigned int Count)
{
	*Semaphore = Count;
}

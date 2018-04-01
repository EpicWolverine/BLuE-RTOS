#include "../header/Task.h"
#include "../header/Semaphore.h"


unsigned int * CurrentStackPointer; //Not static because used in asm file
Tasks TaskBlocks[MaxTasks];		//Needed in Semaphore
unsigned int TaskNum = 0;		//Needed in Semaphore
unsigned int RunningNum;		//Needed in Semaphore
unsigned int OldFlags;

static unsigned int TaskStacks[MaxTasks][StackSize];
static unsigned int Ticks = 0;
static unsigned int IdleStack[16];
static unsigned short Mode = 0; //We start at 0 because going from the main thread to the ROTS scheduler

extern unsigned int StartCritical(void);
extern void EndCritical(unsigned int);


void Scheduler()
{
	unsigned int i;
	while(1)
	{
		for(i=0;i<TaskNum;i++) //Loop through all task
			{
				if(TaskBlocks[i].Blocked == 0)	//Check if UnBlocked
				{
					if(TaskBlocks[i].Delay < Ticks)	//Check if Ready
					{
						RunningNum = i;
						ContextSwitch();
					}
				}
			}
	}
}

void StartRTOS()
{
	Mode = 0;
	Scheduler(); //Start the schedular and git going
}

void Switcher()
{
	if(Mode == 0)
	{//Schedular to Task
		CurrentStackPointer = TaskBlocks[RunningNum].SP;
		Mode = 1;
	}
	else
	{//Tasks to Schedular
		TaskBlocks[RunningNum].SP = CurrentStackPointer;
		Mode = 0;
		IdleStack[14] = (unsigned int) Scheduler; //always start the schedular from the begining
		IdleStack[15] = 0x01000000;								//CCR FIX TO NOT BREAK THE BOARD
		CurrentStackPointer = &IdleStack[0];			//set to bottom of the stack.
	}
}

void AddFunc(void(*Func)(),int Prio)
{
	unsigned int * Temp;
	TaskBlocks[TaskNum].Priority = Prio;
	TaskBlocks[TaskNum].Delay = 0;
	TaskBlocks[TaskNum].FuncPtr = Func;
	TaskBlocks[TaskNum].Blocked = 0;
	TaskBlocks[TaskNum].Semaphore = 0;
	Temp = &TaskStacks[TaskNum][StackSize-1]; //set the stack point to the top of the stack
	*(Temp--) = 0x01000000;			//Post Decriment used to point to a blank address. Initalizing from bottom of the stack to the top.
									//First is set to this because normal operations when returning
	*(Temp--) = (unsigned int)Func;	//Load PC with the start of the Function
	*(Temp--) = 0x14141414;			//XPSR PC LR R12 R3 R2 R1 R0 then R11-R4 **Stack Pointer is never stored on stack
	*(Temp--) = 0x12121212;
	*(Temp--) = 0x03030303;
	*(Temp--) = 0x02020202;
	*(Temp--) = 0x01010101;
	*(Temp--) = 0x00000000;
	*(Temp--) = 0x11111111;
	*(Temp--) = 0x10101010;
	*(Temp--) = 0x09090909;
	*(Temp--) = 0x08080808;
	*(Temp--) = 0x07070707;
	*(Temp--) = 0x06060606;
	*(Temp--) = 0x05050505;
	*(Temp) = 0x04040404;	//SP points to a full Location so no decement
	TaskBlocks[TaskNum].SP = Temp;
//Only thing that matters is getting the proper stack pointer and setting the return address/pc to the function pointer.
	TaskNum++;
}

void SetupIdle(void)
{
	IdleStack[0] = 0x4;
	IdleStack[1] = 0x5;
	IdleStack[2] = 0x6;
	IdleStack[3] = 0x7;
	IdleStack[4] = 0x8;
	IdleStack[5] = 0x9;
	IdleStack[6] = 0x10;
	IdleStack[7] = 0x11;
	IdleStack[8] = 0x0;
	IdleStack[9] = 0x1;
	IdleStack[10] = 0x2;
	IdleStack[11] = 0x3;
	IdleStack[12] = 0x12;
	IdleStack[13] = 0x13; //LR
	IdleStack[14] = (unsigned int) Scheduler; //Function Pointer to the Scheduler
	IdleStack[15] = 0x01000000; //CCR to not break the LaunchPad
	
}

void Ticker(void)
{
	Ticks++;
}

void TaskDelay(unsigned int timeout)
{
	TaskBlocks[RunningNum].Delay = timeout + Ticks;
	ContextSwitch();
}

void EnterCS(void)
	{
		OldFlags = StartCritical();
	}
void ExitCS(void)
{
	EndCritical(OldFlags);
}

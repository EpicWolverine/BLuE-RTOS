#include "../header/Task.h"

unsigned int * CurrentStackPointer; //Not static because used in asm file
Tasks TaskBlocks[MaxTasks];//Not static because needed in Semaphore, Mailbox and Flags
unsigned int TaskNum = 0;	//Total number of tasks created
unsigned int RunningNum;	//Number of the running task

static unsigned int CSFlags;
static unsigned int TaskStacks[MaxTasks][StackSize];
static volatile unsigned int Ticks = 0;
static unsigned int * SchedulerSP;
static unsigned short Mode;

extern unsigned int StartCritical(void);
extern void EndCritical(unsigned int);


void Scheduler()
{
	unsigned int i,ActiveTask;
	while(1)
	{
		ActiveTask = 0;
		for(i=0;i<TaskNum;i++) 									//Loop through all task
			{
				if(TaskBlocks[i].Blocked == 0)			//Check if UnBlocked
				{
					if(TaskBlocks[i].Delay <= Ticks)	//Check if Ready
					{
						if(ActiveTask == 0)							//If no Active Task Set Task As Next To Run
						{
							ActiveTask = 1;
							RunningNum = i;
						}
						//If there already is an active task Then Check if this one is higher priority
						else if(TaskBlocks[i].Priority<TaskBlocks[RunningNum].Priority)
						{
							RunningNum = i;								//Lowest Number has the Highest Priority.
						}
					}
				}
			}
		if(ActiveTask)ContextSwitch(); 					//if a task was selected run it
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
		SchedulerSP = CurrentStackPointer;
		TaskBlocks[RunningNum].switches++;
		CurrentStackPointer = TaskBlocks[RunningNum].SP;
		Mode = 1;
	}
	else
	{//Tasks to Schedular
		TaskBlocks[RunningNum].SP = CurrentStackPointer;
		CurrentStackPointer = SchedulerSP;
		Mode = 0;
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
	TaskBlocks[TaskNum].EventFlag = 0;
	Temp = &TaskStacks[TaskNum][StackSize-1]; //set the stack point to the very bottom of the stack
	/*Post Decriment used to point to a blank address while Initalizing the stack with temp values.
		The Stack get filled in this order and gets poped in the reverse when a context switch is called
		XPSR PC LR R12 R3 R2 R1 R0 then R11-R4 **Stack Pointer is never stored on stack
		The only Places that needed to be filled properly when Initalizing are XPSR and PC
		XPSR is the status control register and the value 0x01000000 is used to not have the system Hard
		Fault as this value corresponds to normal operation mode. PC is set to the address of the Function 
		so that	when the switch is made it starts running the function.
		*/
	*(Temp--) = 0x01000000;
	*(Temp--) = (unsigned int)Func;	
	*(Temp--) = 0x14141414;
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
	*(Temp) = 0x04040404;					 //SP points to a full Location so there is no decement here
	TaskBlocks[TaskNum].SP = Temp; //set the Stack Pointer for the Task to the Top of the Stack
	TaskNum++;
	if(MaxTasks == TaskNum) TaskNum--; //Last Task will Always be overridden if it's greater than the max
}

void Ticker(void)
{
	Ticks++;			//used by systick and delays for counting how much time has elapsed
}

void TaskDelay(unsigned int timeout)
{
	TaskBlocks[RunningNum].Delay = timeout + Ticks;	//sets the delay before a task can run again
	ContextSwitch();																//switch tasks
}

void EnterCS(void)
{
	CSFlags = StartCritical();											//Enter Critical Section saving previous I bit
}

void ExitCS(void)
{
	EndCritical(CSFlags);														//Exit Critical Section Restoring previous I bit
}

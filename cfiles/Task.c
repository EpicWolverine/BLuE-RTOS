#include "Task.h"
#include <stdbool.h>

static void(*TaskBlock)();
static void(*TaskPtr)();
static bool * TaskFilled;
//static unsigned int * TaskFillArray;
static unsigned int TaskBlockSize;

void SetupTaskBlock(unsigned int ArraySize){
	void(*TempTaskBlock)()[ArraySize] = {0};
	bool Temp2[ArraySize] = {0};
	TaskBlock = &TempTaskBlock[0];
	TaskFilled = &Temp2[0];
	TaskBlockSize = ArraySize;
}

unsigned int AddTask(void(*NewFunc),int * TaskNumber){
	unsigned int Error = 1,i;
	for(i=0;i<TaskBlockSize;i++)
		{
			if(TaskFilled[i] != 0)
				{
					continue
				}
			else
				{
					TaskFilled[i] = True;
					*(TaskBlock+i) = NewFunc;
					*TaskNumber = i;
					Error = 0;
					break;
				}
		}
	return Error;
}

unsigned int DeleteTask(unsigned int TaskNumber){
	unsigned int Error = 1,i;
	if(TaskNumber >= TaskBlockSize)
		{
			Error = 1;
		}
	else
		{
			TaskFilled[TaskNumber] = False;
			*(TaskBlock+TaskNumber) = 0;
			Error = 0;
		}
	return Error;
}
#include "../header/Task.h"

extern Tasks TaskBlocks[MaxTasks];
extern unsigned int TaskNum;
extern unsigned int RunningNum;
//External Globals that are Needed.


void * PendMailbox(Mailbox * MBox)
{
	void * temp;
	EnterCS();
	
	while(IsEmpty(MBox))					//Loop while Flag not set
	{
		TaskBlocks[RunningNum].Blocked |= 0x4;	//Set Block For Mailbox
		TaskBlocks[RunningNum].MailBox = MBox;	//Set the address of the blocking mailbox
		ExitCS();								//Disable critical
		ContextSwitch();						//Change Task
		EnterCS();								//Start Critical Again when returned.
	}
	temp = *(MBox->Read); //Read the address
	MBox->Read++;					//Go to next address
	if(MBox->Read == &(MBox->Buffer[Buffersize])) MBox->Read = &(MBox->Buffer[0]);//wrap pointer
	ExitCS();
	return temp;//return the read
}

void * AcceptMailbox(Mailbox * MBox,unsigned short * error)
{
	void * temp;
	EnterCS();
	if(IsEmpty(MBox))					//Loop while Flag not set
	{
		*error = 404;
		ExitCS();
		return 0;
	}
	temp = *(MBox->Read); //Read the data
	MBox->Read++;					//Go to next address
	if(MBox->Read == &(MBox->Buffer[Buffersize])) MBox->Read = &(MBox->Buffer[0]);//wrap pointer
	ExitCS();
	return temp;
}

unsigned short PostMailbox(Mailbox * MBox,void * mail)
{
	int i;
	EnterCS();
	if(IsFull(MBox))
	{
		ExitCS();
		return 404;
	}
	*(MBox->Write) = mail;
	MBox->Write++;
	if(MBox->Write == &(MBox->Buffer[Buffersize])) MBox->Write = &(MBox->Buffer[0]); //wrap pointer
	for(i=0;i<TaskNum;i++)					//unblock functions that are blocked because empty
	{
		if(TaskBlocks[i].MailBox == MBox)	//check if current mailbox is the reason they blocked.
		{
			TaskBlocks[i].Blocked &= ~0x4;	//Unblock Tasks that are relying on MBox
		}
	}
	ExitCS();
	return 0;
}

void CreateMailbox(Mailbox * MBox)
{
	MBox->Write = &(MBox->Buffer[0]);
	MBox->Read = &(MBox->Buffer[0]);
}

static unsigned short IsEmpty(Mailbox * MBox)
{
	if(MBox->Write == MBox->Read)return 1;//Empty, Pointing to the same address
	return 0;
}
static unsigned short IsFull(Mailbox * MBox)
{
	void * temp;
	temp = (MBox->Write + 1);
	if(temp == &(MBox->Buffer[Buffersize])) temp = &(MBox->Buffer[0]);
	if(temp == MBox->Read)return 1;//Full, Pointing to the same address
	return 0;
}

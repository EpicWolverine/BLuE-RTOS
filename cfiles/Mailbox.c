#include "../header/Task.h"

extern Tasks TaskBlocks[MaxTasks];
extern unsigned int TaskNum;
extern unsigned int RunningNum;
//External Globals that are Needed.


void * PendMailbox(Mailbox * MBox)
{
	void * temp;
	EnterCS();
	
	while(IsEmpty(MBox))										//Loop while the Mailbox is empty
	{
		TaskBlocks[RunningNum].Blocked |= 0x4;//Set Blocking For Mailbox
		TaskBlocks[RunningNum].MailBox = MBox;//Set the address of the blocking mailbox
		ExitCS();															//Disable critical
		ContextSwitch();											//Change Task
		EnterCS();														//Start Critical Again when returned.
	}
	temp = *(MBox->Read); 									//Get the Contents from read
	MBox->Read++;														//Move Read to the next address
	if(MBox->Read == &(MBox->Buffer[MailBufferSize])) MBox->Read = &(MBox->Buffer[0]);//wrap pointer for mailbox
	ExitCS();
	return temp;														//return the read value
}

void * AcceptMailbox(Mailbox * MBox,unsigned short * error)
{
	void * temp;
	EnterCS();
	if(IsEmpty(MBox))												//If there is no mail Return an error
	{
		*error = 404;
		ExitCS();
		return 0;
	}
	*error = 0;															//Clear the Error because there is mail
	temp = *(MBox->Read); 									//Get the Contents from read
	MBox->Read++;														//Move Read to the next address
	if(MBox->Read == &(MBox->Buffer[MailBufferSize])) MBox->Read = &(MBox->Buffer[0]);//wrap pointer
	ExitCS();
	return temp;
}

unsigned short PostMailbox(Mailbox * MBox,void * mail)
{
	int i;
	EnterCS();
	if(IsFull(MBox))												//If the mailbox is full return an error
	{
		ExitCS();
		return 404;
	}
	*(MBox->Write) = mail;									//Put the mail in the Mailbox
	MBox->Write++;													//Change the Write Address
	if(MBox->Write == &(MBox->Buffer[MailBufferSize])) MBox->Write = &(MBox->Buffer[0]); //wrap pointer
	for(i=0;i<TaskNum;i++)									//unblock functions that are blocked because empty mailbox
	{
		if(TaskBlocks[i].MailBox == MBox)			//check if current mailbox is the reason they blocked.
		{
			TaskBlocks[i].Blocked &= ~0x4;			//Unblock tasks blocked by this mailbox
		}
	}
	ExitCS();
	return 0;
}

void CreateMailbox(Mailbox * MBox)
{
	MBox->Write = &(MBox->Buffer[0]);				//initalize to Read and Write to the first index
	MBox->Read = &(MBox->Buffer[0]);
}

unsigned short IsEmpty(Mailbox * MBox)
{
	if(MBox->Write == MBox->Read)return 1;	//Empty, Pointing to the same address
	return 0;																//Not empty
}
unsigned short IsFull(Mailbox * MBox)
{
	void * temp;
	temp = (MBox->Write + 1);								//Looks at the Next Address
	if(temp == &(MBox->Buffer[MailBufferSize])) temp = &(MBox->Buffer[0]); //Wraps Temp
	if(temp == MBox->Read)return 1;					//Full, If the next address is the same as read then its full.
	return 0;
}

#define Buffersize 4
typedef struct{
	void ** Write;						//pointer to void pointer in array
	void ** Read;
	void * Buffer[Buffersize];
}Mailbox;

void CreateMailbox(Mailbox *);
void * PendMailbox(Mailbox *);
void * AcceptMailbox(Mailbox *,unsigned short * error);
unsigned short PostMailbox(Mailbox *,void * mail);
static unsigned short IsEmpty(Mailbox *);
static unsigned short IsFull(Mailbox *);

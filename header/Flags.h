typedef struct FlagType{
	unsigned int FlagValue;
	unsigned int BitEnabled;
}Flag;

void CreateFlag(Flag *,unsigned int);
unsigned int PendFlag(Flag *,unsigned int,unsigned short);
unsigned int AcceptFlag(Flag *,unsigned int,unsigned short);
unsigned int PostFlag(Flag *,unsigned int);

#define CORE_NUM		2
#define CLOCK			2

struct Job{
	int Num;
	double in;
	double out;
	double CPU_ExecutionTime;
	double IC;
	double IC_P;
	double Clock;
	double CPI;
	double IO_ExecutionTime;

};

struct Core{
	Job Running;
	int flag;
};

struct CPU{
	Core Multi[CORE_NUM];
};

struct Node{
	Job data;
	Node *next;
	Node *prev;
};

struct DLinkedList{
	Node *head;
	Node *tail;
	Node *cur;
	int numOfData;
};

typedef DLinkedList List;

void ListInit(List *plist);
void LInsert(List *plist, Job Data);

int LFirst(List *plist, Job *pData);
int LNext(List *plist, Job *pData);

void LRemove(List *plist);
int LCount(List *plist);
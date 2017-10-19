
double randomDouble(void);
double randomDouble(int Start, int End);
int randomInt(int Start, int End);
double EXP(double lambda, double y);

Job MakeJob();
void InitValue(Job *P, CPU *C);
Core InitCore(Core C);

double GetExecutionTime(Job P);
double GetTotalPerformance(CPU *C);
double Amdahl(CPU *C);

void RunningJob(Job *P, List *list, Core *C, int index);
void TerminateJob(Job *P, List *list);

void PrintReadyQ(Job *P, List *list);
void PrintCore(List *list, Core C, int index);

DWORD WINAPI ThreadFunction(LPVOID Param);
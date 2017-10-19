#include <stdio.h>
#include <Windows.h>
#include <math.h>
#include "DLinkedList.h"
#include "CPU_Simulation.h"

double tmp = 0.0;
int JobNum = 0;

double randomDouble(void) {
	return (double)rand() / RAND_MAX;
}

double randomDouble(int Start, int End) {
	return (double)(rand() % (End - Start + 1) + Start) / 10;
}

int randomInt(int Start, int End) {
	return rand() % (End - Start + 1) + Start;
}

double EXP(double lambda, double y) {
	return -(1 / lambda) * log(1 - y);
}

Job MakeJob() {
	Job New;

	tmp += EXP(1, randomDouble());
	New.Num = JobNum;
	New.in = tmp;
	New.IC = randomDouble();
	New.IC_P = randomDouble();
	New.Clock = CLOCK;
	New.CPI = randomDouble(0, 30);
	New.CPU_ExecutionTime = GetExecutionTime(New);
	New.IO_ExecutionTime = randomDouble();
	New.out = New.CPU_ExecutionTime + New.IO_ExecutionTime + New.in;

	JobNum++;

	return New;
}

void InitValue(Job *P, CPU *C) {
	P->Num = 0;
	P->in = 0;		P->out = 0;
	P->CPU_ExecutionTime = 0;
	P->IO_ExecutionTime = 0;
	P->CPI = 0;		P->Clock = 0;
	P->IC = 0;		P->IC_P = 0;

	for (int i = 0; i < CORE_NUM; i++) {
		C->Multi[i].Running = *P;
		C->Multi[i].flag = 0;
	}
}

Core InitCore(Core C) {
	C.Running.CPU_ExecutionTime = 0.0;
	C.Running.IO_ExecutionTime = 0.0;
	C.Running.in = 0.0;		C.Running.Num = 0;
	C.Running.out = 0;		C.flag = 0;
	C.Running.IC = 0;		C.Running.IC_P = 0;
	C.Running.CPI = 0;		C.Running.Clock = 0;

	return C;
}

double GetExecutionTime(Job P) {
	return (P.IC + P.IC_P) * P.CPI * (1 / P.Clock);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
																												  // 예상 병렬처리 부분
double GetTotalPerformance(CPU *C) {
	int count = 0;
	double Sum_IC = 0.0, Sum_IC_P = 0.0, Sum_CPI = 0.0;

	for (int i = 0; i < CORE_NUM; i++) {
		if (C->Multi[i].Running.CPU_ExecutionTime == 0)
			continue;

		Sum_IC += C->Multi[i].Running.IC;
		Sum_IC_P += C->Multi[i].Running.IC_P;
		Sum_CPI += C->Multi[i].Running.CPI;
		count++;
	}

	if (count != 0)
		return 1 / ((Sum_IC + Sum_IC_P) * (Sum_CPI / count) * ((double)1 / CLOCK)) * Amdahl(C);
	else
		return 0;
}

double Amdahl(CPU *C) {
	int count = 0;
	double Sum_IC = 0.0, Sum_IC_P = 0.0;

	for (int i = 0; i < CORE_NUM; i++) {
		if (C->Multi[i].Running.CPU_ExecutionTime == 0)
			continue;

		Sum_IC += C->Multi[i].Running.IC;
		Sum_IC_P += C->Multi[i].Running.IC_P;
		count++;
	}

	if (count != 0)
		return 1 / ((1 - (Sum_IC_P / (Sum_IC + Sum_IC_P))) + ((Sum_IC_P / (Sum_IC + Sum_IC_P)) / count));
	else
		return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
																												  // 예상 병렬처리 부분
void RunningJob(Job *P, List *list, Core *C, int index) {
	if (LFirst(list, P)) {
		(C + index)->Running = *P;
		(C + index)->flag = 1;
	}
}

void TerminateJob(Job *P, List *list) {
	if (LFirst(list, P)) {
		LRemove(list);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PrintReadyQ(Job *P, List *list) {
	printf("\n");
	printf("READY QUEUE\n");
	printf("--------------------------------------------------------------------------------------\n");
	printf("NUM		 |		IN		|		CPU_ExecutionTime\n");
	printf("--------------------------------------------------------------------------------------\n");

	if (LFirst(list, P)) {
		printf("%d				%f			%f\n", P->Num, P->in, P->CPU_ExecutionTime);

		while (LNext(list, P))
			printf("%d				%f			%f\n", P->Num, P->in, P->CPU_ExecutionTime);
	}

	printf("--------------------------------------------------------------------------------------\n");
	printf("Ready Queue Size : %d\n", LCount(list));
	printf("\n");
}

void PrintCore(List *list, Core C, int index) {
	printf("\n");
	printf("CORE[%d]\n", index);
	printf("--------------------------------------------------------------------------------------\n");
	printf("NUM		 |		IN		|		CPU_ExecutionTime\n");
	printf("--------------------------------------------------------------------------------------\n");
	printf("%d				%f			%f\n", C.Running.Num, C.Running.in, C.Running.CPU_ExecutionTime);
	printf("--------------------------------------------------------------------------------------\n");

	if (LCount(list) != 0)
		printf("Core[%d] Performance : %f\n", index, 1 / C.Running.CPU_ExecutionTime);
	else
		printf("Core[%d] Performance : 0\n", index);

	printf("\n");
}

DWORD WINAPI ThreadFunction(LPVOID Param) {
	List *Ready = (List*)Param;

	while (1) {
		if (randomDouble() < 0.5)
			LInsert(Ready, MakeJob());
		Sleep(250);
	}

	return 0;
}
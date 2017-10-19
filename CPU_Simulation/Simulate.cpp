#include <stdio.h>
#include <Windows.h>
#include "DLinkedList.h"
#include "CPU_Simulation.h"

int main() {
	Job P;
	List ReadyQ;
	CPU C;
	HANDLE hThread;
	DWORD thrdId;

	srand(GetTickCount());

	ListInit(&ReadyQ);
	InitValue(&P, &C);

	hThread = CreateThread(NULL, 0, ThreadFunction, &ReadyQ, 0, &thrdId);
	WaitForSingleObject(&thrdId, INFINITE);

	while (1) {

		printf("######################################################################################\n");
		PrintReadyQ(&P, &ReadyQ);

		////////////////////////////////////////////////////////////////////////////////////////////
																								 //	예상 병렬처리부분
		for (int i = 0; i < CORE_NUM; i++) {																	
			RunningJob(&P, &ReadyQ, C.Multi, i);
			PrintCore(&ReadyQ, C.Multi[i], i);
			TerminateJob(&P, &ReadyQ);
		}

		printf("TotalPerformance : %f\n", GetTotalPerformance(&C));

		for (int i = 0; i < CORE_NUM; i++)
			C.Multi[i] = InitCore(C.Multi[i]);

		////////////////////////////////////////////////////////////////////////////////////////////

		//Sleep(700);
	}
}
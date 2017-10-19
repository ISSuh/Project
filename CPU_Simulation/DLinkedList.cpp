#include <stdlib.h>
#include "DLinkedList.h"

void ListInit(List * plist) {
	plist->head = (Node*)malloc(sizeof(Node));
	plist->tail = (Node*)malloc(sizeof(Node));

	plist->head->prev = NULL;
	plist->head->next = plist->tail;

	plist->tail->next = NULL;
	plist->tail->prev = plist->head;

	plist->numOfData = 0;
}

void LInsert(List *plist, Job data) {
	Node *newNode = (Node*)malloc(sizeof(Node));
	newNode->data = data;

	newNode->prev = plist->tail->prev;
	plist->tail->prev->next = newNode;

	newNode->next = plist->tail;
	plist->tail->prev = newNode;

	(plist->numOfData)++;
}

int LFirst(List *plist, Job *pData) {
	if (plist->head->next == plist->tail)
		return 0;

	plist->cur = plist->head->next;
	*pData = plist->cur->data;
	return 1;
}

int LNext(List *plist, Job *pData) {
	if (plist->cur->next == plist->tail)
		return 0;

	plist->cur = plist->cur->next;
	*pData = plist->cur->data;
	return 1;
}

void LRemove(List *plist) {
	Node *rpos = plist->cur;
	int remv = rpos->data.Num;

	plist->cur->prev->next = plist->cur->next;
	plist->cur->next->prev = plist->cur->prev;

	plist->cur = plist->cur->prev;

	free(rpos);
	(plist->numOfData)--;
}

int LCount(List *plist) {
	return plist->numOfData;
}

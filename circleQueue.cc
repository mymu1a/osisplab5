#include "circleQueue.h"

#include <sys/types.h>


// initialize Mutex for mutual exclusive access to the Queue
void initMutex(CircleHead* pHead);


void circleQueueInit(CircleHead* pHead, short sizeQueue)
{
	pHead->indexHead = -1;
	pHead->indexTail = -1;
	pHead->size = sizeQueue;
	pHead->isEmpty = true;
	pHead->indexMessage = 0;

	initMutex(pHead);

	pHead->countWrite = 0;
	pHead->countRead = 0;
}

void circleQueueLogState(CircleHead* pHead)
{
	printf("Head=%d\tTail=%d\tEmpty=%s\tSize=%d", pHead->indexHead, pHead->indexTail, pHead->isEmpty ? "true":"false", pHead->size);
}

bool circleQueueNextWrite(CircleHead* pHead, CircleElement** pElement)
{
	if (pHead->indexHead == (pHead->size - 1) && pHead->indexTail == -1)
	{
		return false;
	}
	if (pHead->indexHead == pHead->indexTail && pHead->isEmpty == false)
	{
		return false;
	}
	pHead->isEmpty = false;

	if (++pHead->indexHead == pHead->size)
	{
		pHead->indexHead = 0;
	}
	CircleElement* pBuffer;				// queue first element

	pBuffer = (CircleElement*)((u_char*)pHead + sizeof(CircleHead));

	*pElement = &pBuffer[pHead->indexHead];

	return true;
}

bool circleQueueNextRead(CircleHead* pHead, CircleElement** pElement)
{
	if (pHead->isEmpty == true)
	{
		return false;
	}
	if (++pHead->indexTail == pHead->size)
	{
		pHead->indexTail = 0;
	}
	if (pHead->indexTail == pHead->indexHead)
	{
		pHead->isEmpty = true;
	}
	CircleElement* pBuffer;				// queue first element

	pBuffer = (CircleElement*)((u_char*)pHead + sizeof(CircleHead));

	*pElement = &pBuffer[pHead->indexTail];

	return true;
}

// initialize Mutex for mutual exclusive access to the Queue
void initMutex(CircleHead* pHead)
{
	pthread_mutexattr_t	attribute;

	pthread_mutexattr_init(&attribute);
	pthread_mutexattr_setpshared(&attribute, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&pHead->mutex, &attribute);
}

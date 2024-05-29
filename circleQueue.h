#include "global.h"

#include <pthread.h>


struct CircleHead
{
	short indexHead;
	short indexTail;
	short size;
	struct CircleElement* pBuf;

	bool isEmpty;					// no info in the buffer
	pthread_mutex_t mutex;			// mutual exclusive access to the Queue

	unsigned	indexMessage;		// latest message index

	short countWrite;				// счетчик добавленных сообщений
	short countRead;				// счетчик извлеченных сообщений
};

struct CircleElement
{
	unsigned	indexMessage;		// message index ( is used as File Name )
};

void circleQueueInit(CircleHead* pHead, short sizeQueue);
bool circleQueueNextRead(CircleHead* pHead, CircleElement** pElement);
bool circleQueueNextWrite(CircleHead* pHead, CircleElement** pElement);

void circleQueueLogState(CircleHead* pHead);

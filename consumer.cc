#include "global.h"

#include "circleQueue.h"
#include "child.h"

#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */


void readMessage(unsigned indexMessage, struct Message* pMessage);
static void startNanoSleep(char* nameProgram, CircleHead* pCircleHead, Child* pChild);


void* threadConsumer(void* pData)
{
	Child* pChild = (Child*)pData;

	/*
	sem_t*		pSemaphore;

	pSemaphore = openSemaphore(SEM_PRODUSER_NAME);
	if (pSemaphore == NULL)
	{
		printf("Error: cannot open sem\n");
		return 1;
	}
	//*/
	startNanoSleep(pChild->nameProgram, pChild->pCircleHead, pChild);

	return NULL;
}

static void startNanoSleep(char* nameProgram, CircleHead* pCircleHead, Child* pChild)
{
	struct timespec		time, time2;
	int					count = 5;
	int					result;

	time.tv_sec = 1;
	time.tv_nsec = 0.5 * 1000000000;

	while (count-- > 0)
	{
		result = nanosleep(&time, &time2);
		if (result == 0)
		{
			CircleElement* pElement;

//			sem_wait(pSemaphore);
			pChild->inProcess = true;  // prevent terminating by Exit Signal

			pthread_mutex_lock(&pCircleHead->mutex);									// mutex lock

			if (circleQueueNextRead(pCircleHead, &pElement) == false)
			{
				printf("  circleQueue is empty -- xxxx --\t");
			}
			else
			{
				struct Message pMessage;

				readMessage(pElement->indexMessage, &pMessage);
				pCircleHead->countRead++;												// increment `счетчик извлеченных сообщений`
				printf("  %d ( count read )\t", pCircleHead->countRead);

			}
			printf(" ( %s )\n", nameProgram);

			usleep(500000);
			pChild->inProcess = false;  // prevent terminating by Exit Signal
			pthread_mutex_unlock(&pCircleHead->mutex);									// mutex unlock

//			sem_post(pSemaphore);
			if (pChild->isExit == true)
			{
				printf("isExit == true !!!!!!!!!!!!!\n");
				pthread_exit(NULL);
			}

		}
		if (result == -1)
		{
			if (errno != 4)
			{
				printf("Error in nanosleep():\n");
				printf("  errno: %d\n", errno);
				printf("  errno: %s\n", strerror(errno));

				return;
			}
		}
	}
}

void readMessage(unsigned indexMessage, struct Message* pMessage)
{
	int fd;
	char pathMessage[8 + 6] = { 0, };

	sprintf(pathMessage, "%s/%05d", MESSAGE_FOLDER, indexMessage);

	fd = open(pathMessage, O_RDWR, 0666);
	if (fd < 0)
	{
		printf("Error: cannot open Message file:\n");
		return;
	}

	read(fd, (void*)&pMessage->type, 1);
	read(fd, (void*)&pMessage->hash, 2);
	read(fd, (void*)&pMessage->size, 1);

	printf("pMessage->type = %d\t", pMessage->type);
	printf("pMessage->size = %d\t", pMessage->size);

	pMessage->pData = (u_char*)malloc(pMessage->size);
	read(fd, (void*)pMessage->pData, pMessage->size);

	close(fd);
}

#include "child.h"

#include "circleQueue.h"

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

/*
// read CircledQueue from the shared Memory
int openCircledQueue(u_char** pHeapMemory, off_t& sizeMemory, CircleHead** pCircleHead)
{
	printf("openCircledQueue ST\n");

	int i;
	struct stat sb;
	int fd;

	const char* nameFile = NAME_FILE_CIRCLE_QUEUE;

	fd = shm_open(nameFile, O_RDWR, 0644);
	if (fd < 0)
	{
		return fd;
	}

	fstat(fd, &sb);
	off_t sizeFile = sb.st_size;

	*pHeapMemory = (u_char*)mmap(NULL, sizeFile, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	sizeMemory = sizeFile;
	fprintf(stderr, "sizeFile=%ld\n", sizeFile);

	// init
	CircleHead*		pHead;
	CircleElement*	pBuffer;

	pHead = (CircleHead*)*pHeapMemory;
	pBuffer = (CircleElement*)(*pHeapMemory + sizeof(CircleHead));
	pHead->pBuf = pBuffer;			// !!!!!!!!!


	printf("countElement: %d\n", pHead->size);
	printf("indexHead: %d\n", pHead->indexHead);

	*pCircleHead = pHead;
	printf("openCircledQueue OK\n");
}
//*/
// Porducer ( or Consumer ) works with Message
bool inProcess = false;

void signalHandler(int numberSignal)
{
	fprintf(stderr, "signalHandler: %d\n", numberSignal);

	if(numberSignal != SIGUSR1)
	{
		fprintf(stderr, "Unexpected Signal number: %d\n", numberSignal);
		return;
	}
	if (inProcess == false)
	{ // application wain on the Mutex
		printf("TERMINATE on Mutex\n");
		exit(0);
	}
	isExit = true;
}

bool connectExitSignal()
{
	printf("connectExitSignal ST\n");

	int					result;
	struct sigaction	action;

	action.sa_handler = signalHandler;
	action.sa_flags = 0;

	//=== connect to Signals ===

	result = sigaction(SIGUSR1, &action, NULL);						// SIGUSR1
	if (result == -1)
	{
		printf("cannot connect to Signal: SIGUSR1\n");

		printf("connectExitSignal OK\n");
		return false;
	}
	printf("connectExitSignal OK\n");

	return true;
}


sem_t* openSemaphore(const char* nameSemaphore)
{
	sem_t* pSemaphore;
	pSemaphore = sem_open(nameSemaphore, O_CREAT);
	if (pSemaphore == SEM_FAILED)
	{
		return NULL;
	}
	return pSemaphore;
}
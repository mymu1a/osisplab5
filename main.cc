#include "global.h"

#include "circleQueue.h"

#include <ctype.h>
#include <dirent.h>
#include <semaphore.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/mman.h>
#include <sys/stat.h>	/* For mode constants */
#include <fcntl.h>		/* For O_* constants */
#include <sys/wait.h>


#define SIZE_MESSAGE_QUEUE 8


//=== Global Variables ===
TAILQ_HEAD(HeadOfCollection, Child)
	colProducer = TAILQ_HEAD_INITIALIZER(colProducer),
	colConsumer = TAILQ_HEAD_INITIALIZER(colConsumer);


//===== Functions =====
int createChild(unsigned indexChild, char* dirChild, TYPE_CHILD childType);
CircleHead* createColMessage();
void onExit();
sem_t* openSemaphore(const char* nameSemaphore, unsigned value);
void stopChildAll(TYPE_CHILD type);
void* threadConsumer(void* pData);
void* threadProducer(void* pData);



int main(int argc, char* argv[], char* envp[])
{
	printf("main ST\n");

	char dirChild[] = "./";						/////


	sem_t* pSemaphoreProducer;

	pSemaphoreProducer = openSemaphore(SEM_PRODUSER_NAME, SEM_PRODUSER_VALUE);
	if (pSemaphoreProducer == NULL)
	{
		printf("Error: cannot open Semaphore\n");
		return 1;
	}
	sem_t* pSemaphoreConsumer;

	pSemaphoreConsumer = openSemaphore(SEM_CONSUMER_NAME, SEM_CONSUMER_VALUE);
	if (pSemaphoreConsumer == NULL)
	{
		printf("Error: cannot open Semaphore\n");
		return 1;
	}
	printf("Type: p c k q\n");

	//=== read user input ===
	int ch;
	unsigned	indexProduser = 0;
	unsigned	indexConsumer = 0;
	CircleHead* pCircleHead;

	TAILQ_INIT(&colProducer);                   /* Initialize the queue. */
	TAILQ_INIT(&colConsumer);                   /* Initialize the queue. */

	pCircleHead = createColMessage();			// initialize circled collection for Messages

	while ((ch = getchar()) != EOF)
	{
		//=== select command ===
		switch (ch)
		{
		case 'p':
		{
			struct Child* pChild;
			
			pChild = (Child*)malloc(sizeof(struct Child));

			pChild->index = indexProduser;
			pChild->pCircleHead = pCircleHead;
			pChild->isExit = false;
			pChild->inProcess = false;
			sprintf(pChild->nameProgram, "%s_%02d", CHILD_PRODUSER_PROGRAM, indexProduser);

			pthread_create(&pChild->idThread, NULL, &threadProducer, (void*)pChild);


			TAILQ_INSERT_TAIL(&colProducer, pChild, allChildren);

			printf("Start Producer: %d\n", indexProduser);
			indexProduser++;

			break;
		}
		case 'c':
		{
			struct Child* pChild;

			pChild = (Child*)malloc(sizeof(struct Child));

			pChild->index = indexConsumer;
			pChild->pCircleHead = pCircleHead;
			pChild->isExit = false;
			pChild->inProcess = false;
			sprintf(pChild->nameProgram, "%s_%02d", CHILD_CONSUMER_PROGRAM, indexConsumer);

			pthread_create(&pChild->idThread, NULL, &threadConsumer, (void*)pChild);


			TAILQ_INSERT_TAIL(&colConsumer, pChild, allChildren);

			printf("Start Consumer: %d\n", indexConsumer);
			indexConsumer++;

			break;
		}
		case 'k':								// terminate all Child processes
		{
			onExit();							// clear resources
			TAILQ_INIT(&colProducer);           /* Initialize the queue. */
			TAILQ_INIT(&colConsumer);           /* Initialize the queue. */
			break;
		}
		case 'q':
		{
			onExit();							// clear resources

			printf("main OK quit\n");
			return 0;
		}
		case 'z':						// kill zombie processes
		{
			int stat;

			while (waitpid(-1, &stat, WUNTRACED) > 0)
			{
			}
			break;
		}
		}
	}
	printf("main OK main");
	onExit();							// clear resources
	
	// wait for children termination
	while (wait(NULL) > 0)
	{
	}

	return 0;
}
// clear resources
void onExit()
{
	stopChildAll(TC_PRODUCER);			// terminate all Child processes
	stopChildAll(TC_CONSUMER);			// terminate all Child processes
}

void stopChild(pid_t pidChild)
{
	kill(pidChild, SIGUSR1);
}

void stopChildAll(TYPE_CHILD type)
{
	struct HeadOfCollection* pColl = NULL;
	const char* message;

	if (type == TC_PRODUCER)
	{
		pColl = &colProducer;
		message = "Terminate Producer: %d\n";
	}
	else
	{
		pColl = &colConsumer;
		message = "Terminate Consumer: %d\n";
	}

	// delete from the collection
	for (struct Child* pChild = pColl->tqh_first; pChild != NULL; )
	{
		struct Child* pChildNext;

		pChildNext = pChild->allChildren.tqe_next;

		printf(message, pChild->index);
///		stopChild(pChild->pid);						// send signal to Child
		free(pChild);

		pChild = pChildNext;
	}
}

// initialize circled collection for Messages
CircleHead* createColMessage()
{
	size_t		sizeColl;
	void*		pHeapMemory;
	CircleHead* pHead;

	sizeColl = sizeof(CircleHead) + sizeof(CircleElement) * SIZE_MESSAGE_QUEUE;
	pHeapMemory = malloc(sizeColl);
	pHead = (CircleHead*)pHeapMemory;

	circleQueueInit(pHead, SIZE_MESSAGE_QUEUE);

	return pHead;
}

sem_t* openSemaphore(const char* nameSemaphore, unsigned value)
{
	sem_t* pSemaphore;
	pSemaphore = sem_open(nameSemaphore, O_CREAT, 0660, value);
	if (pSemaphore == SEM_FAILED)
	{
		return NULL;
	}
	return pSemaphore;
}

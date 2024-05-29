#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <cerrno>
#include <cstdio>
#include <unistd.h>

#include <sys/queue.h>
#include <sys/types.h>


#define CHILD_PRODUSER_PROGRAM "produser"
#define CHILD_CONSUMER_PROGRAM "consumer"
#define CHILD_DIR "CHILD_PATH"

#define NAME_FILE_CIRCLE_QUEUE "circleQueue"
#define MESSAGE_FOLDER "messages"

#define SEM_PRODUSER_NAME "lab4_sem_producer"
#define SEM_PRODUSER_VALUE 1
#define SEM_CONSUMER_NAME "lab4_sem_consumer"
#define SEM_CONSUMER_VALUE 2

enum TYPE_CHILD { TC_PRODUCER, TC_CONSUMER };


struct Message
{
	unsigned char type;
	short hash;
	unsigned char size;
	u_char* pData;
};

struct CircleHead;
struct Child
{
	unsigned	index;					// child index
	char		nameProgram[12];
	bool		isExit;					// sugest Porducer ( or Consumer ) to exit
	bool		inProcess;				// Porducer ( or Consumer ) works with Message
	pthread_t	idThread;
	CircleHead* pCircleHead;

	TAILQ_ENTRY(Child) allChildren;         /* Tail queue. */
};

#endif // __GLOBALS_H__

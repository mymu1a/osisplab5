#ifndef __CHILD_H__
#define __CHILD_H__
#include <sys/types.h>
#include <semaphore.h>

struct CircleHead;


bool connectExitSignal();
void closeCircledQueue(u_char* pHeapMemory, off_t& sizeMemory);							// close CircledQueue
int openCircledQueue(u_char** pHeapMemory, off_t& sizeMemory, CircleHead** pHead);		// read CircledQueue from the shared Memory
sem_t* openSemaphore(const char* nameSemaphore);

#endif // __CHILD_H__

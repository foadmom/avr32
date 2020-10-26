
#include "common.h"
#include "csemaphore.h"

volatile static uchar genericSemaphores [MAX_NO_OF_SEMAPHORES];

void SEM_init ()
{
	uint index = 0;
	for (index=0; index<MAX_NO_OF_SEMAPHORES; index++)
	{
		genericSemaphores [index] = 0;
	}
}


 


#ifndef CSEMAPHORE_H_
#define CSEMAPHORE_H_

#include "common.h"
#include "ctc.h"

#define MAX_NO_OF_SEMAPHORES		16
#define CBUFFER_SEMAPHORE			genericSemaphores[0];

void SEM_init ();

__always_inline uint SEM_get (volatile uchar sem)
{
	while (sem != 0) wait_ms (1);
	Disable_global_interrupt ();
	++sem;
	Enable_global_interrupt ();
	return 0;
}


__always_inline uint SEM_put (volatile uchar sem)
{
	Disable_global_interrupt ();
	--sem;
	Enable_global_interrupt ();
	return sem;
}


#endif /*CSEMAPHORE_H_*/

#ifndef CYCLICBUFFER_H_
#define CYCLICBUFFER_H_

#include "common.h"

typedef struct cBuffer_struct 
{
	ushort       BUFFER_SIZE;
	ushort       currentReadPos;
	ushort       currentWritePos;
	ushort		 count;
    char*        buffer;
} cBuffer;

void bufferInit 	(volatile cBuffer* buffer);
char bufferGetc 	(volatile cBuffer* buffer);
uint bufferGets 	(volatile cBuffer* buffer, char* s);
void bufferPutc 	(volatile cBuffer* buffer, char c);
uint bufferPuts 	(volatile cBuffer* buffer, char* s);
uint isBufferEmpty 	(volatile cBuffer* buffer);

__always_inline int bufferCapacity (volatile cBuffer* buffer)
{
	return (buffer->BUFFER_SIZE - buffer->count);
}
#endif /*CYCLICBUFFER_H_*/

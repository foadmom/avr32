// ---------------------------------------------------------
// these simple cyclic buffer have no overwrite protection.
// use them with care.
// ---------------------------------------------------------
//
#include "cyclicBuffer.h"

void bufferInit (volatile cBuffer* buffer)
{
    uint index = buffer->BUFFER_SIZE;
    do
    {
        --index;
        buffer->buffer[index] = 0;
    } while (index > 0);

    buffer->currentReadPos = 0;
    buffer->currentWritePos = 0;
    buffer->count = 0;
}

// ===================================================
// ===================================================
// ===================================================
// ===================================================
// 
// ===================================================
char bufferGetc (volatile cBuffer* buffer)
{
    char c = 0;
    if (buffer->count > 0)
    {
        c = buffer->buffer[buffer->currentReadPos];
        --buffer->count;
        ++buffer->currentReadPos;
	    // take care of wrapping around of the pointer by 0ing the Position
        buffer->currentReadPos &= buffer->BUFFER_SIZE-1;
    }
    return c;
}

// ===================================================
// assumes that s is large enough to hold the string.
// so if the buffer is full then string should be the
// size of the buffer+1
// ===================================================
uint bufferGets (volatile cBuffer* buffer, char* s)
{
    uint count = 0;
    char c = bufferGetc (buffer);
    while ( (c != 0) && (c != '\n') && (c != '\r') )
    {
        *s = c;
        ++s;
        ++count;
        c = bufferGetc (buffer);
    }
    *s = 0; // this is the terminating zero for the C string
    return count;
}


// ===================================================
// 
// ===================================================
void bufferPutc (volatile cBuffer* buffer, char c)
{
	if (buffer->count < buffer->BUFFER_SIZE)
	{
	    buffer->buffer[buffer->currentWritePos] = c;
	    ++buffer->count;
	    ++buffer->currentWritePos;
	    // take care of wrapping around of the pointer by 0ing the Position
	    buffer->currentWritePos &= buffer->BUFFER_SIZE-1;
	}
}

// ===================================================
// 
// ===================================================
uint bufferPuts (volatile cBuffer* buffer, char* s)
{
    uint  count = 0;
    // is there room in the buffer for the whole string ?
	if (strlen (s) < bufferCapacity (buffer) )
	{
	    char c = *s;
	    while (c != 0)
	    {
	        bufferPutc (buffer, c);
	        ++s;
	        ++count;
	        c = *s;
	    }
	}
    return count;
}

// ===================================================
// 
// ===================================================
uint isBufferEmpty (volatile cBuffer* buffer)
{
	return ((buffer->count==0) ? 1 : 0 );
}


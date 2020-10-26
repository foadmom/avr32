#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <string.h>

#include "compiler.h"
#include "error.h"

#define __DEBUG__	1

#define null		(void*)0
#define FOSC0		12000000

//typedef unsigned int		uint;
typedef unsigned char		uchar;
//typedef unsigned short		ushort;



#if __GNUC__
#define __always_inline  __attribute__((__always_inline__))  extern __inline__ 
#elif __ICCAVR32__ 
#define __always_inline  extern __inline__ 
#endif

typedef void (*function)(void);


#endif /*COMMON_H_*/

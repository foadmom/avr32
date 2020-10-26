#ifndef CUSART_H_
#define CUSART_H_

#include <avr32/io.h>
#include "common.h"
#include "cyclicBuffer.h"


void debugTx1Buffer ();
void debugRx1Buffer ();

#define rxBufferLimit     0x000000FF
#define rxBufferSize      rxBufferLimit+1

#define txBufferLimit     0x000003FF
#define txBufferSize      txBufferLimit+1

#define USART_DEFAULT_TIMEOUT   10000

#define USART_SUCCESS           0 //!< Successful completion.
#define USART_FAILURE          -1 //!< Failure because of some unspecified reason.
#define USART_INVALID_INPUT     1 //!< Input value out of range.
#define USART_INVALID_ARGUMENT -1 //!< Argument value out of range.
#define USART_TX_BUSY           2 //!< Transmitter was busy.
#define USART_RX_EMPTY          3 //!< Nothing was received.
#define USART_RX_ERROR          4 //!< Transmission error occurred.
#define USART_MODE_FAULT        5 //!< USART not in the appropriate mode.

typedef struct usartStats_struct
{
	ushort	bytesSent;
	ushort	bytesReceived;
	ushort	dataReceived;  // this is messages, cr or lf separated, messages
} usartStats; 


// -----------------------------------------------------------
// -----------------------------------------------------------
// -----------------------------------------------------------
// -----------------------------------------------------------
// -----------------------------------------------------------
// -----------------------------------------------------------
//
// -----------------------------------------------------------
__always_inline void disableTxRdyInterrupt (volatile avr32_usart_t *usart)
{
    usart->IDR.txrdy = 1;
}

// -----------------------------------------------------------
//
// -----------------------------------------------------------
__always_inline void enableTxRdyInterrupt (volatile avr32_usart_t *usart)
{
    usart->IER.txrdy = 1;
}


// -----------------------------------------------------------
//
// -----------------------------------------------------------
__always_inline void disableRxRdyInterrupt (volatile avr32_usart_t *usart)
{
    usart->IDR.rxrdy = 1;
}

// -----------------------------------------------------------
//
// -----------------------------------------------------------
__always_inline void enableRxRdyInterrupt (volatile avr32_usart_t *usart)
{
    usart->IER.rxrdy = 1;
}

// -----------------------------------------------------------
//
// -----------------------------------------------------------
__always_inline int isTxRdy (volatile avr32_usart_t *usart)
{
    return (usart->CSR.txrdy);
}


// -----------------------------------------------------------
//
// -----------------------------------------------------------
__always_inline int isRxRdy (volatile avr32_usart_t *usart)
{
    return (usart->CSR.rxrdy);
}


// -----------------------------------------------------------
// -----------------------------------------------------------
// -----------------------------------------------------------
// -----------------------------------------------------------
// -----------------------------------------------------------
// -----------------------------------------------------------
//
// -----------------------------------------------------------
__always_inline void usartSetcharLen (volatile avr32_usart_t *usart, uint charLen)
{
	usart->mr |= (charLen - 5) << AVR32_USART_MR_CHRL_OFFSET;
}


// -----------------------------------------------------------
//
// -----------------------------------------------------------
__always_inline void usartSetParity (volatile avr32_usart_t *usart, uint parity)
{
    usart->mr |= parity << AVR32_USART_MR_PAR_OFFSET;
}

// -----------------------------------------------------------
//
// -----------------------------------------------------------
__always_inline void usartSetChannelMode (volatile avr32_usart_t *usart, uint channelMode)
{
	volatile avr32_usart_t    *usart1_ = (&AVR32_USART1);
//    usart->mr |= channelMode << AVR32_USART_MR_CHMODE_OFFSET;
    usart1_->mr |= channelMode << AVR32_USART_MR_CHMODE_OFFSET;
}

// -----------------------------------------------------------
//
// -----------------------------------------------------------
__always_inline void usartStopBits (volatile avr32_usart_t *usart, uint stopBits)
{
	usart->mr |= stopBits << AVR32_USART_MR_NBSTOP_OFFSET;
}

// -----------------------------------------------------------
//
// -----------------------------------------------------------
__always_inline void usartEnableRx (volatile avr32_usart_t *usart)
{
    usart->cr |= AVR32_USART_CR_RXEN_MASK;
}

// -----------------------------------------------------------
//
// -----------------------------------------------------------
__always_inline void usartDisableRx (volatile avr32_usart_t *usart)
{
    usart->cr |= AVR32_USART_CR_RXDIS_MASK;
}

// -----------------------------------------------------------
//
// -----------------------------------------------------------
__always_inline void usartEnableTx (volatile avr32_usart_t *usart)
{
	usart->cr |= AVR32_USART_CR_TXEN_MASK;
}

// -----------------------------------------------------------
//
// -----------------------------------------------------------
__always_inline void usartDisableTx (volatile avr32_usart_t *usart)
{
	usart->cr |= AVR32_USART_CR_TXDIS_MASK;
}


// -----------------------------------------------------------
// -----------------------------------------------------------
// -----------------------------------------------------------
// -----------------------------------------------------------
// -----------------------------------------------------------
// write a single char into transmit register
// -----------------------------------------------------------
__always_inline void usartWriteChar(volatile avr32_usart_t *usart, uchar c)
{
	usart->thr = c;
}

// -----------------------------------------------------------
// read a single char from receive register
// -----------------------------------------------------------
__always_inline char usartReadChar(volatile avr32_usart_t *usart)
{
	return (usart->RHR.rxchr);
}

#define debugf(format, args...) \
	{if (__DEBUG__) {char __str[128]; sprintf (__str, format, args);USART1_WriteLine ((const char*) __str);}}

#define debug(__str)      {if (__DEBUG__) {USART1_WriteLine ((const char*) __str);}}



void 	USART1_initialise ();
void 	USART1_configurePort (uint baudRate, uint charLen, uint parityMode, uint channelMode, uint stopBits);

uint 	USART1_getBytesReceived ();
uint 	USART1_getBytesSent ();
void 	USART1_initStats ();

uint 	USART1_WriteLine (const char *string);
uint 	USART1_ReadLine (char* string);
void 	USART1_ResetBuffers ();

void 	USART1_EnableRx ();
void 	USART1_DisableRx ();

void 	USART1_EnableTx ();
void 	USART1_DisableTx ();

uint 	USART1_isDataReady ();

#define USART1_setBuadRate(baudrate)		usartSetBaudrate(usart1,baudrate,FOSC0)
#define USART1_setCharLen(charLen)			usartSetcharLen(usart1,charLen)
#define USART1_setParity(parity)			usartSetParity(usart1,parity)
#define USART1_setChannelMode(channelMode)	usartSetChannelMode(usart1,channelMode)
#define USART1_setStopBits(stopBits) 		usartStopBits(usart1,stopBits)

#define USART1_enableRxInt()				enableRxRdyInterrupt(usart1)
#define USART1_disableRxInt()				disableRxRdyInterrupt(usart1)

#define USART1_enableTxInt()				enableTxRdyInterrupt(usart1)
#define USART1_disableTxInt()				disableTxRdyInterrupt(usart1)

#endif /*CUSART_H_*/

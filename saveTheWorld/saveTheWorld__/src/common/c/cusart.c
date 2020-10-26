
#include "cusart.h"
#include "cintc.h"

volatile static avr32_usart_t   *usart1 = (&AVR32_USART1);
volatile static usartStats		usart1Stats;

static char 		 rxBuffer1[rxBufferSize];
static cBuffer	     usart1_rxBuffer; 

static char 		 txBuffer1[txBufferSize];
static cBuffer	     usart1_txBuffer; 


// -----------------------------------------------------------
// some buffer debug functions
// -----------------------------------------------------------
void debugBuffer (cBuffer _buffer)
{
	debugf ("size=%d  rPos=%d  wPos=%d  count=%d bPtr=%d%s",
			      _buffer.BUFFER_SIZE,
			               _buffer.currentReadPos,
			                        _buffer.currentWritePos,
			                                  _buffer.count,
			                                           (int)_buffer.buffer, NEWLINE);

}

void debugTx1Buffer ()
{
	debugBuffer (usart1_txBuffer);
}

void debugRx1Buffer ()
{
	debugBuffer (usart1_rxBuffer);
}


// -----------------------------------------------------------
// status and buffer functions
// -----------------------------------------------------------
void setupBuffer (cBuffer* _bufferObject, ushort _bufferSize, char* _buffer)
{
	_bufferObject->BUFFER_SIZE = _bufferSize;
	_bufferObject->buffer = _buffer;
}

// -----------------------------------------------------------
// status and buffer functions
// -----------------------------------------------------------
uint USART1_getBytesReceived ()
{
	return usart1Stats.bytesReceived;
}

uint USART1_getBytesSent ()
{
	return usart1Stats.bytesSent;
}

void USART1_initStats ()
{
	usart1Stats.bytesReceived = 0;
	usart1Stats.bytesSent = 0;
	usart1Stats.dataReceived = 0;
}

void USART1Interrupt ()
{
	// check if it was transmitter that set off the interrups
	if (isTxRdy(usart1) )
	{
	    char c = bufferGetc (&usart1_txBuffer);
	    if (c)
	    {
	    	usartWriteChar(usart1, c);
	    	++usart1Stats.bytesSent;
	    }
	    else USART1_disableTxInt ();
	}
	// check if it was receiver that set off the interrups
	while (isRxRdy(usart1))
	{
	    char c = usartReadChar (usart1);
	    bufferPutc (&usart1_rxBuffer, c);
	    ++usart1Stats.bytesReceived;
	    if ( (c == '\n') || (c == '\r') )
	    {
	    	++usart1Stats.dataReceived;
	    }
	}
}

// -----------------------------------------------------------
// usart interrupt routine
// -----------------------------------------------------------
__isr__ static void usart1_intHandler(void)
{
	USART1Interrupt ();
}

// -----------------------------------------------------------
// if there is any data for usart1 then return truedebugf ("char rcved=%d\n", c);

// -----------------------------------------------------------
uint USART1_isDataReady ()
{
	uint ready = usart1Stats.dataReceived;

	if (usart1Stats.dataReceived > 0)
	{
		USART1_disableRxInt ();
		--usart1Stats.dataReceived;
		USART1_enableRxInt ();
	}
	return ready;
}

// -----------------------------------------------------------
// -----------------------------------------------------------
// -----------------------------------------------------------
// -----------------------------------------------------------
// -----------------------------------------------------------
//
// -----------------------------------------------------------
void usartSetBaudrate (volatile avr32_usart_t *usart, uint baudrate, long pba_hz)
{
    // Clock divider.
    uint cd;

        // Baudrate calculation.
    if (baudrate < pba_hz / 16)
    {
        // Use 16x oversampling, clear SYNC bit.
        usart->mr &=~ (AVR32_USART_MR_OVER_MASK | AVR32_USART_MR_SYNC_MASK);
        cd = (pba_hz + 8 * baudrate) / (16 * baudrate);
    }
    else if (baudrate < pba_hz / 8)
    {
        // Use 8x oversampling.
        usart->mr |= AVR32_USART_MR_OVER_MASK;
        // clear SYNC bit
        usart->mr &=~ AVR32_USART_MR_SYNC_MASK;

        cd = (pba_hz + 4 * baudrate) / (8 * baudrate);
    }
    else
    {
        // set SYNC to 1
        usart->mr |= AVR32_USART_MR_SYNC_MASK;
        // use PBA/BaudRate
        cd = (pba_hz / baudrate);
    }
    usart->brgr = cd << AVR32_USART_BRGR_CD_OFFSET;
}

// -----------------------------------------------------------
//
// -----------------------------------------------------------
void usartReset (volatile avr32_usart_t *usart)
{
	usartDisableTx (usart1);
	usartDisableRx (usart1);
	
    // Disable all USART interrupts.
    // Interrupts needed should be set explicitly on every reset.
    usart->idr = 0xFFFFFFFF;

    // Reset mode and other registers that could cause unpredictable behavior after reset.
    usart->mr = 0;
    usart->rtor = 0;
    usart->ttgr = 0;

    // Shutdown TX and RX (will be re-enabled when setup has successfully completed),
    // reset status bits and turn off DTR and RTS.
    usart->cr = AVR32_USART_CR_RSTRX_MASK   |
                AVR32_USART_CR_RSTTX_MASK   |
                AVR32_USART_CR_RSTSTA_MASK  |
                AVR32_USART_CR_RSTIT_MASK   |
                AVR32_USART_CR_RSTNACK_MASK |
                AVR32_USART_CR_DTRDIS_MASK  |
                AVR32_USART_CR_RTSDIS_MASK;
}

// ----------------------------------------------------------
// ----------------------------------------------------------
// ----------------------------------------------------------
// ----------------------------------------------------------
// setup port 
// ----------------------------------------------------------
void USART1_configurePort (uint baudRate, uint charLen, uint parityMode, uint channelMode, uint stopBits)
{
	USART1_setBuadRate (baudRate);
	USART1_setCharLen (charLen);
	USART1_setParity (parityMode);
	USART1_setChannelMode (channelMode);
	USART1_setStopBits (stopBits);
}

// ----------------------------------------------------------
// setup usart1 interrupts and send and receive buffers
// ----------------------------------------------------------
void USART1_initialise ()
{
	setupBuffer (&usart1_rxBuffer, rxBufferSize, rxBuffer1);
	setupBuffer (&usart1_txBuffer, txBufferSize, txBuffer1);
	usartReset (usart1);
	USART1_ResetBuffers ();
	INTC_register_interrupt(&usart1_intHandler, AVR32_USART1_IRQ, USART_INT_LEVEL);
}

// ----------------------------------------------------------
// reset everything to do with usart1
// ----------------------------------------------------------
void USART1_ResetBuffers ()
{
	USART1_initStats ();
	bufferInit (&usart1_rxBuffer);
	bufferInit (&usart1_txBuffer);
}

// -----------------------------------------------------------
//
// -----------------------------------------------------------
void USART1_EnableTx ()
{
	USART1_enableTxInt ();
    usartEnableTx (usart1);
}

// -----------------------------------------------------------
//
// -----------------------------------------------------------
void USART1_DisableTx ()
{
    usartDisableTx (usart1);
    USART1_disableTxInt ();
}

// -----------------------------------------------------------
// -----------------------------------------------------------
// -----------------------------------------------------------
//
// -----------------------------------------------------------
void USART1_EnableRx ()
{
	USART1_enableRxInt ();
    usartEnableRx (usart1);
}

// -----------------------------------------------------------
//
// -----------------------------------------------------------
void USART1_DisableRx ()
{
    usartDisableRx (usart1);
    USART1_disableRxInt ();
}

// -----------------------------------------------------------
// -----------------------------------------------------------
// -----------------------------------------------------------
// -----------------------------------------------------------
// -----------------------------------------------------------
// -----------------------------------------------------------
// -----------------------------------------------------------
// -----------------------------------------------------------
// Note this function should not be used from inside 
// the interrupt routines.
// -----------------------------------------------------------
void USART1_debug (const char * format, ... )
{
	char str [256];
	USART1_WriteLine (str);
}

// -----------------------------------------------------------
// Note this function should not be used from inside 
// the interrupt routines.
// -----------------------------------------------------------
uint USART1_Write (const char *string)
{
	USART1_disableTxInt ();
	int count = bufferPuts (&usart1_txBuffer, (char*) string);
	USART1_enableTxInt ();
	return count;
}

// -----------------------------------------------------------
// Note this function should not be used from inside
// the interrupt routines.
// -----------------------------------------------------------
uint USART1_WriteLine (const char *string)
{
	USART1_disableTxInt ();
	int count = bufferPuts (&usart1_txBuffer, (char*) string);
	count = count + bufferPuts (&usart1_txBuffer, (char*) NEWLINE);
	USART1_enableTxInt ();
	return count;
}

// -----------------------------------------------------------
//
// -----------------------------------------------------------
uint USART1_ReadLine (char* string)
{
	uint count = 0;
	USART1_disableRxInt ();
	count = bufferGets (&usart1_rxBuffer, string);
	--usart1Stats.dataReceived;
	USART1_enableRxInt ();
	return count;
}






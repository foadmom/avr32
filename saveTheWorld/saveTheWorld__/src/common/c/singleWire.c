//
// the single wire module is written for ds18x20 protocol
//
//
//

#include <avr32/io.h>

#include "singleWire.h"
#include "ctc.h"

#include "cusart.h"
#include "cgpio.h"
#include "crtc.h"

volatile static 	int singleIOPin = AVR32_PIN_PA03;  // used as default

// ---------------------------------------------------------------
// sets the output ping to be used for single wire comms.
// this is written for ds1820
// ---------------------------------------------------------------
void setSingleIOPin (int pin)
{
	singleIOPin = pin;
}

// releasing the bus is by setting the port as input
__always_inline void releaseBus (int pin)
{
	setGpioPinAsInput (pin);
//	gpioSetGlitchFilter (pin);
//	gpioEnablePullupResistor (signalIOPin);
}

//void pullBusLow (int duration)
//{
//	setGpioPinAsOutput (singleIOPin);
//	generateSinglePulse (SIGNAL_LOW, duration);
//}

// ---------------------------------------------------------------
// ---------------------------------------------------------------
// ---------------------------------------------------------------
//int generateSinglePulse (int level, int duration)
//{
//	int  _rc = 0;
//	if (level == SIGNAL_HIGH)
//	{
//		setLineHigh(singleIOPin);  // set the output high
//	}
//	else
//	{
//		setLineLow(singleIOPin); // set the output low
//	}
//	if (duration > 0)  // 0 duration means infinite
//	{
////		if (wait_usec (duration) != duration)
////		{
////			_rc = -1;
////		}
//	}
//	return _rc;
//}


// ---------------------------------------------------------------
// this signal needs to be 1 uSec and therefore too quick to
// measure. just calling this function takes more than 1 uSec
// ---------------------------------------------------------------
void signalIntentionToWrite ()
{
	setLineLow(singleIOPin); // set the output low
	return;
}

// ---------------------------------------------------------------
// write a single bit to the device
// it always starts with pulling the bus down for MASTER_WRITE_SIGNAL_DURATION usec
// if the bit is 0 then keep pulling it down for WRITE_SIGNAL_DURATION
// if the bit is 1 then after the MASTER_WRITE_SIGNAL_DURATION
//    release the bus to enable the pullup to push it high
// ---------------------------------------------------------------
void writeABit (int bit)
{
	setLineLow(singleIOPin); // set the output low
	if (bit != 0)
	{
		setLineHigh (singleIOPin);
	}
	wait_usec(WRITE_SIGNAL_DURATION);

	setLineHigh (singleIOPin);
//	releaseBus (singleIOPin);
//	wait_usec(RECOVERY_DURATION);
}

// ---------------------------------------------------------------
// get the bits from the lsb and use the WRITE_SIGNAL_DURATION to send it
// to write:
// 1. give the write pulse signal which is taking the line low of couple of usecs
// 2. generate the signal (high or low) for around 60 usecs.
// 3. free the line for recovery period of couple of usecs
// ---------------------------------------------------------------
void singleWireWriteAByte (uchar data)
{
	int  _index = 0;
	setGpioPinAsOutput (singleIOPin);
	for (_index=0; _index < 8; _index++)
	{
		int bit = data & 0x01;
		// 2. ignore everything but the lsb
		writeABit (bit);
		// shift to the right so the lsb+1 is now the lsb
		data = data >> 1;
	}
}

// ---------------------------------------------------------------
// ---------------------------------------------------------------
// ---------------------------------------------------------------
uint readOneBit ()
{
	uint _thisBit = 1;
	signalIntentionToRead (singleIOPin);

	releaseBus  (singleIOPin);
	wait_usec(READ_WAIT_FOR_SAMPLE);
	uchar _portValue = gpioGetPortValue (singleIOPin);
	if (_portValue == 0)
	{
		_thisBit = 0;
	}
	wait_usec(READ_WAIT_BEFORE_NEXT_READ);

	return _thisBit;
}

uchar readOneByte ()
{
	int _bitIndex = 0;
	char _data = 0;
	for (; _bitIndex<8; _bitIndex++)
	{
		_data = _data >> 1;
		uint  _nextBit = readOneBit ();
		// try reading halfway through pulse
		if (_nextBit != 0)
		{
			_data = _data | 0x80;
		}
//		wait_usec(RECOVERY_DURATION);
	}
	return _data;
}


// ---------------------------------------------------------------
// ---------------------------------------------------------------
// ---------------------------------------------------------------
// reset is pulling the bus low for minimum of 'RESET_DURATION' usec
// ---------------------------------------------------------------
//static int resetSignal [] = {1, 2000*uSec, 0, 480*uSec, 1, 60*uSec};
void sendResetSignal ()
{
	setGpioPinAsOutput (singleIOPin);
	setLineLow (singleIOPin);
	wait_usec (RESET_DURATION);
}

// ---------------------------------------------------------------
// here we turn the pin to input, wait for 15-60usec.
// which i call it 60, and read the port, wait another
// 10 usec and read it again to see if it is consistent.
// ---------------------------------------------------------------
int checkForPresencePulse ()
{
	int  _rc = 0;
	// change the port to input
	releaseBus (singleIOPin);
	int _counter = 10;
	for (; _counter>0; _counter--)
	{
		wait_usec (10); // check every xx usecs
		// read the port value. it should be low now
		if (gpioGetPortValue (singleIOPin) == 0)
		{
//			debugln ("checkForPresencePulse - 0");
			// if the line is pulled low by the ds18S20 then
			// this is the presence signal
			_rc++;
		}
		else
		{
//			debugln ("checkForPresencePulse - 1");
		}
	}
	return _rc;
}

int resetAndCheckPresence ()
{
	sendResetSignal ();
	int _rc = checkForPresencePulse();
	if (_rc > 0)
	{
		debugf ("ds18S20 is present %d%s", _rc, NEWLINE);
	}
	else
	{
		debugf ("ds18S20 is NOT present %d%s", _rc, NEWLINE);
	}
	return (_rc);
}
// ---------------------------------------------------------------
// ---------------------------------------------------------------
// ---------------------------------------------------------------

uchar* readScratchPad (uchar* data)
{
	if (resetAndCheckPresence () > 0)
	{
		singleWireWriteAByte (SKIP_ROM);
		singleWireWriteAByte (READ_SCRATCHPAD);
		int _index = 0;
		for (; _index<9; _index++)
		{
			*data = readOneByte ();
			++data;
		}
	}
	return (data);
}

int waitForConversionParasitePower ()
{
	releaseBus (singleIOPin);
	setGpioPinAsOutput  (singleIOPin);
	wait_msec   		(CONVERT_T_DURATION);
	return 0;
}

int waitForConversion ()
{
	int _rc = -1;
	releaseBus 	(singleIOPin);
	// ds1820 should now take the line low while conversion is in progress
	wait_msec (20);
	// lets check to see if the line is low
	int _line = gpioGetPortValue(singleIOPin);
	if (_line == 0)
	{
		int _loopCounter = 200;
		for (; _loopCounter > 0; _loopCounter--)
		{
			wait_msec (10);
			_line = gpioGetPortValue(singleIOPin);
			if (_line == 1)
			{
				_rc = 0;
				break;
			}
		}
	}
	return _rc;
}

uchar* getTemperatureNoRom (uchar* data)
{
//	writeDataToScratchPad (0x13, 0x25);
	if (resetAndCheckPresence () > 0)
	{
		singleWireWriteAByte  (SKIP_ROM);
		singleWireWriteAByte  (CONVERT_T);
//		if (waitForConversion() == 0)
		if (waitForConversionParasitePower() == 0)
		{
			readScratchPad     (data);
		}
	}
	return data;
}

void writeDataToScratchPad (uchar th, uchar tl)
{
	uchar data[] = {0,0,0,0,0,0,0,0,0,0,0,0};
	if (resetAndCheckPresence () > 0)
	{
		singleWireWriteAByte  (SKIP_ROM);
		singleWireWriteAByte  (WRITE_SCRATCHPAD);
		singleWireWriteAByte  (th);
		singleWireWriteAByte  (tl);
		readScratchPad     (data);
		debugf ("DS data = %x, %x, %x, %x, %x, %x, %x, %x, %x, %s",
					data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], NEWLINE);
	}
}

/*
 * sigleWire.h
 *
 *  Created on: Aug 25, 2011
 *      Author: foadm
 */

#ifndef SIGLEWIRE_H_
#define SIGLEWIRE_H_

#include "common.h"

#define SIGNAL_LOW				0
#define SIGNAL_HIGH				1
#define RECOVERY_SIGNAL			SIGNAL_LOW
#define WRITE_PULSE_SIGNAL		SIGNAL_LOW
#define INFINITE_DURATION		0
// the below durations are in usecs
// signals the start of read or write cycle
#define MASTER_BUS_SIGNAL_DURATION	3

// time needed for the signal to stablise
#define RECOVERY_DURATION			3

// the overall length of a read or write cycle
#define READ_WRITE_PULSE_DURATION	60

#define WRITE_SIGNAL_DURATION			60-MASTER_BUS_SIGNAL_DURATION
#define MASTER_WRITE_SIGNAL_DURATION	MASTER_BUS_SIGNAL_DURATION

#define MASTER_READ_SIGNAL_DURATION     MASTER_BUS_SIGNAL_DURATION
#define SAMPLE_AFTER_READ_SIGNAL		15
#define READ_WAIT_FOR_SAMPLE    		SAMPLE_AFTER_READ_SIGNAL-MASTER_READ_SIGNAL_DURATION-2
#define READ_WAIT_BEFORE_NEXT_READ		READ_WRITE_PULSE_DURATION-SAMPLE_AFTER_READ_SIGNAL

#define RESET_DURATION					480

// 'CONVERT_T_DURATION' is in msecs
#define CONVERT_T_DURATION		750

void setSingleIOPin 	(int pin);
void singleWireWriteAByte	(uchar data);
//#define setLineHigh(pin)         gpioSetPortValue(pin)
//#define setLineLow(pin)          gpioClearPortValue(pin)

void sendResetSignal ();
int  checkForPresencePulse ();
int  resetAndCheckPresence ();
void pullBusLow        (int duration);
int  generateSinglePulse   (int level, int duration);
void writeDataToScratchPad (uchar th, uchar tl);

uchar* getTemperatureNoRom ();

// ------------------------------------------------------
// ROM commands
#define SEARCH_ROM			0xf0
#define READ_ROM 			0x33
#define MATCH_ROM 			0x55
#define SKIP_ROM 			0xcc
#define ALARM_SEARCH 		0xec
// ------------------------------------------------------

// ------------------------------------------------------
// function commands
#define CONVERT_T 			0x44
#define WRITE_SCRATCHPAD 	0x4e
#define READ_SCRATCHPAD 	0xbe
#define COPY_SCRATCHPAD 	0x48
#define RECALL_EE 			0xb8
#define READ_POWER_SUPPLY 	0xb4

// this provides the pulse to signal the slave (ds18x20) that a read
// is about to follow and allow the bus to be driven by the slave
__always_inline void signalIntentionToRead (int pin)
{
	setGpioPinAsOutput (pin);
	gpioClearPortValue(pin);
//	pullBusLow (MASTER_READ_SIGNAL_DURATION);
}

// this provides the pulse to signal the slave (ds18x20) that a write
// is about to follow and allow the bus to be driven by the master
__always_inline void signalIntentionToWrite ()
{
	pullBusLow (MASTER_WRITE_SIGNAL_DURATION);
}

// assumes the port is already set to output
__always_inline void setLineLow (int pin)
{
//	setGpioPinAsOutput (singleIOPin);
	gpioClearPortValue(pin);
}

// assumes the port is already set to output
__always_inline void setLineHigh (int pin)
{
	gpioSetPortValue(pin);
//	setGpioPinAsInput (pin);
}



#endif /* SIGLEWIRE_H_ */

#ifndef CTC_H_
#define CTC_H_

#include <avr32/io.h>
#include "common.h"
#include "cintc.h"
#include "cpm.h"
#include "tc.h"
#include "singleWire.h"


//#define  __always_inline
#define TIMER_COUNTER_0				0
#define TIMER_COUNTER_1				1
#define TIMER_COUNTER_2				2

#define	TC_RC_COMP_COUNTER_INTERRUPT_INTERVAL		FOSC0

#define MSEC_COUNTER				 				TIMER_COUNTER_1
#define USEC_TIMER									TIMER_COUNTER_2
#define uSec(n)										n*TC_2_USEC_RESOLUTION

// ===================================
// channel 1 counter parameters
// ===================================
// these are the RC values worked out for AVR32_TC_CMR1_TCCLKS_TIMER_DIV4_CLOCK
// which makes the resolution of FOSC0(12000000)/DIV4(16) = 750,000
// so every 750 is one msecond
#define TC_1_MSEC_RESOLUTION		750
#define TC_1_TIMER_DIV_CLOCK		AVR32_TC_CMR1_TCCLKS_TIMER_DIV4_CLOCK

// ===================================
// channel 2 counter parameters
// ===================================
#define TC_2_TIMER_DIV_CLOCK	AVR32_TC_CMR2_TCCLKS_TIMER_DIV2_CLOCK
//#define TC_2_TIMER_DIV_CLOCK	AVR32_TC_CMR2_TCCLKS_TIMER_DIV1_CLOCK

// the actual resolution is 3 but for testing purposes it is changed to
// slower time interval for visual confirmations
#define TC_2_USEC_RESOLUTION		3
// #define TC_2_USEC_RESOLUTION		125

// ===================================

void wait_msec (uint msecs);
uint wait_usec (uint interval);

void TC_setInterruptHandler (uint channel);
void setTC0Array (int* array);
void setTC2Array (int* array);

// ----------------------------------------------------------
// set the external signals selection
// ----------------------------------------------------------
__always_inline void TC_setExtSig0Selection (uint source)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
	tc->BMR.tc0xc0s = source;
}

__always_inline void TC_setExtSig1Selection (uint source)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
	tc->BMR.tc1xc1s = source;
}

__always_inline void TC_setExtSig2Selection (uint source)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
	tc->BMR.tc2xc2s = source;
}


// ----------------------------------------------------------
// enables the synch trigger for all the channels
// ----------------------------------------------------------
__always_inline void TC_setSynch ()
{
	volatile avr32_tc_t *tc = &AVR32_TC;
	tc->BCR.sync = 1;
}

// ----------------------------------------------------------
// disables the synch trigger for all the channels
// ----------------------------------------------------------
__always_inline void TC_clearSynch ()
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->BCR.sync = 0;
}

// ----------------------------------------------------------
// ----------------------------------------------------------
// Control Mode Register CMR
// ----------------------------------------------------------
// ----------------------------------------------------------
// Set clock source
// ----------------------------------------------------------
__always_inline void TC_setClockSource (uint channel, uint source)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].CMR.capture.tcclks = source;
}

// ----------------------------------------------------------
// Set counter mode to capture
// ----------------------------------------------------------
__always_inline void TC_setModeCapture (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].CMR.capture.wave = 0;
}

// ----------------------------------------------------------
// Set counter mode to waveform
// ---------------------ctc-------------------------------------
__always_inline void TC_setModeWaveform (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].CMR.capture.wave = 1;
}

// ----------------------------------------------------------
// Set clock edge select
// ----------------------------------------------------------
__always_inline void TC_clockOnRisingEdge (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].CMR.capture.clki = 0;
}

// ----------------------------------------------------------
// Set clock edge select
// ----------------------------------------------------------
__always_inline void TC_clockOnFallingEdge (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].CMR.capture.clki = 1;
}


// ----------------------------------------------------------
// enable RC compare Trigger
// ----------------------------------------------------------
__always_inline void TC_enableRCCompareTrigger (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].CMR.capture.cpctrg = 1;
}

// ----------------------------------------------------------
// disable RC compare Trigger
// ---------------------ctc-------------------------------------
__always_inline void TC_disableRCCompareTrigger (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].CMR.capture.cpctrg = 0;
}


// ----------------------------------------------------------
// set RA loading selection
// ----------------------------------------------------------
__always_inline void TC_setRALoadingSelection (uint channel, uint value)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].CMR.capture.ldra = value;
}

// ----------------------------------------------------------
// set RB loading selection
// ----------------------------------------------------------
__always_inline void TC_setRBLoadingSelection (uint channel, uint value)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].CMR.capture.ldrb = value;
}

// ----------------------------------------------------------
// use TIOA as the external trigger
// ----------------------------------------------------------
__always_inline void TC_setExtTriggerToTIOA (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].CMR.capture.abetrg = 1;
}
// ----------------------------------------------------------
// use TIOB as the external trigger
// ----------------------------------------------------------
__always_inline void TC_setExtTriggerToTIOB (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].CMR.capture.abetrg = 0;
}


// ----------------------------------------------------------
// ----------------------------------------------------------
// ----------------------------------------------------------
// set RA Trigger Effect TIOA
// ----------------------------------------------------------
__always_inline void TC_setWaveformRATriggerEffect (uint channel, uint value)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].CMR.waveform.acpa = value;
}

// ----------------------------------------------------------
// set RB Trigger Effect TIOB
// ----------------------------------------------------------
__always_inline void TC_setWaveformRBTriggerEffect (uint channel, uint value)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].CMR.waveform.bcpb = value;
}

// ----------------------------------------------------------
// set RC Trigger Effect on TIOA
// ----------------------------------------------------------
__always_inline void TC_setWaveformRCTriggerEffect (uint channel, uint value)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].CMR.waveform.acpc = value;
}


// ----------------------------------------------------------
// set Event Edge
// ----------------------------------------------------------
__always_inline void TC_setEventEdge (uint channel, uint value)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].CMR.waveform.eevtedg = value;
}

// ----------------------------------------------------------
// set wave form selection WAVSEL
// ----------------------------------------------------------
__always_inline void TC_setWaveFormSelect (uint channel, uint value)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].CMR.waveform.wavsel = value;
}



// ----------------------------------------------------------
// ----------------------------------------------------------
// Channel Control Register CCR
// ----------------------------------------------------------
// ----------------------------------------------------------
// disable Counter
// ----------------------------------------------------------
__always_inline void TC_disableCounter (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].CCR.swtrg  = 0;
    tc->channel[channel].CCR.clken  = 0;
    tc->channel[channel].CCR.clkdis = 1;
}

// ----------------------------------------------------------
// enable Counter
// ----------------------------------------------------------
__always_inline void TC_enableCounter (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
//    tc->channel[channel].CCR.swtrg  = 1;
    tc->channel[channel].CCR.clkdis = 0;
    tc->channel[channel].CCR.clken  = 1;
}

// ----------------------------------------------------------
// trigger counter
// ----------------------------------------------------------
__always_inline void TC_TriggerCounter (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
//    TC_enableCounter (channel);
    tc->channel[channel].CCR.swtrg = 1;
}

// ----------------------------------------------------------
// set RA
// ----------------------------------------------------------
__always_inline void TC_setRA (uint channel, uint value)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].RA.ra = value & AVR32_TC_RA_MASK;
}
__always_inline uint TC_getRA (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    return (tc->channel[channel].RA.ra);
}


// ----------------------------------------------------------
// set RB
// ----------------------------------------------------------
__always_inline void TC_setRB (uint channel, uint value)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].RB.rb = value & AVR32_TC_RB_MASK;
}
__always_inline uint TC_getRB (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    return (tc->channel[channel].RB.rb);
}

// ----------------------------------------------------------
// set RC
// ----------------------------------------------------------
__always_inline void TC_setRC (uint channel, uint value)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].RC.rc = value & AVR32_TC_RC_MASK;
}

__always_inline uint TC_getRC (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    return (tc->channel[channel].RC.rc);
}


// ----------------------------------------------------------
// return the current value of the counter
// ----------------------------------------------------------
__always_inline uint TC_counterValue (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    return tc->channel[channel].cv;
}


// ----------------------------------------------------------
// ----------------------------------------------------------
// Interrupts
// ----------------------------------------------------------
// ----------------------------------------------------------
// enable Counter Overflow interrupt
// ----------------------------------------------------------
__always_inline void TC_enableOverflowInterrupt (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].IER.covfs = 1;
}

// ----------------------------------------------------------
// disable Counter Overflow interrupt
// ----------------------------------------------------------
__always_inline void TC_disableOverflowInterrupt (uint channel)
{
    volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].IDR.covfs = 1;
}

// ----------------------------------------------------------
// enable Counter Overflow interrupt
// ----------------------------------------------------------
__always_inline void TC_enableOverrunInterrupt (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].IER.lovrs = 1;
}

// ----------------------------------------------------------
// disable Counter Overflow interrupt
// ----------------------------------------------------------
__always_inline void TC_disableOverrunInterrupt (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].IDR.lovrs = 1;
}

// ----------------------------------------------------------
// enable Counter RA Compare interrupt
// ----------------------------------------------------------
__always_inline void TC_enableCompareRAInterrupt (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].IER.cpas = 1;
}

// ----------------------------------------------------------
// disable Counter RA Compare interrupt
// ----------------------------------------------------------
__always_inline void TC_disableCompareRAInterrupt (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].IDR.cpas = 1;
}

// ----------------------------------------------------------
// enable Counter RB Compare interrupt
// ----------------------------------------------------------
__always_inline void TC_enableCompareRBInterrupt (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
//    tc->channel[channel].IDR.cpbs = 0;
    tc->channel[channel].IER.cpbs = 1;
}

// ----------------------------------------------------------
// disable Counter RB Compare interrupt
// ----------------------------------------------------------
__always_inline void TC_disableCompareRBInterrupt (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].IDR.cpbs = 1;
}

// ----------------------------------------------------------
// enable Counter RC Compare interrupt
// ----------------------------------------------------------
__always_inline void TC_enableCompareRCInterrupt (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].IER.cpcs = 1;
}

// ----------------------------------------------------------
// disable Counter RC Compare interrupt
// ----------------------------------------------------------
__always_inline void TC_disableCompareRCInterrupt (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].IDR.cpcs = 1;
}

// ----------------------------------------------------------
// enable RA loading interrupt
// ----------------------------------------------------------
__always_inline void TC_enableRALoadingInterrupt (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].IER.ldras = 1;
}

// ----------------------------------------------------------
// disable RA loading interrupt
// ----------------------------------------------------------
__always_inline void TC_disableRALoadingInterrupt (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].IDR.ldras = 1;
}

// ----------------------------------------------------------
// enable RB loading interrupt
// ----------------------------------------------------------
__always_inline void TC_enableRBLoadingInterrupt (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].IER.ldrbs = 1;
}

// ----------------------------------------------------------
// disable RB loading interrupt
// ----------------------------------------------------------
__always_inline void TC_disableRBLoadingInterrupt (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].IDR.ldrbs = 1;
}

// ----------------------------------------------------------
// enable external trigger interrupt
// ----------------------------------------------------------
__always_inline void TC_enableExtTrigInterrupt (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].IER.etrgs = 1;
}

// ----------------------------------------------------------
// disable external trigger interrupt
// ----------------------------------------------------------
__always_inline void TC_disableExtTriggInterrupt (uint channel)
{
 	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].IDR.etrgs = 1;
}

// ----------------------------------------------------------
// enable timer/counter interrupts
// ----------------------------------------------------------
__always_inline void TC_enableInterrupts (uint channel, uint mask)
{
 	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].ier = mask;
}
// ----------------------------------------------------------
// disable timer/counter interrupts
// ----------------------------------------------------------
__always_inline void TC_disableInterrupts (uint channel, uint mask)
{
 	volatile avr32_tc_t *tc = &AVR32_TC;
    tc->channel[channel].idr = mask;
}

// ----------------------------------------------------------
// disable external trigger interrupt
// ----------------------------------------------------------
__always_inline uint TC_InterruptMask (uint _channel)
{
 	volatile avr32_tc_t *tc = &AVR32_TC;
    return (tc->channel[_channel].IMR.etrgs);
}

__always_inline uint TC_readCompareRCInterruptStatus (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
	return (tc->channel[channel].IER.cpcs);
}


// ----------------------------------------------------------
// ----------------------------------------------------------
// status register
// ----------------------------------------------------------
// ----------------------------------------------------------
__always_inline uint TC_readStatusRegister (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
	return (tc->channel[channel].sr);
}

__always_inline uint TC_getCompareRAStatus (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
	return (tc->channel[channel].SR.cpas);
}

__always_inline uint TC_getCompareRBStatus (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
	return (tc->channel[channel].SR.cpbs);
}

__always_inline uint TC_getCompareRCStatus (uint channel)
{
	volatile avr32_tc_t *tc = &AVR32_TC;
	return (tc->channel[channel].SR.cpcs);
}

//__always_inline void oneUSec ()
//{
//	NOP();
//}
//
//__always_inline void tenUSec ()
//{
//	oneUSec (); oneUSec (); oneUSec (); oneUSec (); oneUSec ();
//	oneUSec (); oneUSec (); oneUSec (); oneUSec (); oneUSec ();
//}
//
//__always_inline void hundredUSec ()
//{
//	tenUSec (); tenUSec (); tenUSec (); tenUSec (); tenUSec ();
//	tenUSec (); tenUSec (); tenUSec (); tenUSec (); tenUSec ();
//}

__always_inline uint blockedUSecTimer (uint usecs)
{
	uint _interval = usecs >> 1;
	for (;_interval > 0; _interval--)
	{
		NOP ();
	}
	return usecs;
}

__always_inline uint blockedMSecTimer (uint usecs)
{
	uint _interval = usecs*1000;
	for (;_interval > 0; _interval--)
	{
		NOP ();
	}
	return usecs;
}

__always_inline uint blockedSecTimer (uint usecs)
{
	uint _interval = usecs*1000000;
	for (;_interval > 0; _interval--)
	{
		NOP ();
	}
	return usecs;
}


#endif /*CTC_H_*/

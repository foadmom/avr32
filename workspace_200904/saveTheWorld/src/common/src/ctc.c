
#include "ctc.h"
#include "cusart.h"

static volatile uint S_TC_msecTimer = 1;
static volatile uint S_TC_usecTimer = 1;

// ----------------------------------------------------------
// timer counter interrupt routine for compare C register for channel 0
// ----------------------------------------------------------
__isr__ static void TC0_compareCIntHandler(void)
{
//debugf ("********** tc0  mask=%d\n", TC_InterruptMask(0));
//TC_setRA (0, (TC_getRA (2)+300) & 0x0000ffff);
//	if (S_TC_msecTimer > 0)
//	{
//    	--S_TC_msecTimer;
//	}
//	TC_clearCompareRCInterrupt (TIMER_COUNTER_0);
//	TC_TriggerCounter (TIMER_COUNTER_0);
	debugf ("tc0 interrupt ra=%u  rb=%u \n", TC_getRA (0), TC_getRB (0));
	TC_getCompareRAStatus (0);
	TC_getCompareRCStatus (0);
}

// ----------------------------------------------------------
// timer counter interrupt routine for compare C register for channel 1
// ----------------------------------------------------------
__isr__ static void TC1_compareCIntHandler(void)
{
	if (S_TC_usecTimer > 0)
	{
    	--S_TC_usecTimer;
	}
	TC_getCompareRCStatus (TIMER_COUNTER_1);
}

// ----------------------------------------------------------
// timer counter interrupt routine for compare C register for channel 2
// ----------------------------------------------------------
__isr__ static void TC2_compareCIntHandler(void)
{
debugf ("tc2 interrupt ra=%u  rb=%u \n", TC_getRA (2), TC_getRB (2));
	TC_getCompareRAStatus (2);
	TC_getCompareRBStatus (2);
	TC_getCompareRCStatus (2);
}

// ----------------------------------------------------------
// the reolution of this depends on the resolution of the interrupt
// which is 10msecs at the moment
// ----------------------------------------------------------
void wait_ms (uint msecs)
{
	setSleepValue (msecs);
	TC_enableCompareRCInterrupt (TIMER_COUNTER_0);
	TC_enableCounter (TIMER_COUNTER_0);
	TC_TriggerCounter (TIMER_COUNTER_0);
	while (S_TC_msecTimer > TIMER_COUNTER_0)
	{
		SLEEP (AVR32_PM_SMODE_FROZEN);
	}
	TC_disableCounter (TIMER_COUNTER_0);
	TC_disableCompareRCInterrupt (TIMER_COUNTER_0);
}



// ----------------------------------------------------------
// 
// ----------------------------------------------------------
void TC_setInterruptHandler (uint channel)
{
	if (channel == TIMER_COUNTER_0)
	{
		INTC_register_interrupt(&TC0_compareCIntHandler, AVR32_TC_IRQ0, TIMER_COUNTER_INT_LEVEL);
	}
	else if (channel == TIMER_COUNTER_1)
	{
		INTC_register_interrupt(&TC1_compareCIntHandler, AVR32_TC_IRQ1, TIMER_COUNTER_INT_LEVEL);
	}
	else if (channel == TIMER_COUNTER_2)
	{
		INTC_register_interrupt(&TC2_compareCIntHandler, AVR32_TC_IRQ2, TIMER_COUNTER_INT_LEVEL);
	}
}

// ----------------------------------------------------------
// sets a value for the sleep function in msecs
// ----------------------------------------------------------
void setSleepValue (uint msecs)
{
	S_TC_msecTimer = msecs;
}

// ----------------------------------------------------------
// gets the current value of S_TC_msecTimer
// ----------------------------------------------------------
uint getSleepValue ()
{
	return S_TC_msecTimer;
}


// ----------------------------------------------------------
// ----------------------------------------------------------
// ----------------------------------------------------------
// ----------------------------------------------------------
// this sets the compare register to allow timer interrupt
// after usecs (plus the excution time of the following lines).
// the result of the usecs*TC_1_USEC_RESOLUTION can not be more 
// than 16 bit value. it's the limit on the compare register, bloody
// 32 bit controllers having 16 bit registers, hah.
// ----------------------------------------------------------
void wait_us (uint usecs)
{
	setCounter1Value (usecs); // used as a flag
//	TC_setRC (TIMER_COUNTER_1, usecs*TC_1_USEC_RESOLUTION);
	TC_enableCompareRCInterrupt (TIMER_COUNTER_1);
	TC_enableCounter (TIMER_COUNTER_1);
	TC_TriggerCounter (TIMER_COUNTER_1);
	while (S_TC_usecTimer > 0)
	{
		SLEEP (AVR32_PM_SMODE_FROZEN);
	}
	TC_disableCounter (TIMER_COUNTER_1);
	TC_disableCompareRCInterrupt (TIMER_COUNTER_1);
}




// ----------------------------------------------------------
// sets a value for the sleep function in msecs
// ----------------------------------------------------------
void setCounter1Value (uint usecs)
{
	S_TC_usecTimer = usecs;
}

// ----------------------------------------------------------
// gets the current value of S_TC_msecTimer
// ----------------------------------------------------------
uint getCounter1Value ()
{
	return S_TC_usecTimer;
}







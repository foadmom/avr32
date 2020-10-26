
#include "ctc.h"
#include "cusart.h"
#include "cgpio.h"


static int* TC0_value_array;
static int* TC2_singnalArray;

void setTC0Array (int* array)
{
	TC0_value_array = array;
	TC_setRA(0, *TC0_value_array);
	++TC0_value_array;
	TC_setRC(0, *TC0_value_array);
	++TC0_value_array;
}

void setTC2Array (int* array)
{
	TC2_singnalArray = array;
}

int setTC2RCFromArray ()
{
	if (*TC2_singnalArray != -1)
	{
		TC_setRC(2, *TC2_singnalArray);
//		++TC2_value_array;
		return *TC2_singnalArray;
	}
	else return -1;
}

// ----------------------------------------------------------
// ----------------------------------------------------------
// ------------------- count channel 0 ----------------------
// ----------------------------------------------------------
// ----------------------------------------------------------
// timer counter 0 interrupt routine
// ----------------------------------------------------------
__isr__ static void TC0_interruptHandler(void)
{
	return;
	int _endOfSignals = 0;
	// once any part of the status register is read (through the structure)
	// then all stati are cleared. so we read the whole register and
	// extract what we want out of it by casting it to the structure
	uint _srint = TC_readStatusRegister (TIMER_COUNTER_0);
	avr32_tc_sr_t*  _sr = (avr32_tc_sr_t*) &_srint;

	// get the RC status to see if this was the cause of the interrupt
	uint _raStatus = _sr->cpas;
	if (_raStatus > 0)
	{
		if (*TC0_value_array != -1)
		{
			TC_setRA(0, *TC0_value_array);
			++TC0_value_array;
		}
	}

	uint _rcStatus = _sr->cpcs;
	if (_rcStatus > 0)
	{
		if (*TC0_value_array != -1)
		{
			TC_setRC(0, *TC0_value_array);
			++TC0_value_array;
		}
		else _endOfSignals = 1;
	}

	if (_endOfSignals != 0)
	{
		TC_disableCounter(0);
	    // enable GPIO control. this is to reset the PB00 to be an input again
		setPinToGpioFunction (AVR32_TC_A0_0_0_PIN);
		setGpioPinAsInput (AVR32_TC_A0_0_0_PIN);
	}
}


// ----------------------------------------------------------
// ----------------------------------------------------------
// ------------------- msec counter   -----------------------
// ----------------------------------------------------------
static volatile uint S_TC_msecTimer = 1;
// ----------------------------------------------------------
// timer counter interrupt routine for compare C register for channel 1
// ----------------------------------------------------------
__isr__ static void TC1_interruptHandler(void)
{
	if (S_TC_msecTimer > 0)
	{
    	--S_TC_msecTimer;
	}
	TC_getCompareRCStatus (TIMER_COUNTER_1);
}

// ----------------------------------------------------------
// this sets the compare register to allow timer interrupt
// after msecs (plus the excution time of the following lines).
// the result of the msecs can not be more than 16 bit value.
// it's the limit on the compare register, bloody
// 32 bit controllers having 16 bit registers, hah.
// ----------------------------------------------------------
void wait_msec (uint msecs)
{
	S_TC_msecTimer = msecs;
	TC_enableCompareRCInterrupt (MSEC_COUNTER);
	TC_enableCounter (MSEC_COUNTER);
	TC_TriggerCounter (MSEC_COUNTER);
	while (S_TC_msecTimer > 0)
	{
		SLEEP (AVR32_PM_SMODE_FROZEN);
	}
	TC_disableCounter (MSEC_COUNTER);
	TC_disableCompareRCInterrupt (MSEC_COUNTER);
}

// ----------------------------------------------------------
// ----------------------------------------------------------
// -------------- counter 2 ---------------------------------
// ----------------------------------------------------------
// ----------------------------------------------------------
// this is used as a way of signalling the counter interrupt
// to the main code.
// counter2TriggerStatus = 0 means the counter is running
// counter2TriggerStatus = 1 means the counter has reached the desired count.
// ----------------------------------------------------------
static volatile uint counter2ElapsedUsecs = 0;
// ----------------------------------------------------------
// timer counter interrupt routine for compare C register for channel 2
// this is used for waiting or timing intervals in usecs.
// ----------------------------------------------------------
//__isr__ static void TC2_interruptHandler(void)
//{
//	uint _srint = TC_readStatusRegister (TIMER_COUNTER_2);
//	avr32_tc_sr_t*  _sr = (avr32_tc_sr_t*) &_srint;
//
//	// get the RC status to see if this was the cause of the interrupt
//	uint _rcStatus = _sr->cpcs;
//	if (_rcStatus > 0)
//	{
//		// what is the time passed from the reset to now is saved in
//		// counter2TriggerStatus
//		counter2ElapsedUsecs += TC_getRC (TIMER_COUNTER_2)/TC_2_USEC_RESOLUTION;
//		TC_disableCounter (TIMER_COUNTER_2);
//	}
//	else
//	{
//		uint _covfs = _sr->covfs;
//		if (_covfs == 1)
//		{
//			counter2ElapsedUsecs += (65536/TC_2_USEC_RESOLUTION);
//		}
//	}
//}
static int singleIOPin = AVR32_PIN_PA20;
void TC2_interruptAction ()
{
	int _signalLevel = *TC2_singnalArray;
	++TC2_singnalArray;
	int _signalDuration = *TC2_singnalArray;
	if (_signalLevel == 0)
	{
		setGpioPinAsOutput (singleIOPin);
		gpioClearPortValue (singleIOPin);
	}
	else setGpioPinAsInput(singleIOPin);

	if (_signalDuration != -1)
	{
		TC_setRC(2, _signalDuration);
		++TC2_singnalArray;
		TC_TriggerCounter (2);
		TC_enableCounter(2);
	}
	else TC_disableCounter (2);
}

__isr__ static void TC2_interruptHandler(void)
{
	uint _srint = TC_readStatusRegister (TIMER_COUNTER_2);
	avr32_tc_sr_t*  _sr = (avr32_tc_sr_t*) &_srint;

	// get the RC status to see if this was the cause of the interrupt
	uint _rcStatus = _sr->cpcs;
	if (_rcStatus > 0)
	{
//		TC2_interruptAction();
		counter2ElapsedUsecs = 1;
	}
	else
	{
	}
}

// --------------------------------------------------------------
// this function resets the counter 2 for a duration in usec.
// 'TC_1_USEC_RESOLUTION' represents no of counter cycles
// needed to make up one usec.
// --------------------------------------------------------------
void resetUsecCounter (ushort valueForRC)
{
	counter2ElapsedUsecs = 0;
	if (valueForRC != 0) TC_setRC (USEC_TIMER, valueForRC);
	TC_enableCounter  (USEC_TIMER);
	TC_TriggerCounter (USEC_TIMER);
}

// ----------------------------------------------------------
// this allows usec wait with the condition that
// interval*TC_1_USEC_RESOLUTION <= 16 bit counter (65535)
// if you need more then you must do you own loop over this
// the counter register as well as RA, RB, RC are 16 bit
// registers.
// ----------------------------------------------------------
uint wait_usec (uint interval)
{
	uint _RCValue = interval*TC_2_USEC_RESOLUTION;
//	if ( (_RCValue&0xffff0000) == 0)
//	{
		resetUsecCounter ((ushort) _RCValue);
//		while (counter2ElapsedUsecs == 0)
//		{
			SLEEP (AVR32_PM_SMODE_FROZEN);
//		}
//	}
	return counter2ElapsedUsecs;
}

//uint wait_usec (uint interval)
//{
//	return (blockedUSecTimer(interval));
//}
// ----------------------------------------------------------
// ----------------------------------------------------------
// ----------------------------------------------------------
// ----------------------------------------------------------
// ----------------------------------------------------------
//
// ----------------------------------------------------------
void TC_setInterruptHandler (uint channel)
{
	if (channel == TIMER_COUNTER_0)
	{
		INTC_register_interrupt(&TC0_interruptHandler, AVR32_TC_IRQ0, TIMER_COUNTER_INT_LEVEL);
	}
	else if (channel == TIMER_COUNTER_1)
	{
		INTC_register_interrupt(&TC1_interruptHandler, AVR32_TC_IRQ1, TIMER_COUNTER_INT_LEVEL);
	}
	else if (channel == TIMER_COUNTER_2)
	{
		INTC_register_interrupt(&TC2_interruptHandler, AVR32_TC_IRQ2, TIMER_COUNTER_INT_LEVEL);
	}
}


//int tc_init_waveform (
//		volatile avr32_tc_t *tc,
//		const tc_waveform_opt_t *opt)
//{
//	// Check for valid input.
//	if (opt->channel >= TC_NUMBER_OF_CHANNELS)
//	return TC_INVALID_ARGUMENT;
//
//	// GENERATE SIGNALS: Waveform operating mode.
//	tc->channel[opt->channel].cmr = opt->bswtrg << AVR32_TC_BSWTRG_OFFSET |
//								 opt->beevt << AVR32_TC_BEEVT_OFFSET |
//								 opt->bcpc << AVR32_TC_BCPC_OFFSET |
//								 opt->bcpb << AVR32_TC_BCPB_OFFSET |
//								 opt->aswtrg << AVR32_TC_ASWTRG_OFFSET |
//								 opt->aeevt << AVR32_TC_AEEVT_OFFSET |
//								 opt->acpc << AVR32_TC_ACPC_OFFSET |
//								 opt->acpa << AVR32_TC_ACPA_OFFSET |
//										 1 << AVR32_TC_WAVE_OFFSET |
//								 opt->wavsel << AVR32_TC_WAVSEL_OFFSET |
//								 opt->enetrg << AVR32_TC_ENETRG_OFFSET |
//								 opt->eevt << AVR32_TC_EEVT_OFFSET |
//								 opt->eevtedg << AVR32_TC_EEVTEDG_OFFSET |
//								 opt->cpcdis << AVR32_TC_CPCDIS_OFFSET |
//								 opt->cpcstop << AVR32_TC_CPCSTOP_OFFSET |
//								 opt->burst << AVR32_TC_BURST_OFFSET |
//								 opt->clki << AVR32_TC_CLKI_OFFSET |
//								 opt->tcclks << AVR32_TC_TCCLKS_OFFSET;
//
// return 0;
//}

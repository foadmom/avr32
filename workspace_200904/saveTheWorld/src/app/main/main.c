/** *********************************************************************
 *
 * The main entry point for AVR32 cpu
 *
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 *
 *
 *****************************************************************************/

#include <avr32/io.h>

#include <stdio.h>
#include <string.h>

#include "common.h"
#include "cgpio.h"
#include "cintc.h"
#include "ctc.h"
#include "cpm.h"
#include "cusart.h"
#include "crtc.h"
#include "cadc.h"
#include "command.h"
#include "control.h"
#include "temperature.h"


// ----------------------------------------------------------
// this is just a quick test of the LEDs connected to GPIO ports
// ----------------------------------------------------------
void testLEDInit ()
{
    setGpioPinAsOutput (AVR32_PIN_PA07);
    setGpioPinAsOutput (AVR32_PIN_PA08);
    setGpioPinAsOutput (AVR32_PIN_PA21);
    setGpioPinAsOutput (AVR32_PIN_PA22);
    setGpioPortValue   (AVR32_PIN_PA07);
    clearGpioPortValue (AVR32_PIN_PA08);
    setGpioPortValue   (AVR32_PIN_PA21);
    clearGpioPortValue (AVR32_PIN_PA22);

    CGPIO_registerInterrupt ();
    gpioEnableInterrupt (AVR32_PIN_PB02);
    gpioEnableInterrupt (AVR32_PIN_PB03);
    CGPIO_setGlitchFilter (AVR32_PIN_PB02);
    CGPIO_setGlitchFilter (AVR32_PIN_PB03);
}

// ----------------------------------------------------------
// this is just a quick test of the LEDs connected to GPIO ports
// ----------------------------------------------------------
void testLEDs ()
{
	gpioTogglePortValue (AVR32_PIN_PA07);
	gpioTogglePortValue (AVR32_PIN_PA08);
}

#define WAVE_OUTPUT_TIMER_CHANNEL		TIMER_COUNTER_0
#define CAPTURE_INPUT_TIMER_CHANNEL		TIMER_COUNTER_2

void testCounters ()
{
	PM_enableTCClock ();
	// setup channel 0
//    TC_setClockSource (0, AVR32_TC_CMR0_TCCLKS_TIMER_DIV4_CLOCK);
//    TC_setModeCapture (0);
//    TC_setRC (0, TC_10_MSEC_RESOLUTION);
//    TC_enableRCCompareTrigger (0);
//	TC_setInterruptHandler (0);

	// setup channel 1
    TC_setClockSource (1, AVR32_TC_CMR1_TCCLKS_TIMER_DIV2_CLOCK);
    TC_setModeCapture (1);
    TC_setRC (1, TC_1_USEC_RESOLUTION*1000);
    TC_enableRCCompareTrigger (1);
	TC_setInterruptHandler (1);


	// setup channel 0 waveform
	setPinFunction (AVR32_TC_A0_0_0_PIN, AVR32_TC_A0_0_0_FUNCTION);
    TC_setClockSource (WAVE_OUTPUT_TIMER_CHANNEL, AVR32_TC_CMR0_TCCLKS_TIMER_DIV5_CLOCK);
    TC_setModeWaveform(WAVE_OUTPUT_TIMER_CHANNEL);
    TC_setEventEdge (WAVE_OUTPUT_TIMER_CHANNEL, AVR32_TC_POS_EDGE);
    TC_setRA (WAVE_OUTPUT_TIMER_CHANNEL, 3200);
    TC_setWaveformRATriggerEffect (WAVE_OUTPUT_TIMER_CHANNEL, AVR32_TC_ACPA_SET);
    TC_setRC (WAVE_OUTPUT_TIMER_CHANNEL, 64000);
    TC_setWaveformRCTriggerEffect (WAVE_OUTPUT_TIMER_CHANNEL, AVR32_TC_ACPA_CLEAR);
//    TC_setRB (WAVE_OUTPUT_TIMER_CHANNEL, 50000);
	TC_enableCounter (WAVE_OUTPUT_TIMER_CHANNEL);
	TC_setInterruptHandler (WAVE_OUTPUT_TIMER_CHANNEL);
//	TC_disableInterrupts (WAVE_OUTPUT_TIMER_CHANNEL, 0xff);
	TC_enableInterrupts (WAVE_OUTPUT_TIMER_CHANNEL, 0xff);
//	TC_TriggerCounter (WAVE_OUTPUT_TIMER_CHANNEL);

	setPinFunction (AVR32_TC_A2_0_1_PIN, AVR32_TC_A2_0_1_FUNCTION);
    TC_setClockSource (CAPTURE_INPUT_TIMER_CHANNEL, AVR32_TC_CMR1_TCCLKS_TIMER_DIV2_CLOCK);
    TC_setModeCapture (CAPTURE_INPUT_TIMER_CHANNEL);
    TC_setExtTriggerToTIOA (CAPTURE_INPUT_TIMER_CHANNEL);
    TC_setEventEdge (CAPTURE_INPUT_TIMER_CHANNEL, AVR32_TC_POS_EDGE);
    TC_setRALoadingSelection (CAPTURE_INPUT_TIMER_CHANNEL, AVR32_TC_BIOTH_EDGES_TIOA);
//    TC_enableRCCompareTrigger (CAPTURE_INPUT_TIMER_CHANNEL);
//	TC_setInterruptHandler (CAPTURE_INPUT_TIMER_CHANNEL);
//	TC_enableRALoadingInterrupt(CAPTURE_INPUT_TIMER_CHANNEL);
//	TC_enableInterrupts (CAPTURE_INPUT_TIMER_CHANNEL, 0x04);
//	TC_TriggerCounter (CAPTURE_INPUT_TIMER_CHANNEL);
	TC_enableCounter (CAPTURE_INPUT_TIMER_CHANNEL);
	TC_setSynch ();
}

void setAllPorts (int value)
{
	if (value & 0x1) setGpioPortValue (AVR32_PIN_PA07);
	else clearGpioPortValue (AVR32_PIN_PA07);

	if (value & 0x2) setGpioPortValue (AVR32_PIN_PA08);
	else clearGpioPortValue (AVR32_PIN_PA08);

	if (value & 0x4) setGpioPortValue (AVR32_PIN_PA21);
	else clearGpioPortValue (AVR32_PIN_PA21);

	if (value & 0x8) setGpioPortValue (AVR32_PIN_PA22);
	else clearGpioPortValue (AVR32_PIN_PA22);

}

void testUSART ()
{
	setPinFunction (AVR32_USART1_RXD_0_0_PIN, AVR32_USART1_RXD_0_0_FUNCTION);
	setPinFunction (AVR32_USART1_TXD_0_0_PIN, AVR32_USART1_TXD_0_0_FUNCTION);

	USART1_initialise ();
	USART1_configurePort (57600, 8, AVR32_USART_MR_PAR_NONE, AVR32_USART_MR_MODE_NORMAL, AVR32_USART_MR_NBSTOP_1);

	USART1_EnableRx ();
	USART1_EnableTx ();

}

void testRTC ()
{
	PM_selectOsc32Crystal ();
	RTC_isBusy ();
	PM_enableClk32 (0);
	RTC_isBusy ();
	RTC_setPrescale ();
	RTC_set32KOscAsClockSource ();
	RTC_setCounterValue (0);
	RTC_setTopValue (0);
	RTC_setInterruptHandler ();
	RTC_enableInterrupt ();
	RTC_enable ();
}

void setupClocks ()
{
	PM_setOsc0Mode (AVR32_PM_OSCCTRL0_MODE_CRYSTAL_G3);
	PM_setOsc0Startup (3);
	PM_enableOsc0 ();
	PM_waitForClk0Ready ();
	PM_setOsc0AsMainClockSource ();
}

void testADC ()
{
	ADC_BufferInit ();
	setPinFunction (EXAMPLE_ADC_LIGHT_PIN, AVR32_ADC_AD_6_FUNCTION);
	setPinFunction (AVR32_ADC_AD_7_PIN, AVR32_ADC_AD_7_FUNCTION);

	ADC_setLowResMode ();
	ADC_setSampleAndHoldTime (0xf);
	ADC_setStartUpTime (0x1f);
	ADC_setInterruptHandler ();
	ADC_enable(6);
	ADC_enable(7);
	ADC_enableDataReadyInterrupt ();
}


int main(void)
{
	setupClocks ();
	Disable_global_interrupt ();
	INTC_init_interrupts ();
	testUSART ();
	debug ("AVR32Studio 2 is now on\n");
	debug ("USART1 Initialised\n");
//    testLEDInit ();
    testCounters ();
	debug ("Timer/Counters Initialised\n");
//    testRTC ();
	debug ("RTC Initialised\n");
//	CGPIO_initialisejoystick ();
	debug ("GPIO Initialised\n");
	Enable_global_interrupt();
	debug ("interrupts enabled\n");

//	testADC ();
	debug ("ADC test completed\n");

	mainLoop ();
	debug ("returning from the main loop\n");
//	while (1)
//	{
//		testLEDs ();
//		ADC_start ();
//		wait (1000);
//		if (ADC_conversionComplete () )
//		{
//			ADCBuffer* buffer = ADC_getBuffer ();
//			uint temperature = getTemperature (buffer[7]);
//			debugf ("ADC light channel=%d  temperature=%dc \n", buffer[6]), temperature);
//		}
//
//		if (USART1_isDataReady ())
//		{
//			USART1_ReadLine((char *) st);
//			processMessage ((char *) st);
//		}
//		timeToString ((char*)st);
//		debug (st);
//		if (joyStickActivated ())
//		{
//			JoyStick _joyStick = getJoyStickFlags();
//			debugf ("joyStick   north=%u south=%u east=%u west=%u\n", _joyStick.north, _joyStick.south, _joyStick.east, _joyStick.west);
//		}
//	};

    return 0;
}


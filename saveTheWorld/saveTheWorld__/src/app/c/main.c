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
#include "singleWire.h"

void TurnOnAllLEDs()
{
	gpioClearPortValue(AVR32_PIN_PA07);
	gpioClearPortValue(AVR32_PIN_PA08);
	gpioClearPortValue(AVR32_PIN_PA21);
	gpioClearPortValue(AVR32_PIN_PA22);
}

void TurnOffAllLEDs()
{
	gpioSetPortValue(AVR32_PIN_PA07);
	gpioSetPortValue(AVR32_PIN_PA08);
	gpioSetPortValue(AVR32_PIN_PA21);
	gpioSetPortValue(AVR32_PIN_PA22);
}

void TurnOnHalfLEDs()
{
	gpioClearPortValue(AVR32_PIN_PA07);
	gpioClearPortValue(AVR32_PIN_PA08);
	gpioSetPortValue(AVR32_PIN_PA21);
	gpioSetPortValue(AVR32_PIN_PA22);
}

void switchThisLED(int pin, int onOff)
{
	if (onOff == 0)
	{
		gpioClearPortValue(pin);
	}
	else
		gpioSetPortValue(pin);
}
void switchLEDs(int pa07, int pa08, int pa21, int pa22)
{
	switchThisLED(AVR32_PIN_PA07, pa07);
	switchThisLED(AVR32_PIN_PA08, pa08);
	switchThisLED(AVR32_PIN_PA21, pa21);
	switchThisLED(AVR32_PIN_PA22, pa22);
}
// ----------------------------------------------------------
// this is just a quick test of the LEDs connected to GPIO ports
// ----------------------------------------------------------
void testLEDInit()
{
	setGpioPinAsOutput(AVR32_PIN_PA07);
	setGpioPinAsOutput(AVR32_PIN_PA08);
	setGpioPinAsOutput(AVR32_PIN_PA21);
	setGpioPinAsOutput(AVR32_PIN_PA22);
	gpioClearPortValue(AVR32_PIN_PA07);
	gpioClearPortValue(AVR32_PIN_PA08);
	gpioClearPortValue(AVR32_PIN_PA21);
	gpioClearPortValue(AVR32_PIN_PA22);

	CGPIO_registerInterrupt();

	gpioSetInterruptModeAnyFallingEdge(AVR32_PIN_PB02);
	gpioEnableInterrupt(AVR32_PIN_PB02);
	gpioSetInterruptModeAnyFallingEdge(AVR32_PIN_PB03);
	gpioEnableInterrupt(AVR32_PIN_PB03);
	gpioSetGlitchFilter(AVR32_PIN_PB02);
	gpioSetGlitchFilter(AVR32_PIN_PB03);
}

// ----------------------------------------------------------
// this is just a quick test of the LEDs connected to GPIO ports
// ----------------------------------------------------------
void testLEDs()
{
	//	gpioTogglePortValue (AVR32_PIN_PA07);
	gpioTogglePortValue(AVR32_PIN_PA08);
}

#define WAVE_OUTPUT_TIMER_CHANNEL		TIMER_COUNTER_0
#define CAPTURE_INPUT_TIMER_CHANNEL		TIMER_COUNTER_2

static void init_tc_output(volatile avr32_tc_t * tc, unsigned int channel)
{
	// Options for waveform generation.
	tc_waveform_opt_t waveform_opt =
	{ .channel = channel, // Channel selection.

			.bswtrg = TC_EVT_EFFECT_NOOP, // Software trigger effect on TIOB.
			.beevt = TC_EVT_EFFECT_NOOP, // External event effect on TIOB.
			.bcpc = TC_EVT_EFFECT_NOOP, // RC compare effect on TIOB.
			.bcpb = TC_EVT_EFFECT_NOOP, // RB compare effect on TIOB.

			.aswtrg = TC_EVT_EFFECT_NOOP, // Software trigger effect on TIOA.
			.aeevt = TC_EVT_EFFECT_NOOP, // External event effect on TIOA.
			.acpa = TC_EVT_EFFECT_CLEAR, // RA compare effect on TIOA.
			.acpc = TC_EVT_EFFECT_SET, // RC compare effect on TIOA.
			//		.acpc = TC_EVT_EFFECT_NOOP, // RC compare effect on TIOA.
			//		.acpa = TC_EVT_EFFECT_TOGGLE, // RA compare effect on TIOA.

			.wavsel = AVR32_TC_CMR0_WAVSEL_UPDOWN_AUTO, // Waveform selection:
			.enetrg = 0, // External event trigger enable.
			.eevt = TC_EXT_EVENT_SEL_TIOB_INPUT, // External event selection.
			.eevtedg = TC_SEL_NO_EDGE, // External event edge selection.
			.cpcdis = 0, // Counter disable when RC compare.
			.cpcstop = 0, // Counter clock stopped with RC compare.

			.burst = TC_BURST_NOT_GATED, // Burst signal selection.
			.clki = TC_CLOCK_RISING_EDGE, // Clock inversion.
			//		.tcclks = TC_CLOCK_SOURCE_TC3 // Internal source clock 3, connected to fPBA / 8.
			};

	// Initialize the timer/counter waveform.
	tc_init_waveform(tc, &waveform_opt);
}

static int signal [] = {uSec(100), 480*TC_2_USEC_RESOLUTION, 1, -1, -1, -1};
static int clearSignal [] = {uSec(100), 480*TC_2_USEC_RESOLUTION, 1, -1, -1, -1};
static int SetSignal   [] = {1, 10000, 0, 480*TC_2_USEC_RESOLUTION, 1, -1, -1, -1};
void setupCounter0()
{
	// disable all the counter interrupts for this counter channel
	// so we are starting from a known point
	//	TC_disableInterrupts(WAVE_OUTPUT_TIMER_CHANNEL, 0xff);


	// setup channel 0 waveform
	setPinFunction(AVR32_TC_A0_0_0_PIN, AVR32_TC_A0_0_0_FUNCTION);

	volatile avr32_tc_t *tc = &AVR32_TC;
	tc->channel[WAVE_OUTPUT_TIMER_CHANNEL].CMR.capture.wave = 1;

	init_tc_output(tc, WAVE_OUTPUT_TIMER_CHANNEL);

	setTC0Array (signal);

	TC_setRA(WAVE_OUTPUT_TIMER_CHANNEL, 20);
	TC_setRC(WAVE_OUTPUT_TIMER_CHANNEL, 60);

	TC_setClockSource(WAVE_OUTPUT_TIMER_CHANNEL,
			AVR32_TC_CMR0_TCCLKS_TIMER_DIV2_CLOCK);

	//	TC_setModeWaveform(WAVE_OUTPUT_TIMER_CHANNEL);
	//	TC_setWaveFormSelect(WAVE_OUTPUT_TIMER_CHANNEL,
	//			AVR32_TC_CMR0_WAVSEL_UPDOWN_AUTO);
	//	TC_setEventEdge(WAVE_OUTPUT_TIMER_CHANNEL, AVR32_TC_POS_EDGE);
	//
	//	// set RA register to produce interrupt when counter value and RA match.
	//	TC_setRA(WAVE_OUTPUT_TIMER_CHANNEL, 80);
	//	TC_enableCompareRAInterrupt(WAVE_OUTPUT_TIMER_CHANNEL);
	//
	//	// set RB register to produce interrupt when counter value and RB match.
	//	// set RC register to produce interrupt when counter value and RC match.
	//	TC_setRC(WAVE_OUTPUT_TIMER_CHANNEL, 160);
//	TC_enableCompareRCInterrupt(WAVE_OUTPUT_TIMER_CHANNEL);
//	TC_enableCompareRAInterrupt(WAVE_OUTPUT_TIMER_CHANNEL);

//	TC_setInterruptHandler(WAVE_OUTPUT_TIMER_CHANNEL);
	TC_enableCounter(WAVE_OUTPUT_TIMER_CHANNEL);
}

void setupCounter1()
{
	// setup channel 1
	TC_setClockSource(1, TC_1_TIMER_DIV_CLOCK);
	TC_setModeCapture(1);
	TC_setRC(1, TC_1_MSEC_RESOLUTION);

	// rc compare triggers the counter to start from 0 again
	TC_enableRCCompareTrigger(1);
	TC_setInterruptHandler(1);
}

void setupCounter2()
{
	TC_disableInterrupts(CAPTURE_INPUT_TIMER_CHANNEL, 0xff);
	TC_setClockSource(CAPTURE_INPUT_TIMER_CHANNEL, TC_2_TIMER_DIV_CLOCK);

	TC_setModeCapture(CAPTURE_INPUT_TIMER_CHANNEL);
	TC_setInterruptHandler(CAPTURE_INPUT_TIMER_CHANNEL);
	TC_enableCompareRCInterrupt(CAPTURE_INPUT_TIMER_CHANNEL);
	TC_enableOverflowInterrupt(CAPTURE_INPUT_TIMER_CHANNEL);
}

void testCounters()
{
	PM_enableTCClock();

	setupCounter0();
	setupCounter1();
	setupCounter2();

	TC_setSynch();
}

void setAllPorts(int value)
{
	//	if (value & 0x1) gpioSetPortValue (AVR32_PIN_PA07);
	//	else gpioClearPortValue (AVR32_PIN_PA07);

	if (value & 0x2)
		gpioSetPortValue(AVR32_PIN_PA08);
	else
		gpioClearPortValue(AVR32_PIN_PA08);

	if (value & 0x4)
		gpioSetPortValue(AVR32_PIN_PA21);
	else
		gpioClearPortValue(AVR32_PIN_PA21);

	if (value & 0x8)
		gpioSetPortValue(AVR32_PIN_PA22);
	else
		gpioClearPortValue(AVR32_PIN_PA22);

}

void testUSART()
{
	setPinFunction(AVR32_USART1_RXD_0_0_PIN, AVR32_USART1_RXD_0_0_FUNCTION);
	setPinFunction(AVR32_USART1_TXD_0_0_PIN, AVR32_USART1_TXD_0_0_FUNCTION);

	USART1_initialise();
	USART1_configurePort(57600, 8, AVR32_USART_MR_PAR_NONE,
			AVR32_USART_MR_MODE_NORMAL, AVR32_USART_MR_NBSTOP_1);

	USART1_EnableRx();
	USART1_EnableTx();

}

void testRTC()
{
	PM_selectOsc32Crystal();
	RTC_isBusy();
	PM_enableClk32(0);
	RTC_isBusy();
	RTC_setPrescale();
	RTC_set32KOscAsClockSource();
	RTC_setCounterValue(0);
	RTC_setTopValue(0);
	RTC_setInterruptHandler();
	RTC_enableInterrupt();
	RTC_enable();
}

void setupClocks()
{
	PM_setOsc0Mode(AVR32_PM_OSCCTRL0_MODE_CRYSTAL_G3);
	PM_setOsc0Startup(3);
	PM_enableOsc0();
	PM_waitForClk0Ready();
	PM_setOsc0AsMainClockSource();
}

void testADC()
{
	ADC_BufferInit();
	setPinFunction(EXAMPLE_ADC_LIGHT_PIN, AVR32_ADC_AD_6_FUNCTION);
	setPinFunction(AVR32_ADC_AD_7_PIN, AVR32_ADC_AD_7_FUNCTION);

	ADC_setLowResMode();
	ADC_setSampleAndHoldTime(0xf);
	ADC_setStartUpTime(0x1f);
	ADC_setInterruptHandler();
	ADC_enable(6);
	ADC_enable(7);
	ADC_enableDataReadyInterrupt();
}

void oneSecond()
{
	int _mSecCounter = 1000;
	for (; _mSecCounter > 0; _mSecCounter--)
	{
		blockedUSecTimer(1000);
	}
}

void testOneUSecTiming()
{
	char _timeSt[32];
	timeToString((char*) _timeSt);
	debugf ("testOneUSecTiming    %s%s", _timeSt, NEWLINE);
	int _interval = 100; // seconds
	for (; _interval > 0; _interval--)
	{
		oneSecond();
	}
	timeToString((char*) _timeSt);
	debugf ("testOneUSecTiming    %s%s", _timeSt, NEWLINE);
}

void testMSecTiming()
{
	char _timeSt[32];
	timeToString((char*) _timeSt);
	debugf ("testOneUSecTiming    %s%s", _timeSt, NEWLINE);
	blockedMSecTimer(100000);
	timeToString((char*) _timeSt);
	debugf ("testOneUSecTiming    %s%s", _timeSt, NEWLINE);
}

void testSecTiming()
{
	char _timeSt[32];
	timeToString((char*) _timeSt);
	debugf ("testOneUSecTiming    %s%s", _timeSt, NEWLINE);
	blockedSecTimer(100);
	timeToString((char*) _timeSt);
	debugf ("testOneUSecTiming    %s%s", _timeSt, NEWLINE);
}

void testGPIOInterrupts()
{
	setGpioPinAsInput(AVR32_PIN_PB02);
	gpioSetInterruptModeAnyFallingEdge(AVR32_PIN_PB02);
	gpioEnableInterrupt(AVR32_PIN_PB02);
}

void pulseTest(uint pin, int interval)
{
	setGpioPinAsInput(pin);
	wait_usec(interval);
	setGpioPinAsOutput(pin);
	gpioClearPortValue(pin);
	wait_usec(interval);
}

#define testPulseWidth 20
void testSingleWirePort(uint pin)
{
	do
	{
		pulseTest(pin, testPulseWidth);
	} while (1);
}

void testSingleWire(uint pin)
{
	setTC2Array (signal);
	setSingleIOPin(pin);
	TC_setRC(2, 10000);
	TC_enableCounter  (USEC_TIMER);
	TC_TriggerCounter (USEC_TIMER);
//	gpioSetInterruptModeAnyFallingEdge(pin);
//	gpioSetGlitchFilter(pin);
//	writeDataToScratchPad(0x13, 0x25);

}

#define SingleWireIOPin		AVR32_PIN_PA20
void testIOPulse ()
{
	int _loopCounter = 10000;
	setGpioPinAsInput(SingleWireIOPin);
	blockedUSecTimer (2000);
	setGpioPinAsOutput(SingleWireIOPin);
	for (_loopCounter = 10000; _loopCounter != 0; _loopCounter--)
	{
		gpioClearPortValue (SingleWireIOPin);
		blockedUSecTimer (500);
		setGpioPinAsInput(SingleWireIOPin);
		blockedUSecTimer (2000);
		blockedUSecTimer (2000);
	}
}

void testTimedIOPulse ()
{
	int _loopCounter = 10000;
	for (_loopCounter = 10000; _loopCounter != 0; _loopCounter--)
	{
		setGpioPinAsInput(SingleWireIOPin);
		wait_usec(100);
		setGpioPinAsOutput(SingleWireIOPin);
		gpioClearPortValue (SingleWireIOPin);
		wait_usec(100);
	}
}

int main(void)
{
	setupClocks();
	Disable_global_interrupt ();
	INTC_init_interrupts();

	testUSART();
	debugln ("AVR32Studio 2 is now on");
	testLEDInit();
	testCounters();
	debugln ("Timer/Counters Initialised");
	testRTC();
	debugln ("RTC Initialised");
	CGPIO_initialisejoystick();
	debugln ("GPIO Initialised"); // setup channel 0 waveform

	Enable_global_interrupt();
	debugln ("interrupts enabled");

	//	testOneUSecTiming ();
	//	testMSecTiming  ();
	//	testSecTiming ();

	testADC();
	debugln ("ADC test completed");
	testIOPulse ();
//	testTimedIOPulse ();

	testSingleWire (SingleWireIOPin);
	//	testSingleWire (SingleWireIOPin);
	getTemperatureNoRom();

	mainLoop();
	debugln ("returning from the main loop");

	return 0;
}


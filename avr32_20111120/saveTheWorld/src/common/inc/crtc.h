#ifndef CRTC_H_
#define CRTC_H_

#include <avr32/io.h>
#include "cintc.h"
#include "cusart.h"
#include "common.h"


#define RTC_PSEL_32KHZ_1HZ    14
#define RTC_PSEL_RC_1HZ       16


void RTC_setInterruptHandler ();

#define __rtc 	(&AVR32_RTC)

__always_inline  int RTC_isBusy ()
{
    return __rtc->CTRL.busy;
}

// -----------------------------------------------------------
// set the prescale select bit 
// -----------------------------------------------------------
__always_inline  void RTC_setPrescale ()
{
	while (RTC_isBusy ());
	__rtc->CTRL.psel = RTC_PSEL_32KHZ_1HZ;
}


// -----------------------------------------------------------
// set 32KHz oscillator as the clock source
// -----------------------------------------------------------
__always_inline void RTC_set32KOscAsClockSource ()
{
	while (RTC_isBusy ());
	__rtc->CTRL.clk32 = 1;
}


// -----------------------------------------------------------
// read and write the RTC counter value
// -----------------------------------------------------------
__always_inline uint RTC_getCounterValue ()
{
	while (RTC_isBusy ());
	return (__rtc->val);
}

__always_inline void RTC_setCounterValue (unsigned int value)
{
	while (RTC_isBusy ());
	__rtc->val = value;
}

// -----------------------------------------------------------
// read and write the TOP register
// -----------------------------------------------------------
__always_inline uint RTC_getTopValue ()
{
	while (RTC_isBusy ());
	return (__rtc->top);
}

__always_inline void RTC_setTopValue (unsigned int value)
{
	while (RTC_isBusy ());
	__rtc->top = value;
}


// -----------------------------------------------------------
// read and write the TOP register
// -----------------------------------------------------------
__always_inline void RTC_enable ()
{
	while (RTC_isBusy ());
	__rtc->CTRL.en = 1;
}


// -----------------------------------------------------------
// enable rtc interrupt
// -----------------------------------------------------------
__always_inline void RTC_enableInterrupt ()
{
	__rtc->IER.topi = 1;
}

__always_inline void RTC_disableInterrupt ()
{
	__rtc->IDR.topi = 1;
}

__always_inline void RTC_clearInterrupt ()
{
	__rtc->ICR.topi = 1;
}






struct TimeStamp
{
    unsigned int     seconds;
    unsigned int     minutes;
    unsigned int     hours;
    unsigned int     days;
    unsigned int     months;
    unsigned int     years;
};


unsigned int currentSecond();
void setDate (unsigned int year_, unsigned int month_, unsigned int day_);
void setTime (unsigned int hour_, unsigned int min_, unsigned int sec_);
unsigned int leapYear (unsigned int year);
void incSeconds ();
void incMinutes ();
void incHours ();
void incDays ();
void incMonths ();
void incYears ();
void timeToString (char* str);



#endif /*CRTC_H_*/

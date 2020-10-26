
#include <stdio.h>

#include "crtc.h"
#include "cgpio.h"

__isr__ static void RTC_int_handler(void)
{
	incSeconds ();
	RTC_clearInterrupt ();
}


void RTC_setInterruptHandler ()
{
	INTC_register_interrupt(&RTC_int_handler, AVR32_RTC_IRQ, RTC_INT_LEVEL);
}






volatile static struct TimeStamp timeStamp;

int daysInMonth[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};

unsigned int currentSecond ()
{
    return (timeStamp.seconds);
}

// ===================================================
// year is 2 digits and starts from 2001
// 
// ===================================================
void setDate (unsigned int year_, unsigned int month_, unsigned int day_)
{
    timeStamp.years = year_;
    timeStamp.months = month_;
    timeStamp.days = day_;
}


// ===================================================
// 
// 
// ===================================================
void setTime (unsigned int hour_, unsigned int min_, unsigned int sec_)
{
    timeStamp.hours = hour_;
    timeStamp.minutes = min_;
    timeStamp.seconds = sec_;
}


// ===================================================
// 
// 
// ===================================================
unsigned int leapYear (unsigned int year)
{
    if ((year % 4) == 0) return 1;
    else return 0;
}


// ===================================================
// 
// 
// ===================================================
void incSeconds ()
{
    if (timeStamp.seconds < 59) ++timeStamp.seconds;
    else
    {
        timeStamp.seconds = 0;
        incMinutes ();
    }
}

// ===================================================
// 
// 
// ===================================================
void incMinutes ()
{
    if (timeStamp.minutes < 59) ++timeStamp.minutes;
    else
    {
        timeStamp.minutes = 0;
        incHours ();
    }
}

// ===================================================
// 
// 
// ===================================================
void incHours ()
{
    if (timeStamp.hours < 23) ++timeStamp.hours;
    else
    {
        timeStamp.hours = 0;
        incDays ();
    }
}

// ===================================================
// 
// 
// ===================================================
void incDays ()
{
    unsigned int leapYeaarAdjust = leapYear(timeStamp.years) * ((timeStamp.months==2) ? 1 : 0);
    if (timeStamp.days < (daysInMonth[timeStamp.months]+leapYeaarAdjust)) ++timeStamp.days;
    else
    {
        timeStamp.days = 1;
        incMonths ();
    }
}

// ===================================================
// 
// 
// ===================================================
void incMonths ()
{
    if (timeStamp.months < 12) ++timeStamp.months;
    else
    {
        timeStamp.months = 1;
        incYears ();
    }
}

// ===================================================
// 
// 
// ===================================================
void incYears ()
{
    ++timeStamp.years;
}


// ===================================================
// 
// 
// ===================================================
void timeToString (char* str)
{
    sprintf ((char*)str, "20%02d-%02d-%02d  %02d:%02d:%02d", 
                    timeStamp.years,
                    timeStamp.months,
                    timeStamp.days,
                    timeStamp.hours,
                    timeStamp.minutes,
                    timeStamp.seconds
            );
}

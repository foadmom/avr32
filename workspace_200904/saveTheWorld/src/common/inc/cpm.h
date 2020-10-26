#ifndef CPM_H_
#define CPM_H_

#include <avr32/io.h>

#define PM_TC_CLOCK_MASK	0x00001000

void PM_enableGenericClock (int clockNumber);
void PM_disableGenericClock (int clockNumber);

void PM_setOsc0AsMainClockSource ();
void PM_enableOsc0 ();
void PM_setOsc0Mode (int mode);
void PM_setOsc0Startup (int _startup);
void PM_waitForClk0Ready ();

void PM_setOscAsGenericClockSource (int clockNumber);
void PM_setPllAsGenericClockSource (int clockNumber);

void PM_setOsc0AsGenericClockSource (int clockNumber);
void PM_setOsc1AsGenericClockSource (int clockNumber);

void PM_enableTCClock ();
void PM_disableTCClock ();

void PM_selectOsc32Crystal ();
void PM_enableClk32 (int startup);
void PM_waitForClk32Ready ();

void PM_selectPBA (unsigned int _pbasel);
void PM_deselectPBA ();


// ----------------------------------------------
// AVR32_PM_SMODE_IDLE
// AVR32_PM_SMODE_FROZEN
// AVR32_PM_SMODE_STANDBY
// AVR32_PM_SMODE_STOP
// AVR32_PM_SMODE_SHUTDOWN
// AVR32_PM_SMODE_STATIC
// ----------------------------------------------
#define SLEEP(mode)   {__asm__ __volatile__ ("sleep "STRINGZ(mode));}



#endif /*CPM_H_*/

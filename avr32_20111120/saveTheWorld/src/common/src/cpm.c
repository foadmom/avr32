
#include "cpm.h"

static volatile avr32_pm_t *pm = &AVR32_PM;



// ----------------------------------------------------------
// enable the generic clock
// ----------------------------------------------------------
void PM_enableGenericClock (int clockNumber)
{
	pm->GCCTRL[clockNumber].cen = 1;
}

// ----------------------------------------------------------
// disable the generic clock
// ----------------------------------------------------------
void PM_disableGenericClock (int clockNumber)
{
	pm->GCCTRL[clockNumber].cen = 0;
}



// ----------------------------------------------------------
// ----------------------------------------------------------
// set Oscillator 0 As Main Clock Source
// ----------------------------------------------------------
void PM_setOsc0AsMainClockSource ()
{
	pm->MCCTRL.mcsel = AVR32_PM_MCCTRL_MCSEL_OSC0;
}

// ----------------------------------------------------------
// set Oscillator 0 As Main Clock Source
// ----------------------------------------------------------
void PM_enableOsc0 ()
{
	pm->MCCTRL.osc0en = 1;
}




// ----------------------------------------------------------
// ----------------------------------------------------------
// generic clock source is oscillator
// ----------------------------------------------------------
void PM_setOscAsGenericClockSource (int clockNumber)
{
	pm->GCCTRL[clockNumber].pllsel = 0;
}

// ----------------------------------------------------------
// generic clock source is PLL
// ----------------------------------------------------------
void PM_setPllAsGenericClockSource (int clockNumber)
{
	pm->GCCTRL[clockNumber].pllsel = 1;
}



// ----------------------------------------------------------
// generic clock source is osc or pll 0
// ----------------------------------------------------------
void PM_setOsc0AsGenericClockSource (int clockNumber)
{
	pm->GCCTRL[clockNumber].oscsel = 0;
}

// ----------------------------------------------------------
// generic clock source is osc or pll 1
// ----------------------------------------------------------
void PM_setOsc1AsGenericClockSource (int clockNumber)
{
	pm->GCCTRL[clockNumber].oscsel = 1;
}

// ----------------------------------------------------------
// set Oscillator 0 mode to External
// ----------------------------------------------------------
void PM_setOsc0Mode (int mode)
{
	pm->OSCCTRL0.mode = mode;
}

// ----------------------------------------------------------
// set Oscillator 0 startup delay value
// ----------------------------------------------------------
void PM_setOsc0Startup (int _startup)
{
	pm->OSCCTRL0.startup = _startup;
}

// ----------------------------------------------------------
// wait for the clock to get ready
// ----------------------------------------------------------
void PM_waitForClk0Ready ()
{
	while (!(pm->poscsr & AVR32_PM_POSCSR_OSC0RDY_MASK));
}


// ----------------------------------------------------------
// select the 32KHz crystal
// ----------------------------------------------------------
void PM_selectOsc32Crystal ()
{
	pm->OSCCTRL32.mode = AVR32_PM_OSCCTRL32_MODE_CRYSTAL;
}


// ----------------------------------------------------------
// enable the 32KHz oscillator 
// ----------------------------------------------------------
void PM_enableClk32 (int startup)
{
  pm->OSCCTRL32.osc32en = 1;
  pm->OSCCTRL32.startup = startup;
}

// ----------------------------------------------------------
// it takes a while for the clock to be ready so wait for it 
// ----------------------------------------------------------
void PM_waitForClk32Ready ()
{
	while (!(pm->poscsr & AVR32_PM_POSCSR_OSC32RDY_MASK));
}



// ----------------------------------------------------------
// ----------------------------------------------------------
// ----------------------------------------------------------
// ----------------------------------------------------------
// Clock selections
// ----------------------------------------------------------
void PM_selectPBA (unsigned int _pbasel)
{
	pm->CKSEL.pbasel = _pbasel & 0x7;
	pm->CKSEL.pbadiv = 1;
}

void PM_deselectPBA ()
{
	pm->CKSEL.pbasel = 0;
	pm->CKSEL.pbadiv = 0;
}

// ----------------------------------------------------------
// ----------------------------------------------------------
// ----------------------------------------------------------
// ----------------------------------------------------------
// Clock Masks
// ----------------------------------------------------------


// ----------------------------------------------------------
// enable TC clock
// ----------------------------------------------------------
void PM_enableTCClock ()
{
	pm->pbamask = (pm->pbamask | PM_TC_CLOCK_MASK);
}

// ----------------------------------------------------------
// disable TC clock
// ----------------------------------------------------------
void PM_disableTCClock ()
{
	pm->pbamask = (pm->pbamask & ~PM_TC_CLOCK_MASK);
}










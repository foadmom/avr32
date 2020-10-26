#ifndef CGPIO_H_
#define CGPIO_H_

#include "common.h"

#if __GNUC__
#  include <avr32/io.h>
#elif __ICCAVR32__
#  include <avr32/iouc3a0512.h>
#else
#  error Unknown compiler
#endif

typedef struct JoyStick_struct
{
	uchar 		north;
	uchar		south;
	uchar		east;
	uchar		west;
} JoyStick;

#define NORTH_PORT 		AVR32_PIN_PB06
#define SOUTH_PORT 		AVR32_PIN_PB07
#define EAST_PORT 		AVR32_PIN_PB08
#define WEST_PORT 		AVR32_PIN_PB09
JoyStick	getJoyStickFlags ();
uchar 		joyStickActivated ();
void CGPIO_initialisejoystick ();

void CGPIO_registerInterrupt ();

void setGpioPinAsOutput     (uint pin);
void setGpioPortValue       (uint pin);
void clearGpioPortValue     (uint pin);
void gpioTogglePortValue    (uint pin);
uint gpioPortValue          (uint pin);

void setGpioPinAsInput      (uint pin);

void gpioEnableInterrupt    (uint pin);
void gpioClearPinInterruptFlag    (uint pin);

void setPinFunction (uint pin, uint function);
void CGPIO_setGlitchFilter (uint pin);
void CGPIO_clearGlitchFilter (uint pin);

#endif /*CGPIO_H_*/

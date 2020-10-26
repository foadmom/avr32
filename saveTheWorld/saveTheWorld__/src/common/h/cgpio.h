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
JoyStick	getJoyStickFlags 	();
uchar 		joyStickActivated 	();
void CGPIO_initialisejoystick 	();

void gpioSetInterruptModeAnyChange 		(uint pin);
void gpioSetInterruptModeRisingEdge		(uint pin);
void gpioSetInterruptModeAnyFallingEdge	(uint pin);

void setPinToGpioFunction 		(uint pin);

void CGPIO_registerInterrupt 	();
void gpioSetInterruptMode 		(uint pin, uint imr0Bit, uint imr1Bit);
void gpioEnableInterrupt    	(uint pin);
void gpioDisableInterrupt 		(uint pin);
void gpioClearPinInterruptFlag  (uint pin);

void setGpioPinAsOutput     	(uint pin);
void setGpioPinAsInput      	(uint pin);

void gpioEnablePullupResistor  	(uint pin);
void gpioDisablePullupResistor	(uint pin);

void gpioSetPortValue       	(uint pin);
void gpioClearPortValue     	(uint pin);
void gpioTogglePortValue    	(uint pin);
uint gpioGetPortValue          	(uint pin);

void setPinFunction 			(uint pin, uint function);
void gpioSetGlitchFilter 		(uint pin);
void gpioClearGlitchFilter 	(uint pin);

uint getSingleWireInputLevel    ();
void setSingleWireInputLevel    (uint level);


#endif /*CGPIO_H_*/

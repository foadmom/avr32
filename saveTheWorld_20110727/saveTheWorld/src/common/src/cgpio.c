

#include <stdio.h>
#include <string.h>

#include "cgpio.h"
#include "cintc.h"
#include "cusart.h"


volatile static JoyStick	joyStick;

void PB02_interrupt ();
void PB03_interrupt ();
void PA13_joyStickHandler ();
void NORTH_interrupt ();
void SOUTH_interrupt ();
void EAST_interrupt  ();
void WEST_interrupt  ();

// with this chip there are AVR32_GPIO_PORT_LENGTH sets of ports
// and each have a 32 bit io.
// set the interrupt handler of the ones you are interested.
static function PORTS [AVR32_GPIO_PORT_LENGTH][32] = 
{
	{ 0,0,0,0,0,0,0,0,    // PA00-PA07
	  0,0,0,0,0,&PA13_joyStickHandler,0,0,  // PA08-PA15
	  0,0,0,0,0,0,0,0,    // PA16-PA23
	  0,0,0,0,0,0,0,0},   // PA24-PA31
	{ 0,0,&PB02_interrupt,&PB03_interrupt,0,0,&NORTH_interrupt,&SOUTH_interrupt,   // PB00-PB07
	  &EAST_interrupt,&WEST_interrupt,0,0,0,0,0,0,    // PB08-PB15
	  0,0,0,0,0,0,0,0,    // PB16-PB23
	  0,0,0,0,0,0,0,0}    // PB24-PB32
};
	
//! Base address of the GPIO modules
volatile avr32_gpio_t *cgpio = (volatile avr32_gpio_t*) &AVR32_GPIO;


// -------------------------------------------------------------
// joy stick
// -------------------------------------------------------------

__always_inline void resetJoyStick ()
{
	memset ((void*)&joyStick, 0, sizeof (JoyStick));
}

JoyStick	getJoyStickFlags ()
{
	JoyStick result;
	memcpy ((void*)&result, (const void*)&joyStick, sizeof(JoyStick));
	resetJoyStick ();
	return (result);
}

uchar joyStickActivated ()
{
	return (joyStick.north | joyStick.south | joyStick.east | joyStick.west);
}

__always_inline uint checkAndReset (avr32_gpio_port_t *gpioPort, uint mask)
{
	uint value = gpioPort->ifrc &= mask;
	gpioPort->ifrc &= mask;
	return value;
}

void CGPIO_initialisejoystick ()
{
    setGpioPinAsInput (AVR32_PIN_PA13);
	gpioEnableInterrupt (AVR32_PIN_PA13);
	setGpioPinAsInput (AVR32_PIN_PB06);
	gpioEnableInterrupt (AVR32_PIN_PB06);
	setGpioPinAsInput (AVR32_PIN_PB07);
	gpioEnableInterrupt (AVR32_PIN_PB07);
	setGpioPinAsInput (AVR32_PIN_PB08);
	gpioEnableInterrupt (AVR32_PIN_PB08);
	setGpioPinAsInput (AVR32_PIN_PB09);
	gpioEnableInterrupt (AVR32_PIN_PB09);
	resetJoyStick ();
}

void PA13_joyStickHandler ()
{
	resetJoyStick ();
}


// -----------------------------------------------------------------------
void PB02_interrupt ()
{
	gpioTogglePortValue (AVR32_PIN_PA22);
}

void PB03_interrupt ()
{
	gpioTogglePortValue (AVR32_PIN_PA22);
}

void NORTH_interrupt ()
{
	++joyStick.north;
}

void SOUTH_interrupt ()
{
	++joyStick.south;
}

void EAST_interrupt ()
{
	++joyStick.east;
}

void WEST_interrupt ()
{
	++joyStick.west;
}


// interrupt handler for a group of 8 pins starting with spin
void gpio_IRQ_handler (uint spin)
{
	uint portsBlockIndex = spin/32;	// max is AVR32_GPIO_PORT_LENGTH
	volatile avr32_gpio_port_t *gpioPort = (volatile avr32_gpio_port_t *) &cgpio->port[portsBlockIndex];
	uint _ifr = gpioPort->ifr;
	uint portBitStart = spin%32;
	uint portBitEnd = portBitStart+AVR32_GPIO_IRQS_PER_GROUP;
	uint portBit;
	uint mask = 1 << portBitStart;
	for (portBit=portBitStart; portBit<portBitEnd; portBit++)
	{
		// if interrupt bit is set
        if ( (_ifr) & mask )
        {
			function handler = PORTS [portsBlockIndex][portBit];
			if (handler != 0)
			{
				// and there is handler for it, then call the handler
		        handler ();
	        }
			// reset the interrupt flag bit otherwise interrupt will not be enabled.
        	gpioPort->ifrc &= mask;
        }
        mask = mask << 1;
	}
}

__isr__ static void gpio_IRQ1_handler(void)
{
	gpio_IRQ_handler (AVR32_PIN_PA08);
}


__isr__ static void gpio_IRQ4_handler(void)
{
	gpio_IRQ_handler (AVR32_PIN_PB00);
}

__isr__ static void gpio_IRQ5_handler(void)
{
	gpio_IRQ_handler (AVR32_PIN_PB08);
}

// -----------------------------------------------------------------------------
// gpio interrupt service routine
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// register the gpio service interrupt routine with the INTC
// -----------------------------------------------------------------------------
void CGPIO_registerInterrupt ()
{
	// pin number/8 = IRQ line
	INTC_register_interrupt(&gpio_IRQ1_handler, AVR32_GPIO_IRQ_1, GPIO_INT_LEVEL);
	INTC_register_interrupt(&gpio_IRQ4_handler, AVR32_GPIO_IRQ_4, GPIO_INT_LEVEL);
	INTC_register_interrupt(&gpio_IRQ5_handler, AVR32_GPIO_IRQ_5, GPIO_INT_LEVEL);
}

// ------------------------------------------------
// set the pin to be a GPIO output port
// ------------------------------------------------
void setGpioPinAsOutput (uint pin)
{
    // The port holding that pin.
    volatile avr32_gpio_port_t *gpioPort = (volatile avr32_gpio_port_t *) &cgpio->port[pin/32];

    gpioPort->oders = (1<<(pin%32)); // The GPIO output driver is enabled for that pin.
    gpioPort->gpers = (1<<(pin%32)); // The GPIO module controls that pin.
}

// ------------------------------------------------
// set the pin to be a GPIO input port
// ------------------------------------------------
void setGpioPinAsInput (uint pin)
{
    // The port holding that pin.
    volatile avr32_gpio_port_t *gpioPort = (volatile avr32_gpio_port_t *) &cgpio->port[pin/32];

    gpioPort->oderc = (1<<(pin%32)); // The GPIO output driver is disabled for that pin.
    gpioPort->gpers = (1<<(pin%32)); // The GPIO module controls that pin.
}

// ------------------------------------------------
// sets the output port pin value
// ------------------------------------------------
void setGpioPortValue (uint pin)
{
    // The port holding that pin.
    volatile avr32_gpio_port_t *gpioPort = (volatile avr32_gpio_port_t *) &cgpio->port[pin/32];

    gpioPort->ovrs  = (1<<(pin%32)); // Value to be driven on the I/O line: 1
}

// ------------------------------------------------
// clears the output port pin value
// ------------------------------------------------
void clearGpioPortValue (uint pin)
{
    // The port holding that pin.
    volatile avr32_gpio_port_t *gpioPort = (volatile avr32_gpio_port_t *) &cgpio->port[pin/32];
    gpioPort->ovrc  = (1<<(pin%32)); // Value to be driven on the I/O line: 0
}

// ------------------------------------------------
// toggles the output port pin value
// ------------------------------------------------
void gpioTogglePortValue (uint pin)
{
    // The port holding that pin.
    volatile avr32_gpio_port_t *gpioPort = (volatile avr32_gpio_port_t *) &cgpio->port[pin/32];
    gpioPort->ovrt  = (1<<(pin%32));       // Toggle the I/O line.
}


void gpioClearPinInterruptFlag(uint pin)
{
    // The port holding that pin.
    volatile avr32_gpio_port_t *gpioPort = (volatile avr32_gpio_port_t *) &cgpio->port[pin/32];
    gpioPort->ifrc = 1 << (pin & 0x1F);
}


// ------------------------------------------------
// read the input port pin value
// ------------------------------------------------
uint gpioPortValue (uint pin)
{
    volatile avr32_gpio_port_t *gpioPort = ( volatile avr32_gpio_port_t *) &cgpio->port[pin/32];
    return (gpioPort->pvr >>(pin%32))&1;
}

// ------------------------------------------------
// 
// ------------------------------------------------
void gpioEnableInterrupt (uint pin)
{
    volatile avr32_gpio_port_t *gpioPort = ( volatile avr32_gpio_port_t *) &cgpio->port[pin/32];
    gpioPort->imr0c = 1 << (pin & 0x1F);
    gpioPort->imr1s = 1 << (pin & 0x1F);
    gpioPort->iers  = 1 << (pin & 0x1F);
}


// ------------------------------------------------
// set pin function
// ------------------------------------------------
void setPinFunction (uint pin, uint function)
{
    volatile avr32_gpio_port_t *gpioPort = ( volatile avr32_gpio_port_t *) &cgpio->port[pin/32];

    // Enable the correct function.
    switch (function)
    {
    case 0: // A function.
        gpioPort->pmr0c = 1 << (pin & 0x1F);
        gpioPort->pmr1c = 1 << (pin & 0x1F);
        break;

    case 1: // B function.
        gpioPort->pmr0s = 1 << (pin & 0x1F);
        gpioPort->pmr1c = 1 << (pin & 0x1F);
        break;

    case 2: // C function.
        gpioPort->pmr0c = 1 << (pin & 0x1F);
        gpioPort->pmr1s = 1 << (pin & 0x1F);
        break;
    }

    // Disable GPIO control.
    gpioPort->gperc = 1 << (pin & 0x1F);
}


// ------------------------------------------------
// set glitch fileter
// ------------------------------------------------
void CGPIO_setGlitchFilter (uint pin)
{
	// The port holding that pin.
	volatile avr32_gpio_port_t *gpioPort = (volatile avr32_gpio_port_t *) &cgpio->port[pin/32];
	gpioPort->gfers  = (1<<(pin%32));       // set the set register
}


// ------------------------------------------------
// clear glitch fileter
// ------------------------------------------------
void CGPIO_clearGlitchFilter (uint pin)
{
	// The port holding that pin.
	volatile avr32_gpio_port_t *gpioPort = (volatile avr32_gpio_port_t *) &cgpio->port[pin/32];
	gpioPort->gferc  = (1<<(pin%32));       // set the clear register
}



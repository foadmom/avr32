#ifndef CADC_H_
#define CADC_H_

#include "cgpio.h"
#include "cintc.h"

// Connection of the temperature sensor
#  define EXAMPLE_ADC_TEMPERATURE_CHANNEL     7
#  define EXAMPLE_ADC_TEMPERATURE_PIN         AVR32_ADC_AD_7_PIN
#  define EXAMPLE_ADC_TEMPERATURE_FUNCTION    AVR32_ADC_AD_7_FUNCTION
// Connection of the light sensor
#  define EXAMPLE_ADC_LIGHT_CHANNEL           6
#  define EXAMPLE_ADC_LIGHT_PIN               AVR32_ADC_AD_6_PIN
#  define EXAMPLE_ADC_LIGHT_FUNCTION          AVR32_ADC_AD_6_FUNCTION
// Note: Corresponding defines are defined in /BOARDS/EVK1101/evk1101.h.
// These are here for educational purposes only.

extern void ADC_setInterruptHandler ();

#include "cintc.h"
#include "common.h"

typedef ushort     ADCBuffer[AVR32_ADC_ADC_CHANNELS_MSB+1];

typedef struct ADC_conversion_struct
{
	volatile ADCBuffer 	ADC_Buffer;
	volatile uchar  	ADC_conversionReady;
} ADC_conversion;


ADCBuffer* ADC_getBuffer ();
void       ADC_BufferInit ();
int        ADC_start();
uchar      ADC_conversionComplete ();

__always_inline  void ADC_setLowResMode ()
{
	volatile avr32_adc_t *_adc = &AVR32_ADC;
    _adc->MR.lowres = 1;
}

__always_inline  void ADC_setHighResMode ()
{
	volatile avr32_adc_t *_adc = &AVR32_ADC;
    _adc->MR.lowres = 0;
}

__always_inline  void ADC_setSampleAndHoldTime (unsigned int _shtim)
{
	volatile avr32_adc_t *_adc = &AVR32_ADC;
    _adc->MR.shtim = _shtim;
}

__always_inline  void ADC_setStartUpTime (unsigned int _startup)
{
	volatile avr32_adc_t *_adc = &AVR32_ADC;
    _adc->MR.startup = _startup;
}


__always_inline void ADC_enable (unsigned short channel)
{
	volatile avr32_adc_t *_adc = &AVR32_ADC;

  	// enable channel
  	_adc->cher = (1 << channel);
}

__always_inline void ADC_disable (unsigned short channel)
{
	volatile avr32_adc_t *_adc = &AVR32_ADC;

    // disable channel
    _adc->chdr |= (1 << channel);
}

__always_inline void ADC_enableDataReadyInterrupt ()
{
	volatile avr32_adc_t *_adc = &AVR32_ADC;

    // enable DataReady interrupt
    _adc->IER.drdy = 1;
}

__always_inline void ADC_disableDataReadyInterrupt ()
{
	volatile avr32_adc_t *_adc = &AVR32_ADC;

    // disable DataReady interrupt
    _adc->IDR.drdy = 1;
}


__always_inline unsigned int ADC_readData (unsigned int channel)
{
	volatile avr32_adc_t *_adc = &AVR32_ADC;
	return *((unsigned long * )((&(_adc->cdr0)) + channel));
}

#endif /*CADC_H_*/

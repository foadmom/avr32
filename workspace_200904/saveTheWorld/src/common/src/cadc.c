
#include <avr32/io.h>
#include "cadc.h"
#include "cusart.h"

static volatile ADC_conversion  adc_conversion; 


__isr__ static void ADC_InterruptHandler(void)
{
	uint channel;
	for (channel=0; channel<=AVR32_ADC_ADC_CHANNELS_MSB; channel++)
	{
		adc_conversion.ADC_Buffer[channel] = ADC_readData (channel);
	}
	adc_conversion.ADC_conversionReady = 1;
}

void ADC_BufferInit ()
{
	uint channel;
	for (channel=0; channel<=AVR32_ADC_ADC_CHANNELS_MSB; channel++)
	{
		adc_conversion.ADC_Buffer[channel] = 0;
	}
	adc_conversion.ADC_conversionReady = 0;
}

uchar ADC_conversionComplete ()
{
	return adc_conversion.ADC_conversionReady;
}

void ADC_setInterruptHandler ()
{
	INTC_register_interrupt(&ADC_InterruptHandler, AVR32_ADC_IRQ, ADC_INT_LEVEL);
}


ADCBuffer* ADC_getBuffer ()
{
	return (ADCBuffer*) adc_conversion.ADC_Buffer;
}


int ADC_start()
{
	int rc = OK;
	volatile avr32_adc_t *_adc = &AVR32_ADC;
  	ADC_BufferInit ();
  	// start conversion
  	_adc->CR.start = 1;
  	
  	return rc;
}


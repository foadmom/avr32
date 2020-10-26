
// This module performs some of the termperature conversions
// and calculations 
// It relies on the ADC_toTemperatureConversion array to create 
// a chart of index (ADC_Reading/4) and to find the temperature 
// matching temperature.
// 

#include <string.h>

#include "common.h"
#include "ctc.h"
#include "temperature.h"
#include "cusart.h"


void readADCConversions (TemperatureBuffer tempBuffer)
{
	memcpy (tempBuffer, ADC_getBuffer (), sizeof (TemperatureBuffer));
}

TemperatureBuffer* convertToTemperature (TemperatureBuffer buffer)
{
	int  sensorIndex = 0;
	for (sensorIndex=TEMP_SENSOR_FIRST; sensorIndex<=TEMP_SENSOR_LAST; sensorIndex++)
	{
		uint index = 64-(buffer[sensorIndex]/4);
		buffer[sensorIndex] = ADC_toTemperatureConversion[index]-20;
	}
	return (TemperatureBuffer*)buffer;
}

int initiateTemperatureSensors ()
{
	int rc = ADC_start ();
	return rc;
}

TemperatureBuffer* getTemperatureReadings (TemperatureBuffer buffer)
{
	if (temperatureDataReady() )
	{
		readADCConversions (buffer);
		convertToTemperature (buffer);
		return (TemperatureBuffer*)buffer;
	}
	else return null;
}

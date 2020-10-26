

#include "common.h"
#include "control.h"
#include "cusart.h"
#include "command.h"
#include "ctc.h"
#include "crtc.h"
#include "temperature.h"
#include "singleWire.h"


uint checkForDataReceived ()
{
	int  rc = __OK;
	if (commandReceived ())
	{
		char command[128]={0};
		uint commandLen = readCommand((char *) &command);
		if (commandLen > 0) rc = processMessage ((char *) &command);
	}
	return rc;
}


void printSingleWireData (char* data)
{
	debugf ("DS data = %x, %x, %x, %x, %x, %x, %x, %x, %x, %s",
			data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], NEWLINE);
}

static TemperatureBuffer tempBuffer;
static char timeSt [32];

int mainLoop ()
{
	int rc = __OK;
	int tempSensorInitiated = 0;
	int __index = 0;
	
	while (1) //rc == __OK)
	{

		__index++;
		// kick off the temp. sensor reading
		if (tempSensorInitiated == 0)
		{
			initiateTemperatureSensors ();
			tempSensorInitiated = 1;
		}
		
		// has joystick been moved
		if (joyStickActivated ())
		{
			JoyStick _joyStick = getJoyStickFlags();
			gpioTogglePortValue (AVR32_PIN_PA07);
			debugf ("joyStick   north=%u south=%u east=%u west=%u%s", _joyStick.north, _joyStick.south, _joyStick.east, _joyStick.west, NEWLINE);
		}

		// check to see if temperature reading has been completed
		if (getTemperatureReadings (tempBuffer) != null)
		{
			debugf ("%u %s  light meter = %d     temperature=%dc%s", __index, timeSt, tempBuffer[6], tempBuffer[7], NEWLINE);
			tempSensorInitiated = 0;
		}

		// send couple of commands through single wire channel.
		// just for fun
//		int _rc = resetAndCheckPresence ();
//		if (_rc == 0 )
//		{
//			debugln ("ds18s20 is present");
//		}
//		else
//		{
//			debugln ("ds18s20 is NOT present");
//		}
		uchar _data[] = {0,0,0,0,0,0,0,0,0,0,0,0};
//	    writeDataToScratchPad (0x13, 0x25);
		getTemperatureNoRom (_data);
		// check for incoming data through rs232
		rc = checkForDataReceived ();
		printSingleWireData (_data);
		
		wait_msec (3000);
	}
	
	return rc;
}

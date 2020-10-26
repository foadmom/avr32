

#include "common.h"
#include "control.h"
#include "cusart.h"
#include "command.h"
#include "ctc.h"
#include "crtc.h"
#include "temperature.h"


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
			debugf ("joyStick   north=%u south=%u east=%u west=%u\n", _joyStick.north, _joyStick.south, _joyStick.east, _joyStick.west);
		}

		// check to see if temperature reading has been completed
		if (getTemperatureReadings (tempBuffer) != null)
		{
			timeToString ((char*)timeSt);
			debugf ("%u %s  light meter = %d     temperature=%dc \n", __index, timeSt, tempBuffer[6], tempBuffer[7]);
			tempSensorInitiated = 0;
		}		

		// check for incoming data through rs232
		rc = checkForDataReceived ();
		
		uint __ra = TC_getRA (2);
//		TC_getCompareRAStatus (2);
//		TC_getCompareRBStatus (2);
//		TC_setRA (0, (__ra+300) & 0x0000ffff);
		uint __rb = TC_getRB (2);
		uint __cv2 = TC_counterValue (2);
		uint __sr  = TC_readStatusRegister (2);
		debugf ("ra2 = %u   rb2=%u  cv2=%u  sr=%x\n", __ra, __rb, __cv2, __sr);
		
		wait_us (1000);
	}
	
	return rc;
}

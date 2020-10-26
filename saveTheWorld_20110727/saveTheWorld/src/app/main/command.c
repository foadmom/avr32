//
// This the command processing module 
// Commands may be sent externally or internally for processing.
// The address and sub-address may not be used at the moment.
// 
// command message format
//                 address                       command
//           --------------------  --------------------------------------
//           |                  |  |                                    |
//      SOM, address, sub_address, command category, command sub_category, data, EOM
// bytes 1      2          2              2                    2            var   1
//
//
// example:   "!@#1122ecst15:35:40#@!" set the time to 15:35:40
//            "!@#1122ecsd08:05:15#@!" set the date to 15/05/2008
// 
// 


#include <avr32/io.h>
#include "command.h"
#include "crtc.h"
#include "cusart.h"

void resetMessageStruct (Message* message)
{
	memset (message, 0, sizeof (Message));
}

Message* createMessageStruct (char* messageText, Message* message)
{
	Message*	result = message;
	
	resetMessageStruct (message);
	memcpy (message->header.som,        messageText, SOM_LENGTH);
	if ( strcmp (message->header.som, SOM) != 0) result = 0;
	else
	{
		messageText += SOM_LENGTH;
		memcpy (message->header.address,    messageText, ADDRESS_LENGTH);
		messageText += ADDRESS_LENGTH;
		memcpy (message->header.subAddress, messageText, SUB_ADDRESS_LENGTH);
		messageText += SUB_ADDRESS_LENGTH;
		memcpy (message->header.command,    messageText, COMMAND_CAT_LENGTH);
		messageText += COMMAND_CAT_LENGTH;
		memcpy (message->header.sub_command, messageText, COMMAND_SUB_CAT_LENGTH);
		messageText += COMMAND_SUB_CAT_LENGTH;
		char* eom = strstr (messageText, EOM);
		if (eom == 0)
		{
			result = 0;
		}
		else
		{
			uint dataLen = eom - messageText;
			if (dataLen > MESSAGE_DATA_LEN) result = 0;
			else
			{
				memcpy (message->data, messageText, dataLen);
			}
		}
	}
	
	return result;
}

int convertToInt (char* data, int len)
{
	char  st [8];
	memset (st, 0 , 8);
	memcpy (st, data, len);
	int result = atoi (st);
	return result;
}

// -------------------------------------------------------------
// -------------------------------------------------------------
void processSetTimeCommand (Message* message)
{
	uint hour   = convertToInt (message->data, 2);
	uint minute = convertToInt (message->data+3, 2);
	uint second = convertToInt (message->data+6, 2);
	
	setTime (hour, minute, second);
}

void processSetDateCommand (Message* message)
{
	uint year_  = convertToInt (message->data, 2);
	uint month_ = convertToInt (message->data+3, 2);
	uint day_   = convertToInt (message->data+6, 2);
	
	setDate (year_, month_, day_);
}

// -------------------------------------------------------------
// -------------------------------------------------------------
void processHardwareConfigCommand (Message* message)
{
	
}

void processEnvironmentConfigCommand (Message* message)
{
	if (strcmp (message->header.sub_command, EC_SET_TIME) == 0) processSetTimeCommand (message);
	if (strcmp (message->header.sub_command, EC_SET_DATE) == 0) processSetDateCommand (message);
}

void processSoftwareConfigCommand (Message* message)
{
	
}

void processApplicationConfigCommand (Message* message)
{
	
}

// -------------------------------------------------------------
// -------------------------------------------------------------
void processCommand (Message* message)
{
	if (strcmp (message->header.command, HARDWARE_CONFIGURATION)         == 0) processHardwareConfigCommand (message);
	else if (strcmp (message->header.command, ENVIRONMENT_CONFIGURATION) == 0) processEnvironmentConfigCommand (message);
	else if (strcmp (message->header.command, SOFTWARE_CONFIGURATION)    == 0) processSoftwareConfigCommand (message);
	else if (strcmp (message->header.command, APPLICATION_CONFIGURATION) == 0) processApplicationConfigCommand (message);
}


// -------------------------------------------------------------
// -------------------------------------------------------------
int processMessage (char* messageText)
{
	int 		rc = 0;
	Message		message;
	if (createMessageStruct (messageText, &message) )
	{
		char st[rxBufferLimit+1];
		sprintf (st, "som=%s address=%s subAddress=%s commandCat=%s commandSubCat=%s data=%s\n",
				          message.header.som, 
				                     message.header.address,
				                                    message.header.subAddress,
				                                                  message.header.command,
				                                                                   message.header.sub_command,
				                                                                           message.data);
		USART1_WriteLine ((const char*)&st);
		
		processCommand (&message);
	}
	else
	{
		debug (messageText);
		debug ("\n         Invalid message format\n");
		rc = ERROR_INVALID_COMMAND_FORMAT;
	}
	return rc;
}


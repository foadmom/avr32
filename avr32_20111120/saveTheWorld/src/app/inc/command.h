#ifndef COMMAND_H_
#define COMMAND_H_

#include "common.h"

// command message format
//                 address                       command
//           --------------------  --------------------------------------
//           |                  |  |                                    |
//      SOM, address, sub_address, command category, command sub_category, data, EOM
// bytes 1      2          2              2                    2            var   1
// 
//

#define SOM_LENGTH					3
#define ADDRESS_LENGTH				2
#define SUB_ADDRESS_LENGTH			2
#define COMMAND_CAT_LENGTH   		2
#define COMMAND_SUB_CAT_LENGTH		2
#define MESSAGE_DATA_LEN			128

// the following parts of the message are in binary format
//#define SOM								0x02
//#define EOM								0x03

// the following parts of the message are in text format
// command category
#define SOM								"!@#"
#define EOM								"#@!"

#define HARDWARE_CONFIGURATION			"hc"
#define ENVIRONMENT_CONFIGURATION		"ec"
#define SOFTWARE_CONFIGURATION          "sc"
#define APPLICATION_CONFIGURATION       "ac"
#define COMMAND_USART					USART1

#define readCommand(line)				USART1_ReadLine(line)
#define commandReceived                 USART1_isDataReady

// sub-command category
#define EC_SET_TIME						"st"
#define EC_SET_DATE						"sd"


typedef struct MessageHeader_struct
{
	char 			som [SOM_LENGTH+1];
	char			address [ADDRESS_LENGTH+1];
	char			subAddress [SUB_ADDRESS_LENGTH+1];
	char			command [COMMAND_CAT_LENGTH+1];
	char			sub_command [COMMAND_SUB_CAT_LENGTH+1];
} MessageHeader;

typedef struct Message_struct
{
	MessageHeader	header;
	char			data [MESSAGE_DATA_LEN+1];
} Message;



int processMessage (char* messageText);


#endif /*COMMAND_H_*/

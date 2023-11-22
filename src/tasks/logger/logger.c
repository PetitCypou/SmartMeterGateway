

#include <FreeRTOS.h>
#include <task.h>

#include "lib/smgDma/smgDma.h"
#include "lib/ticParser/ticParser.h"
#include "lib/ticConstructor/ticConstructor.h"
#include "lib/smgNetwork/smgNetwork.h"

/*
 * logger.c
 *
 *  Created on: 23 nov. 2023
 *      Author: cyp
 */
/*
 * Task:  logger_task
 * --------------------
 * Looks at the incoming data, parses it and sends it to a remote server.
 *
 *  void
 *
 *  returns: void
 */
void logger_task(void *argument)
{
	char pUartWord[256]="";
	char Frame[1024] = "";
	uint8_t* LOG_SOCKET = (uint8_t*) argument;

	struct ticFrame *pTIC, TIC;
	pTIC = &TIC;
	memset(&TIC,0,sizeof(TIC));

	//Ethernet destination configuration
	uint16_t sendPort = 5000;
	uint16_t destPort = 5000;
	uint8_t destIp[] = {
		192, 168, 11, 3
	};

	smgNetwork_Connect(*LOG_SOCKET,sendPort,destIp,destPort);	//Initial connection to port.

	while(1)
	{
		if (smgDma_Fetch(pUartWord) != 0) 		//Check if ready to receive from UART. If ready, pUartWord points so a word string.
		{
			switch(ticParse(pTIC,pUartWord)){
			case 2:											//If Tic values are valid and ready to send.
				ticConstruct(Frame, TIC);					//Create a network frame.
				smgNetwork_Send(*LOG_SOCKET,sendPort,destIp,destPort,Frame);		//Send frame.
				break;
			case 0:											//If received Tic frame was invalidated at some point.
				smgNetwork_Send(*LOG_SOCKET,sendPort,destIp,destPort,"INVALID\n");	//Report invalid frame to server ?
				break;
			default:
				break;
			}
		}
		vTaskDelay(500);// 50ms
	}
}

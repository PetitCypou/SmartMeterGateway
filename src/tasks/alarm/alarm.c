/*
 * logger.c
 *
 *  Created on: 23 nov. 2023
 *      Author: cyp
 */

#include <FreeRTOS.h>
#include <task.h>

#include "lib/smgDma/smgDma.h"
#include "lib/ticParser/ticParser.h"
#include "lib/ticConstructor/ticConstructor.h"
#include "lib/smgNetwork/smgNetwork.h"

/*
 * Task:  alarm_task
 * --------------------
 * Looks at the incoming data, check if "ADPS" is present and send and alarm accordingly.
 *
 *  void
 *
 *  returns: void
 */
void alarm_task(void *argument)
{
	//Ethernet destination configuration
	uint16_t sendPort = 4000; //FIXME : fetch config from config lib
	uint16_t destPort = 4000;
	uint8_t destIp[] = {
		192, 168, 11, 3
	};

	uint8_t* ALARM_SOCKET = (uint8_t*) argument;

	smgNetwork_Connect(*ALARM_SOCKET,sendPort,destIp,destPort);	//Initial connection to port.

	while(1)
	{
		vTaskDelay(40);// 4ms
		if(smgDma_Find("ADPS")){	//Check for the alarm in the DMA ring buffer every 4ms.
			ticSetAlarm();
		}
		if(ticGetAlarm()){ 			//While the alarm is active.
			smgNetwork_Send(*ALARM_SOCKET,sendPort,destIp,destPort,"ALARM!\n");	//Report alarm to server.
			vTaskDelay(2000);		//Cooldown on sending alarm packets. (200ms)
		}
	}
}


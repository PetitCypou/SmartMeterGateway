/*
 * ticNetwork.h
 *
 *  Created on: 3 nov. 2023
 *      Author: cyp
 */

#ifndef TICNETWORK_H_
#define TICNETWORK_H_

#include <stdint.h>

/* DATA_BUF_SIZE defined from Loopback example */
#ifndef DATA_BUF_SIZE
	#define DATA_BUF_SIZE			2048
#endif

//FUNCTION PROTOTYPES
int8_t aegNetwork_Send(char* buf, uint8_t* destip, uint16_t destPort);

#endif /* TICNETWORK_H_ */

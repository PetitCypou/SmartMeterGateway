/*
 * ticNetwork.h
 *
 *  Created on: 3 nov. 2023
 *      Author: cyp
 */

#ifndef TICNETWORK_H_
#define TICNETWORK_H_

#include <stdint.h>
#include <wizchip_conf.h>

/* DATA_BUF_SIZE defined from Loopback example */
#ifndef DATA_BUF_SIZE
	#define DATA_BUF_SIZE 2048
#endif

//FUNCTION PROTOTYPES
uint8_t smgNetwork_Init(uint32_t clkKhz);
uint8_t smgNetwork_Connect(uint8_t* destIp, uint16_t destPort);
int8_t smgNetwork_Send(char* buf, uint8_t* destip, uint16_t destPort);

/* DHCP */
static void wizchip_dhcp4_init(void);
static void wizchip_dhcp4_assign(void);
static void wizchip_dhcp4_conflict(void);

#endif /* TICNETWORK_H_ */

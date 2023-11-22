/*
 * smgDma.h
 *
 *  Created on: 14 nov. 2023
 *      Author: cyp
 */

#ifndef SMGDMA_H_
#define SMGDMA_H_

#include <stdint.h>
#include "hardware/uart.h"
//STRUCTURE BUFFER CIRCULAIRE
struct ringBuf {
	char buf[256];
	volatile uint32_t* pWrite;
	char* pRead;
};

//FUNCTION PROTOTYPES
void dma_handler();
void smgDma_Init(uart_inst_t* UART_ID, struct ringBuf* pRING);
uint8_t smgDma_Fetch(char* patchBuf);
uint8_t smgDma_Find(char* patchBuf);

#endif /* SMGDMA_H_ */

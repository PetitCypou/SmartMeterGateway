/*
 * aegDma.h
 *
 *  Created on: 14 nov. 2023
 *      Author: cyp
 */

#ifndef AEGDMA_H_
#define AEGDMA_H_

//STRUCTURE BUFFER CIRCULAIRE
struct ringBuf {
	char buf[256];
	volatile uint32_t* pWrite;
	char* pRead;
};

//FUNCTION PROTOTYPES
void dma_handler();
void aegDma_Init(uart_inst_t* UART_ID, struct ringBuf* pRING);
uint8_t aegDma_Fetch(char* patchBuf);

#endif /* AEGDMA_H_ */

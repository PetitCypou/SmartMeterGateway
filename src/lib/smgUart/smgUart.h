/*
 * aegUart.h
 *
 *  Created on: 6 nov. 2023
 *      Author: cyp
 */

#ifndef SMGUART_H_
#define SMGUART_H_

#include "hardware/uart.h"

//FUNCTION PROTOTYPE
void ticUart_Init(uart_inst_t* UART_ID, uint BAUD_RATE, uint DATA_BITS, uint STOP_BITS, uart_parity_t PARITY, uint UART_TX_PIN, uint UART_RX_PIN);

#endif /* SMGUART_H_ */

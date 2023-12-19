/*
 * aegUart.c
 *
 *  Created on: 6 nov. 2023
 *      Author: cyp
 */

#include "aegUart.h"
#include <hardware/gpio.h>
#include <hardware/uart.h>

//FUNCTION PROTOTYPES
void ticUart_Init(uart_inst_t* UART_ID, uint BAUD_RATE, uint DATA_BITS, uint STOP_BITS, uart_parity_t PARITY, uint UART_TX_PIN, uint UART_RX_PIN);

void ticUart_Init(uart_inst_t* UART_ID, uint BAUD_RATE, uint DATA_BITS, uint STOP_BITS, uart_parity_t PARITY, uint UART_TX_PIN, uint UART_RX_PIN){
	//SETUP UART
	uart_init(UART_ID, BAUD_RATE);
	gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
	gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    uart_set_hw_flow(UART_ID, false, false);
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);
    uart_set_fifo_enabled(UART_ID, false);
}

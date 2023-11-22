/**
 * ----------------------------------------------------------------------------------------------------
 * Includes
 * ----------------------------------------------------------------------------------------------------
 */
#include <port_common.h>

#include "lib/smgUart/smgUart.h"
#include "lib/smgDma/smgDma.h"
#include "lib/smgNetwork/smgNetwork.h"

#include <FreeRTOS.h>
#include <task.h>

#include "tasks/webServer/webServer.h"
#include "tasks/logger/logger.h"
#include "tasks/alarm/alarm.h"



/* FreeRTOS Tasks*/
#define LOGGER_TASK_STACK_SIZE 2048
#define LOGGER_TASK_PRIORITY 8
#define ALARM_TASK_STACK_SIZE 2048
#define ALARM_TASK_PRIORITY 10
#define HTTP_TASK_STACK_SIZE 2048
#define HTTP_TASK_PRIORITY 6

/* System clock */
#define PLL_SYS_KHZ (133 * 1000)

/* UART Parameters */
#define UART_ID uart0
#define BAUD_RATE 1200
#define DATA_BITS 7
#define STOP_BITS 1
#define PARITY UART_PARITY_EVEN
#define UART_TX_PIN 0
#define UART_RX_PIN 1

/* ETH Parameters */
const uint8_t LOG_SOCKET = 2;
const uint8_t ALARM_SOCKET = 1;


/**
 * ----------------------------------------------------------------------------------------------------
 * Main
 * ----------------------------------------------------------------------------------------------------
 */
int main()
{
	/* Init IO for verbose debugging */
	stdio_init_all();

	/* Declare & initialize program variables*/
	struct ringBuf *pRING, RING;
	pRING = &RING;

	//Initialize UART Communication through DMA
	ticUart_Init(UART_ID, BAUD_RATE, DATA_BITS, STOP_BITS, PARITY, UART_TX_PIN, UART_RX_PIN);
	smgDma_Init(UART_ID,pRING);

	//Initialize Network comms.
	smgNetwork_Init(PLL_SYS_KHZ);

    xTaskCreate(logger_task, "LOGGER_Task", LOGGER_TASK_STACK_SIZE, (void*) &LOG_SOCKET, LOGGER_TASK_PRIORITY, NULL);
    xTaskCreate(alarm_task, "ALARM_Task", ALARM_TASK_STACK_SIZE, (void*) &ALARM_SOCKET, ALARM_TASK_PRIORITY, NULL);
    xTaskCreate(webServer_task, "HTTP_Task", HTTP_TASK_STACK_SIZE, NULL, HTTP_TASK_PRIORITY, NULL);

	/* Infinite loop */
    vTaskStartScheduler();	//Start vTaskStartScheduler. Nothing else after this in main.
	while (1)
	{
		;
    }
}

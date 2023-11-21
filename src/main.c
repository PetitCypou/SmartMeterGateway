/**
 * ----------------------------------------------------------------------------------------------------
 * Includes
 * ----------------------------------------------------------------------------------------------------
 */
#include <stdio.h>
#include <hardware/uart.h>
#include <port_common.h>
#include <wizchip_conf.h>
#include <w6x00_spi.h>
#include <timer.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <smgUart/smgUart.h>
#include <smgDma/smgDma.h>
#include <ticParser/ticParser.h>
#include <ticConstructor/ticConstructor.h>
#include <smgNetwork/smgNetwork.h>

/* FreeRTOS Tasks*/
#define LOGGER_TASK_STACK_SIZE 2048
#define LOGGER_TASK_PRIORITY 10
#define BLINKER_TASK_STACK_SIZE 512
#define BLINKER_TASK_PRIORITY 5

/* FreeRTOS Task Prototypes */
void logger_task(void *argument);
void blinker_task(void *argument);

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

    xTaskCreate(logger_task, "LOGGER_Task", LOGGER_TASK_STACK_SIZE, NULL, LOGGER_TASK_PRIORITY, NULL);
    xTaskCreate(blinker_task, "BLINKER_Task", BLINKER_TASK_STACK_SIZE, NULL, BLINKER_TASK_PRIORITY, NULL);

	/* Infinite loop */
    vTaskStartScheduler();	//Start vTaskStartScheduler. Nothing else after this in main.
	while (1)
	{
		;
    }
}


/* Tasks */

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

	struct ticFrame *pTIC, TIC;
	pTIC = &TIC;

	//Ethernet destination configuration
	uint16_t ethDestPort = 5002;
	uint8_t ethDestIp[] = {
		192, 168, 11, 1
	};

	//Initialize Network communication through TCP
	smgNetwork_Init(PLL_SYS_KHZ);
	smgNetwork_Connect(ethDestIp,ethDestPort);	//Initial connection to port.

	while(1)
	{
		if (smgDma_Fetch(pUartWord) != 0) 		//Check if ready to receive from UART. If ready, pUartWord points so a word string.
		{
			switch(ticParse(pTIC,pUartWord)){
			case 2:											//If Tic values are valid and ready to send.
				ticConstruct(Frame, TIC);					//Create a network frame.
				smgNetwork_Send(Frame,ethDestIp,ethDestPort);		//Send frame.
				break;
			case 0:											//If received Tic frame was invalidated at some point.
				smgNetwork_Send("INVALID\n",ethDestIp,ethDestPort);	//Report invalid frame to server ?
				break;
			default:
				break;
			}
		}
		vTaskDelay(500);// 50ms
	}
}

/*
 * Task:  blinker_task
 * --------------------
 * Blinks an LED. (:D)
 *
 *  void
 *
 *  returns: void
 */
void blinker_task(void *argument)
{
	const uint LED_PIN = 25;

	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);
	while (1) {
		gpio_put(LED_PIN, 0);
		vTaskDelay(10000);//1000 ms
		gpio_put(LED_PIN, 1);
		vTaskDelay(5000);//500 ms
	}
}


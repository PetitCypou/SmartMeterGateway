/**
 * ----------------------------------------------------------------------------------------------------
 * Includes
 * ----------------------------------------------------------------------------------------------------
 */

#include <stdio.h>
#include <port_common.h>
#include <wizchip_conf.h>
#include <w6x00_spi.h>
#include <timer.h>

#include <aegUart/aegUart.h>
#include <aegDma/aegDma.h>
#include <ticParser/ticParser.h>
#include <ticConstructor/ticConstructor.h>
#include <aegNetwork/aegNetwork.h>


/* Clock */
#define PLL_SYS_KHZ (133 * 1000)

/**
 * ----------------------------------------------------------------------------------------------------
 * Parameters
 * ----------------------------------------------------------------------------------------------------
 */
	//UART
#define UART_ID uart0
#define BAUD_RATE 1200
#define DATA_BITS 7
#define STOP_BITS 1
#define PARITY    UART_PARITY_EVEN
#define UART_TX_PIN 0
#define UART_RX_PIN 1


	//Ethernet destination configuration
#define ethDestPort 5002
uint8_t ethDestIp[] = {
		192, 168, 11, 1
};


/**
 * ----------------------------------------------------------------------------------------------------
 * Main
 * ----------------------------------------------------------------------------------------------------
 */
int main()
{
	/* Declare & initialize program variables*/
	struct ringBuf *pRING, RING;
	struct ticFrame *pTIC, TIC;
	char pUartWord[256]="";
	char Frame[1024] = "";

	pTIC = &TIC; //BOOM
	pRING = &RING;

	//Initialize Network communication through TCP
	aegNetwork_Init(PLL_SYS_KHZ);
	aegNetwork_Connect(ethDestIp,ethDestPort);	//Initial connection to port.

	//Initialize UART Communication through DMA
	ticUart_Init(UART_ID, BAUD_RATE, DATA_BITS, STOP_BITS, PARITY, UART_TX_PIN, UART_RX_PIN);
	aegDma_Init(UART_ID,pRING);

	/* Infinite loop */
	while (1)
	{
		if (aegDma_Fetch(pUartWord) != 0) 		//Check if ready to receive from UART. If ready, pUartWord points so a word string.
		{
			switch(ticParse(pTIC,pUartWord)){
			case 2:											//If Tic values are valid and ready to send.
				ticConstruct(Frame, TIC);					//Create a network frame.
				aegNetwork_Send(Frame,ethDestIp,ethDestPort);		//Send frame.
				break;
			case 0:											//If received Tic frame was invalidated at some point.
				aegNetwork_Send("INVALID\n",ethDestIp,ethDestPort);	//Report invalid frame to server ?
				break;
			default:
				break;
			}
		}
    	//sleep_ms(5);	//TODO:Needed when debugging as to not overwhelm the server.
    	//DO IT ALL AGAIN
    }
}


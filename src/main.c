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

/**
 * ----------------------------------------------------------------------------------------------------
 * Macros
 * ----------------------------------------------------------------------------------------------------
 */

/* Clock */
#define PLL_SYS_KHZ (133 * 1000)

/* Timer */
static volatile uint16_t g_msec_cnt = 0;

/* Timer */
static void repeating_timer_callback(void)
{
    g_msec_cnt++;

    if (g_msec_cnt >= 1000 - 1)
    {
        g_msec_cnt = 0;
    }
}

/**
 * ----------------------------------------------------------------------------------------------------
 * Variables
 * ----------------------------------------------------------------------------------------------------
 */

/* Wizchip Network Configuration*/
static wiz_NetInfo g_net_info =
    {
        .mac = {0x00, 0x08, 0xDC, 0x12, 0x34, 0x56}, // MAC address
        .ip = {192, 168, 11, 2},                     // IP address
        .sn = {255, 255, 255, 0},                    // Subnet Mask
        .gw = {192, 168, 11, 1},                     // Gateway
        .dns = {8, 8, 8, 8},                         // DNS server
        .lla = {0xfe, 0x80, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00,
                0x02, 0x08, 0xdc, 0xff,
                0xfe, 0x57, 0x57, 0x25},             // Link Local Address
        .gua = {0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00},             // Global Unicast Address
        .sn6 = {0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff,
                0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00},             // IPv6 Prefix
        .gw6 = {0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00},             // Gateway IPv6 Address
        .dns6 = {0x20, 0x01, 0x48, 0x60,
                0x48, 0x60, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x88, 0x88},             // DNS6 server
        .ipmode = NETINFO_STATIC_ALL
};

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */

/* Clock */
static void set_clock_khz(void);

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

	//ETHERNET
#define ETH_PORT    5002
uint8_t ETH_IP[] = {
		192, 168, 11, 1
};

/**
 * ----------------------------------------------------------------------------------------------------
 * Main
 * ----------------------------------------------------------------------------------------------------
 */
int main()
{
	/* Initialize Wiznet Chip */
	set_clock_khz();
	stdio_init_all();

	sleep_ms(1000 * 2);

	wizchip_spi_initialize();
	wizchip_cris_initialize();
	wizchip_reset();
	wizchip_initialize();
	wizchip_check();
	wizchip_1ms_timer_initialize(repeating_timer_callback);
	network_initialize(g_net_info);

	sleep_ms(1000 * 1);

	/* Declare & init program variables*/
	struct ringBuf *pRING, RING;
	struct ticFrame *TIC, TAC;
	char pUartWord[256]="";
	char Frame[1024] = "";
	TIC = &TAC; //TOC
	pRING = &RING; //BIF

	//Initialize UART Communication through DMA
	ticUart_Init(UART_ID, BAUD_RATE, DATA_BITS, STOP_BITS, PARITY, UART_TX_PIN, UART_RX_PIN);
	aegDma_Init(UART_ID,pRING);
	char DEBUG[2048];

	/* Infinite loop */
	while (1)
	{
		if (aegDma_Fetch(pUartWord) != 0) 		//Check if ready to receive from UART. If ready, pUartWord points so a word string.
		{
			switch(ticParse(TIC,pUartWord)){
			case 2:											//If Tic values are valid and ready to send.
				ticConstruct(Frame, TAC);					//Create a network frame.
				aegNetwork_Send(Frame,ETH_IP,ETH_PORT);		//Send frame.
				break;
			case 0:											//If received Tic frame was invalidated at some point.
				aegNetwork_Send("INVALID\n",ETH_IP,ETH_PORT);	//Report invalid frame to server ?
				break;
			default:
				break;
			}
		}
    	//sleep_ms(5);	//TODO:Needed when debugging as to not overwhelm the server.
    	//DO IT ALL AGAIN
    }
}

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
/* Clock */
static void set_clock_khz(void)
{
    // set a system clock frequency in khz
    set_sys_clock_khz(PLL_SYS_KHZ, true);

    // configure the specified clock
    clock_configure(
        clk_peri,
        0,                                                // No glitchless mux
        CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS, // System PLL on AUX mux
        PLL_SYS_KHZ * 1000,                               // Input frequency
        PLL_SYS_KHZ * 1000                                // Output (must be same as no divider)
    );
}


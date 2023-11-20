/*
 * ticNetwork.c
 *
 *  Created on: 3 nov. 2023
 *      Author: cyp
 */

#include "aegNetwork.h"
#include <dhcpv4.h>
#include "socket.h"
#include "wizchip_conf.h"
#include <string.h>
#include "w6x00_spi.h"

#include <stdio.h>
#include <port_common.h>
#include <timer/timer.h>

/* Buffer */
#define ETHERNET_BUF_MAX_SIZE (1024 * 2)
/* Socket */
#define SOCKET_TCP_CLIENT 1
#define SOCKET_DHCP 6
/* Max retry count*/
#define RETRY_CNT   10000

static uint16_t any_port = 	50000;
static uint8_t sock_state[8] = {0,};

static uint8_t g_ethernet_buf[ETHERNET_BUF_MAX_SIZE] = {
    0,
}; // common buffer


/* Wizchip Network Configuration*/

static wiz_NetInfo netInfo =
    {
        .mac = {0x00, 0x08, 0xDC, 0x01, 0x01, 0x01}, // MAC address
        .ip = {192, 168, 11, 5},                     // IP address
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
        .ipmode = NETINFO_DHCP_V4//NETINFO_STATIC_ALL
};





//FUNCTION PROTOTYPES
uint8_t aegNetwork_Init(uint32_t clkKhz);
uint8_t aegNetwork_Connect(uint8_t* destIp, uint16_t destPort);
int8_t aegNetwork_Send(char* buf, uint8_t* destIp, uint16_t destPort);

/* DHCP */
static void wizchip_dhcp4_init(void);
static void wizchip_dhcp4_assign(void);
static void wizchip_dhcp4_conflict(void);

/* Clock */
static void set_clock_khz(int PLL_SYS_KHZ);

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
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
/* Clock */
static void set_clock_khz(int PLL_SYS_KHZ)
{
    // set a system clock frequency in khz
    set_sys_clock_khz(PLL_SYS_KHZ, 1);

    // configure the specified clock
    clock_configure(
        clk_peri,
        0,                                                // No glitchless mux
        CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS, // System PLL on AUX mux
        PLL_SYS_KHZ * 1000,                               // Input frequency
        PLL_SYS_KHZ * 1000                                // Output (must be same as no divider)
    );
}

/*
 * Function:  aegNetwork_Connect
 * --------------------
 * Opens a socket to a specific port to prepare for data transfer
 *
 *  destIp: Destination IP address.
 *  destPort: Destination port.
 *
 *  returns : Socket state. SOCK_OK = 1;
 */
uint8_t aegNetwork_Connect(uint8_t* destIp, uint16_t destPort){
	uint8_t status,ret;

	getsockopt(SOCKET_TCP_CLIENT,SO_STATUS,&status);
	if(SOCK_CLOSED == status){
	    ret = socket(SOCKET_TCP_CLIENT, Sn_MR_TCP4, any_port++, SOCK_IO_NONBLOCK);

	    if(ret != SOCKET_TCP_CLIENT){    /* reinitialize the socket */
	        return ret;
	    }

	}

    getsockopt(SOCKET_TCP_CLIENT,SO_STATUS,&status);
    if(SOCK_INIT == status){
		ret = connect(SOCKET_TCP_CLIENT, destIp, destPort, 4); /* Try to connect to TCP server(Socket, DestIP, DestPort) */

	}
    return ret;
}

/*
 * Function:  aegNetwork_Init
 * --------------------
 * La fonction Init initialise la puce W6100 pour une communication ethernet.
 *
 *  clkKhz: Cadence de l'horloge en kHz.
 *  netInfo: Configuration réseau à appliquer.
 */
uint8_t aegNetwork_Init(uint32_t clkKhz){
	uint8_t retval;
	uint8_t dhcpMaxTry = 100;
	uint8_t dhcpTries = 0;
	uint8_t g_dhcp_get_ip_flag = 0;
	/* Initialize Wiznet Chip */
	set_clock_khz(clkKhz);
	stdio_init_all();

	sleep_ms(1000 * 2);

	wizchip_spi_initialize();
	wizchip_cris_initialize();
	wizchip_reset();
	wizchip_initialize();
	wizchip_check();
	wizchip_1ms_timer_initialize(repeating_timer_callback);
	network_initialize(netInfo);

	if (netInfo.ipmode & NETINFO_DHCP_V4) // DHCP
	{
	wizchip_dhcp4_init();
	}
    //retval = DHCPv4_run();

    if (netInfo.ipmode & NETINFO_DHCP_V4)
       {
    	while(g_dhcp_get_ip_flag == 0 && dhcpTries<=dhcpMaxTry){
           retval = DHCPv4_run();

           if (retval == DHCP_IPV4_LEASED)
           {
               if (g_dhcp_get_ip_flag == 0)
               {
                   printf(" DHCP success\n");

                   g_dhcp_get_ip_flag = 1;
               }
           }
           else if (retval == DHCPV4_FAILED)
           {
               g_dhcp_get_ip_flag = 0;
               dhcpTries++;

               if (dhcpTries <= dhcpMaxTry)
               {
                   printf(" DHCP timeout occurred and retry %d\n", dhcpTries);
               }
           }

           if (dhcpTries > dhcpMaxTry)
           {
               printf(" DHCP failed\n");

               DHCPv4_stop();

               //while (1)
               //    ;
           }

           if(g_dhcp_get_ip_flag == 0)
           {
               wizchip_delay_ms(1000); // wait for 1 second
           }
    	}
	}

    if(g_dhcp_get_ip_flag == 1)
    {
       return 1;
    }
    else
    {
       return 0;
    }
}



/*
 * Function:  aegNetwork_Send
 * --------------------
 * La fonction Send envoye une chaîne de caractères à un serveur TCP particulier.
 *
 *  buf: Pointeur vers la chaîne à transmettre.
 *  destIp: Adresse IP du serveur.
 *  destPort: Port d'accès au serveur.
 *
 *  returns: L'état de la socket TCP.
 *  		1 : La socket est ouverte et fonctionnelle.
 */
int8_t aegNetwork_Send(char* buf, uint8_t* destIp, uint16_t destPort)
{

    int32_t ret; // return value for SOCK_ERRORs
    datasize_t sentsize=0;
    uint8_t status,inter,addr_len;
    datasize_t received_size;
    uint8_t tmp = 0;
    uint8_t arg_tmp8;
    wiz_IPAddress destinfo;


    // Socket Status Transitions
    // Check the W6100 Socket n status register (Sn_SR, The 'Sn_SR' controlled by Sn_CR command or Packet send/recv status)
    getsockopt(SOCKET_TCP_CLIENT,SO_STATUS,&status);
    switch(status)
    {
    case SOCK_ESTABLISHED :
        ctlsocket(SOCKET_TCP_CLIENT,CS_GET_INTERRUPT,&inter);
        if(inter & Sn_IR_CON)	// Socket n interrupt register mask; TCP CON interrupt = connection with peer is successful
        {
            arg_tmp8 = Sn_IR_CON;
            ctlsocket(SOCKET_TCP_CLIENT,CS_CLR_INTERRUPT,&arg_tmp8);// this interrupt should be write the bit cleared to '1'
        }

        //////////////////////////////////////////////////////////////////////////////////////////////
        // Data Transaction Parts; Handle the [data receive and send] process
        //////////////////////////////////////////////////////////////////////////////////////////////
        getsockopt(SOCKET_TCP_CLIENT, SO_RECVBUF, &received_size);

        received_size = strlen(buf);
            // Data sentsize control
            while(received_size != sentsize)
            {
                ret = send(SOCKET_TCP_CLIENT, buf+sentsize, received_size-sentsize); // Data send process (User's buffer -> Destination through H/W Tx socket buffer)
                if(ret < 0) // Send Error occurred (sent data length < 0)
                {
                    close(SOCKET_TCP_CLIENT); // socket close
                    return ret;
                }
                sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            }

        //////////////////////////////////////////////////////////////////////////////////////////////
        break;

    case SOCK_CLOSE_WAIT :
        getsockopt(SOCKET_TCP_CLIENT, SO_RECVBUF, &received_size);

        if((received_size = getSn_RX_RSR(SOCKET_TCP_CLIENT)) > 0) // Sn_RX_RSR: Socket n Received Size Register, Receiving data length
        {
            if(received_size > DATA_BUF_SIZE) received_size = DATA_BUF_SIZE; // DATA_BUF_SIZE means user defined buffer size (array)
            ret = recv(SOCKET_TCP_CLIENT, buf, received_size); // Data Receive process (H/W Rx socket buffer -> User's buffer)

            if(ret <= 0) return ret; // If the received data length <= 0, receive failed and process end
            received_size = (uint16_t) ret;
            sentsize = 0;

            // Data sentsize control
            while(received_size != sentsize)
            {
                ret = send(SOCKET_TCP_CLIENT, buf+sentsize, received_size-sentsize); // Data send process (User's buffer -> Destination through H/W Tx socket buffer)
                if(ret < 0) // Send Error occurred (sent data length < 0)
                {
                    close(SOCKET_TCP_CLIENT); // socket close
                    return ret;
                }
                sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            }
        }
        if((ret=disconnect(SOCKET_TCP_CLIENT)) != SOCK_OK) return ret;
        break;

    case SOCK_INIT :

        ret = connect(SOCKET_TCP_CLIENT, destIp, destPort, 4); /* Try to connect to TCP server(Socket, DestIP, DestPort) */

        if( ret != SOCK_OK) return ret;	//	Try to TCP connect to the TCP server (destination)
        break;

    case SOCK_CLOSED:

        tmp = socket(SOCKET_TCP_CLIENT, Sn_MR_TCP4, any_port++, SOCK_IO_NONBLOCK);


        if(tmp != SOCKET_TCP_CLIENT){    /* reinitialize the socket */
            return SOCKERR_SOCKNUM;
        }
        sock_state[SOCKET_TCP_CLIENT] = 1;

        break;
    default:
        break;
    }
    return SOCK_OK;
}

/* DHCP */
static void wizchip_dhcp4_init(void)
{
    printf(" DHCP client running\n");

    DHCPv4_init(SOCKET_DHCP, g_ethernet_buf);

    reg_dhcpv4_cbfunc(wizchip_dhcp4_assign, wizchip_dhcp4_assign, wizchip_dhcp4_conflict);
}

static void wizchip_dhcp4_assign(void)
{
    getIPfromDHCPv4(netInfo.ip);
    getGWfromDHCPv4(netInfo.gw);
    getSNfromDHCPv4(netInfo.sn);
    getDNSfromDHCPv4(netInfo.dns);

    /* Network initialize */
    network_initialize(netInfo); // apply from DHCP

    print_network_information(netInfo);
    printf(" DHCP leased time : %ld seconds\n", getDHCPv4Leasetime());
}

static void wizchip_dhcp4_conflict(void)
{
    printf(" Conflict IP from DHCP\n");

    // halt or reset or any...
    while (1)
        ; // this example is halt.
}

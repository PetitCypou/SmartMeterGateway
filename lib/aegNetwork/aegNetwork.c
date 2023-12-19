/*
 * ticNetwork.c
 *
 *  Created on: 3 nov. 2023
 *      Author: cyp
 */

#include "aegNetwork.h"

#include "socket.h"
#include "wizchip_conf.h"
#include <string.h>
#include "w6x00_spi.h"

/* Buffer */
#define ETHERNET_BUF_MAX_SIZE (1024 * 2)
/* Socket */
#define SOCKET_TCP_CLIENT 1
/* Port */
#define RETRY_CNT   10000

static uint16_t any_port = 	50000;
static uint8_t sock_state[8] = {0,};


//FUNCTION PROTOTYPES
int8_t aegNetwork_Send(char* buf, uint8_t* destIp, uint16_t destPort);

/*
 * Function:  aegNetwork_send
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

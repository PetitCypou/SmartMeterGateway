/**
 * @file	userHandler.c
 * @brief	User Control Example
 * @version 1.0
 * @date	2014/07/15
 * @par Revision
 *			2014/07/15 - 1.0 Release
 * @author	
 * \n\n @par Copyright (C) 1998 - 2014 WIZnet. All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "board.h"
#include "httpUtil.h"
#include "userHandler.h"
//#include "adcHandler.h"
#include "wizchip_conf.h"
#include "lib/smgIo/smgIo.h"
#include "hardware/gpio.h"

// Data IO Status
typedef enum
{
	Off	= 0,
	On 	= 1
} IO_Status_Type;

//uint16_t LED_pin[3]={GPIO_PIN_6, GPIO_PIN_8, GPIO_PIN_9};

// Pre-defined Get CGI functions
void make_json_netinfo(uint8_t * buf, uint16_t * len);

// Pre-defined Set CGI functions
int8_t set_diodir(uint8_t * uri);
int8_t set_diostate(uint8_t * uri);

uint8_t predefined_get_cgi_processor(uint8_t * uri_name, uint8_t * buf, uint16_t * len)
{
	uint8_t ret = 1;	// ret = 1 means 'uri_name' matched
	//uint8_t cgibuf[14] = {0, };
	int8_t cgi_dio = -1;
	int8_t cgi_ain = -1;

	if(strcmp((const char *)uri_name, "todo.cgi") == 0)
	{
		// to do
		;//make_json_todo(buf, len);
	}
	else if(strcmp((const char *)uri_name, "get_netinfo.cgi") == 0)
	{
		make_json_netinfo(buf, len);
	}
	else
	{
		if((cgi_dio < 0) && (cgi_ain < 0)) ret = 0;
	}

	return ret;
}


uint8_t predefined_set_cgi_processor(uint8_t * uri_name, uint8_t * uri, uint8_t * buf, uint16_t * len)
{
	uint8_t ret = 1;	// ret = '1' means 'uri_name' matched
	uint8_t val = 0;

	if(strcmp((const char *)uri_name, "todo.cgi") == 0)
	{
		// to do
		;//val = todo(uri);
		//*len = sprintf((char *)buf, "%d", val);
	}
	else if(strcmp((const char *)uri_name, "set_diostate.cgi") == 0)
	{
		val = set_diostate(uri);
		*len = sprintf((char *)buf, "%d", val);
	}
	else
	{
		ret = 0;
	}

	return ret;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pre-defined Get CGI functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void make_json_netinfo(uint8_t * buf, uint16_t * len)
{
	wiz_NetInfo netinfo;
	//ctlnetwork(CN_GET_NETINFO, (void*) &netinfo);
    wizchip_getnetinfo((void*) &netinfo);
	// DHCP: 1 - Static, 2 - DHCP
	*len = sprintf((char *)buf, "NetinfoCallback({\"mac\":\"%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\",\
											\"ip\":\"%d.%d.%d.%d\",\
											\"gw\":\"%d.%d.%d.%d\",\
											\"sn\":\"%d.%d.%d.%d\",\
											\"dns\":\"%d.%d.%d.%d\",\
											\"lla\":\"%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X\",\
			                                \"gua\":\"%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X\",\
											\"sn6\":\"%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X\",\
											\"gw6\":\"%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X\",\
											});",
											netinfo.mac[0], netinfo.mac[1], netinfo.mac[2], netinfo.mac[3], netinfo.mac[4], netinfo.mac[5],
											netinfo.ip[0], netinfo.ip[1], netinfo.ip[2], netinfo.ip[3],
											netinfo.gw[0], netinfo.gw[1], netinfo.gw[2], netinfo.gw[3],
											netinfo.sn[0], netinfo.sn[1], netinfo.sn[2], netinfo.sn[3],
											netinfo.dns[0], netinfo.dns[1], netinfo.dns[2], netinfo.dns[3],
											netinfo.lla[0],netinfo.lla[1],netinfo.lla[2],netinfo.lla[3],netinfo.lla[4],netinfo.lla[5],netinfo.lla[6],netinfo.lla[7],
											netinfo.lla[8],netinfo.lla[9],netinfo.lla[10],netinfo.lla[11],netinfo.lla[12],netinfo.lla[13],netinfo.lla[14],netinfo.lla[15],
											netinfo.gua[0],netinfo.lla[1],netinfo.gua[2],netinfo.gua[3],netinfo.gua[4],netinfo.gua[5],netinfo.gua[6],netinfo.gua[7],
											netinfo.gua[8],netinfo.gua[9],netinfo.gua[10],netinfo.gua[11],netinfo.gua[12],netinfo.gua[13],netinfo.gua[14],netinfo.gua[15],
											netinfo.sn6[0],netinfo.sn6[1],netinfo.sn6[2],netinfo.sn6[3],netinfo.sn6[4],netinfo.sn6[5],netinfo.sn6[6],netinfo.sn6[7],
											netinfo.sn6[8],netinfo.sn6[9],netinfo.sn6[10],netinfo.sn6[11],netinfo.sn6[12],netinfo.sn6[13],netinfo.sn6[14],netinfo.sn6[15],
											netinfo.gw6[0],netinfo.gw6[1],netinfo.gw6[2],netinfo.gw6[3],netinfo.gw6[4],netinfo.gw6[5],netinfo.gw6[6],netinfo.gw6[7],
											netinfo.gw6[8],netinfo.gw6[9],netinfo.gw6[10],netinfo.gw6[11],netinfo.gw6[12],netinfo.gw6[13],netinfo.gw6[14],netinfo.gw6[15]
											);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pre-defined Set CGI functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int8_t set_diostate(uint8_t * uri)
{
	uint8_t * param;
	uint8_t pin = 0, val = 0;

	if((param = get_http_param_value((char *)uri, "func"))) // GPIO; D0 ~ D15
	{
		pin = (uint8_t)ATOI(param, 10);
		if(25 != pin) return -1; //Protect the pico.

		if((param = get_http_param_value((char *)uri, "val")))  // State; high(on)/low(off)
		{
			val = (uint8_t)ATOI(param, 10);
			if(val > On) val = On;
		}

		if(val == On){
			smgIoSetAlarm(1);
		}
		else{
			smgIoSetAlarm(0);
		}
	}

	return pin;
}

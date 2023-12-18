/*
 * webServer.c
 *
 *  Created on: 23 nov. 2023
 *      Author: cyp
 */


#include <FreeRTOS.h>
#include <task.h>

#include "webServer.h"
#include "hardware/gpio.h"
#include "lib/smgBoot/smgBoot.h"


/* HTTP Parameters */
#define MAX_HTTPSOCK	4
#define LED_PIN 25
#define DATA_BUF_SIZE 2048
/*
 * Task:  webServer_task
 * --------------------
 * HTTP webserver for user config
 *
 *  void
 *
 *  returns: void
 */
void webServer_task(void *argument)
{
	char bootSelect;

	bootSelect = smgGetBoot();
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);

	uint8_t socknumlist[] = {0, 1, 2, 3};
	uint8_t RX_BUF[DATA_BUF_SIZE];
	uint8_t TX_BUF[DATA_BUF_SIZE];
	uint8_t i;

	  /* HTTP Server Initialization  */
	  httpServer_init(TX_BUF, RX_BUF, MAX_HTTPSOCK, socknumlist);		// Tx/Rx buffers (1kB) / The number of W6100 chip H/W sockets in use

	  /* Web content registration (web content in webpage.h, Example web pages) */
		// Index page and netinfo / base64 image demo
		reg_httpServer_webContent((uint8_t *)"index.html", (uint8_t *)index_page);				// index.html 		: Main page example
		reg_httpServer_webContent((uint8_t *)"netinfo.html", (uint8_t *)netinfo_page);			// netinfo.html 	: Network information example page
		reg_httpServer_webContent((uint8_t *)"netinfo.js", (uint8_t *)wiz6100web_netinfo_js);	// netinfo.js 		: JavaScript for Read Network configuration 	(+ ajax.js)
		reg_httpServer_webContent((uint8_t *)"img.html", (uint8_t *)img_page);					// img.html 		: Base64 Image data example page
		reg_httpServer_webContent((uint8_t *)"firmware.js", (uint8_t *)firmware_js);			// firmware.js 		: JavaScript for firmware version. 				(+ ajax.js)
		reg_httpServer_webContent((uint8_t *)"firmware.html", (uint8_t *)firmware_page);
		reg_httpServer_webContent((uint8_t *)"fwupload.html", (uint8_t *)fwupload_page);
		reg_httpServer_webContent((uint8_t *)"fwfailed.html", (uint8_t *)fwfailed_page);

		// Example #1
		reg_httpServer_webContent((uint8_t *)"dio.html", (uint8_t *)dio_page);					// dio.html 		: Digital I/O control example page
		reg_httpServer_webContent((uint8_t *)"dio.js", (uint8_t *)wiz6100web_dio_js);			// dio.js 			: JavaScript for digital I/O control 	(+ ajax.js)

		// AJAX JavaScript functions
		reg_httpServer_webContent((uint8_t *)"ajax.js", (uint8_t *)wiz6100web_ajax_js);			// ajax.js			: JavaScript for AJAX request transfer

	while (bootSelect==smgGetBoot()) {
		vTaskDelay(100);//10 ms
		for(i = 0; i < MAX_HTTPSOCK; i++)	httpServer_run(i); 	// HTTP Server handler
	}
	smgReBoot();//If our firmware has switched, it means we used FOTA and we now need to reboot.
}

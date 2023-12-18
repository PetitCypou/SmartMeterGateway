/**
 * ----------------------------------------------------------------------------------------------------
 * Includes
 * ----------------------------------------------------------------------------------------------------
 */
#include "memory_map.h"
#include <port_common.h>
#include "pico/stdlib.h"
#include <inttypes.h>
#include "lib/smgBoot/smgBoot.h"
#define FLASH_TARGET_OFFSET (__bootConfigOffset__)	//1.5MB offset from start of flash.

__attribute__((naked)) static void startApp(uint32_t pc, uint32_t sp) ;
void loadApp (uint32_t * symbol);

__attribute__((naked)) static void startApp(uint32_t pc, uint32_t sp) {
    __asm("           \n\
          msr msp, r1 /* load r1 into MSP */\n\
          bx r0       /* branch to the address at r0 */\n\
    ");
}

void loadApp (uint32_t* fwAddress){
	uint32_t *vector_table = (uint32_t *) fwAddress;
	uint32_t *vtor = (uint32_t *)0xE000ED08;
	*vtor = ((uint32_t) vector_table & 0xFFFFFFF8);
	uint32_t *app_code = (uint32_t *) fwAddress;
	uint32_t app_sp = app_code[0];
	uint32_t app_start = app_code[1];
	startApp(app_start, app_sp);
}

/**
 * ----------------------------------------------------------------------------------------------------
 * Main
 * ----------------------------------------------------------------------------------------------------
 */
int main()
{
    //smgSetBoot('A'); //FIXME : DBG :Switch firmware just in case everything glitches out.
    const uint LED_PIN = 25;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);
    sleep_ms(250);
    gpio_put(LED_PIN, 0);
    sleep_ms(250);
    gpio_deinit(LED_PIN);
    if (smgGetBoot() == 'A'){
    	//Start app A
    	loadApp((uint32_t *) &__firmwareA__);
    }
    else{
    	//Start app B
    	loadApp((uint32_t *) &__firmwareB__);
    }
  /* Not Reached */
  while (1) {}
}


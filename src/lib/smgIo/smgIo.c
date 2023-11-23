/*
 * smgIo.c
 *
 *  Created on: 23 nov. 2023
 *      Author: cyp
 */
#include "smgIo.h"
#include <stdio.h>
#include <hardware/gpio.h>

#define LED_PIN 25

void smgIoSetAlarm(uint8_t state);
uint8_t smgIoGetAlarm(void);

uint8_t ALARM = 0; //Alarm state


void smgIo_Init(void)
{
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

	gpio_put(LED_PIN, 0);
}

void smgIoSetAlarm(uint8_t state){
	ALARM = state;

	if(ALARM == 1)
	{
		gpio_put(LED_PIN, 1);
	}
	else{
		gpio_put(LED_PIN, 0);
	}
}

uint8_t smgIoGetAlarm(void){
	return ALARM;
}

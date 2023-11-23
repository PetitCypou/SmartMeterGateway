/*
 * smgIo.h
 *
 *  Created on: 23 nov. 2023
 *      Author: cyp
 */

#ifndef SMGIO_H_
#define SMGIO_H_

#include <stdio.h>

void smgIo_Init(void);
void smgIoSetAlarm(uint8_t state);
uint8_t smgIoGetAlarm(void);

#endif /* SMGIO_H_ */

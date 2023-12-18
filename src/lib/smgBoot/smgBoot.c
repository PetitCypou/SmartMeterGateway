/*
 * smgBoot.c
 *
 *  Created on: 5 déc. 2023
 *      Author: cyp
 */

#include "smgBoot.h"
#include "memory_map.h"
#include "lib/smgNetwork/smgNetwork.h"
#include <port_common.h>
#include <hardware/flash.h>
#include <hardware/sync.h>
#include <hardware/regs/addressmap.h>
#include <lib/sha1/sha1.h>
#include <string.h>
#include <stdlib.h>
#include "hardware/gpio.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lib/sha1/sha1.h"

#define AIRCR_Register (*((volatile uint32_t*)(PPB_BASE + 0x0ED0C)))

#define BOOT_CONFIG_TARGET_OFFSET ((uint32_t)&__bootConfig__- XIP_BASE)	//FIXME : UGLY : NEED TO POINT TO MEMORY SECTION.
#define FWA_TARGET_OFFSET ((uint32_t)&__firmwareA__- XIP_BASE)	//FIXME : UGLY : NEED TO POINT TO MEMORY SECTION.
#define FWB_TARGET_OFFSET ((uint32_t)&__firmwareB__- XIP_BASE)	//FIXME : UGLY : NEED TO POINT TO MEMORY SECTION.

//FUNCTION PROTOTYPES
void smgBoot_Save(void);
void smgBoot_Load(void);
char smgGetBoot(void);
void smgSetBoot(char FW);
uint8_t smgSwitchBoot(char SHA[41],uint32_t fwLength);
void smgReBoot(void);
void smgStoreFirmware(uint8_t buf[4096],uint16_t writeSize,uint32_t offset);
long unsigned int roundUpMult(uint32_t val, uint32_t multiple);

struct smgBoot currentConf;
const struct smgBoot defaultConf = {
	.FW_SEL = 'A'
};

void smgBoot_Save(void){
	struct smgBoot checkConf; //Config loaded from memory after saving to check integrity.
    void* newBytes = (void*) &currentConf;

    uint32_t ints = save_and_disable_interrupts();	//Disable interrupts
    flash_range_erase(BOOT_CONFIG_TARGET_OFFSET, (size_t) &__sizeBootConfig__); //Erase flash config.
    flash_range_program(BOOT_CONFIG_TARGET_OFFSET, newBytes, sizeof(newBytes));
    restore_interrupts (ints);

    if (memcmp(&__bootConfig__,newBytes,sizeof(struct smgBoot))!=0){
    	//Try to go back to default conf.
        newBytes = (uint8_t*) &defaultConf;

        uint32_t intss = save_and_disable_interrupts();	//Disable interrupts
        flash_range_erase(BOOT_CONFIG_TARGET_OFFSET, (size_t) &__sizeBootConfig__); //Erase flash config.
        flash_range_program(BOOT_CONFIG_TARGET_OFFSET, newBytes, sizeof(newBytes));
        restore_interrupts (intss);
    }
    else{
        //printf("Programming successful!\n");
    }
}

void smgBoot_Load(void){
	 //TODO : ADD CONFIG CHECKSUM : check if we are about to load a corrupt configuration. If so, revert back to defaultConf
    memcpy(&currentConf, &__bootConfig__, sizeof(struct smgBoot));	//Copy the struct back into RAM.
}

char smgGetBoot(void){
	smgBoot_Load();
	return currentConf.FW_SEL;
}

void smgSetBoot(char FW){
	currentConf.FW_SEL = FW;
	smgBoot_Save();
}

void smgReBoot(void){
    smgNetwork_DeInit();
	AIRCR_Register = 0x5FA0004; //Reboot Pico
}

uint8_t smgSwitchBoot(char SHA[41],uint32_t fwLength){
	  char result[21];
	  char hexresult[41];
	  size_t offset;

    if(smgGetBoot()=='A')
    {

  	  SHA1( result, (void*)&__firmwareB__, fwLength ); /* calculate hash */
  	  for( offset = 0; offset < 20; offset++) {/* format the hash for comparison */
  	    sprintf( ( hexresult + (2*offset)), "%02x", result[offset]&0xff);
  	  }
    	//CHECK SHA
    	if(memcmp(SHA,hexresult,40)==0)
    	{
    	currentConf.FW_SEL = 'B';
		memcpy(currentConf.SHA_B, SHA, 41);
		memcpy((uint32_t*) &currentConf.LEN_B,(uint32_t*) &fwLength, sizeof(fwLength));
    	smgBoot_Save();
    	return 1;
    	}
    	else{
    		//SOFTWARE CORRUPTED
    		return 0;
    	}
    }
    else
    {
    	  SHA1( result, (void*)&__firmwareA__, fwLength ); /* calculate hash */
    	  for( offset = 0; offset < 20; offset++) {/* format the hash for comparison */
    	    sprintf( ( hexresult + (2*offset)), "%02x", result[offset]&0xff);
    	  }
      	//CHECK SHA
      	if(memcmp(SHA,hexresult,40)==0)
      	{
      	currentConf.FW_SEL = 'A';
  		memcpy(currentConf.SHA_A, SHA, 41);
  		memcpy((uint32_t*) &currentConf.LEN_A,(uint32_t*) &fwLength, sizeof(fwLength));
      	smgBoot_Save();
      	return 1;
      	}
      	else{
      		//SOFTWARE CORRUPTED
      		return 0;
      	}
    }
}


void smgStoreFirmware(uint8_t* buf,uint16_t writeSize,uint32_t offset)//FIXME buf has no size. Really bad.
{
	uint32_t ints = save_and_disable_interrupts();	//Disable interrupts
	static uint8_t pad_buf[FLASH_PAGE_SIZE*2]; //Somewhere to store tail end of buffer that isn't long enough to write into flash.
	uint32_t alignment = offset%FLASH_PAGE_SIZE; //How far away are we to the last multiple of 256 in our alignment.

	static void* pOFFSET;
	static void* pADDRESS;
	static void* pSIZE;

	if(offset == 0) //Starting to transfer a FW.
	{
		if(smgGetBoot()=='A') //Try not to overwrite your own code haha.
		{
			pOFFSET = FWB_TARGET_OFFSET;
			pADDRESS = &__firmwareB__;
			pSIZE = &__sizeFirmwareB__;
		}
		else{
			pOFFSET = FWA_TARGET_OFFSET;
			pADDRESS = &__firmwareA__;
			pSIZE = &__sizeFirmwareA__;
		}

		flash_range_erase(pOFFSET, roundUpMult((uint32_t)pSIZE,4096)); //Erase flash config.
		flash_range_program(pOFFSET,
				buf,
				roundDownMult(writeSize, FLASH_PAGE_SIZE));
		//Store the remaining data in the pad_buf.
		memcpy(pad_buf,buf+roundDownMult(writeSize,FLASH_PAGE_SIZE),writeSize%FLASH_PAGE_SIZE);
	}
	else if(writeSize !=0){
	//Use the remaining data that is in the pad_buf.
	memcpy(pad_buf+alignment,buf,FLASH_PAGE_SIZE-alignment);
	flash_range_program(roundDownMult(pOFFSET+offset,FLASH_PAGE_SIZE),
			pad_buf,
			FLASH_PAGE_SIZE);//Which should also be the size of pad_buf.
	flash_range_program(roundUpMult(pOFFSET+offset,FLASH_PAGE_SIZE),
			buf+(FLASH_PAGE_SIZE-alignment),
			roundDownMult(writeSize-(FLASH_PAGE_SIZE-alignment), FLASH_PAGE_SIZE));//Maybe I'm pronting a bit too much data.

	//Store the remaining data in the pad_buf.
	int Cursor = (FLASH_PAGE_SIZE-alignment)+roundDownMult(writeSize-(FLASH_PAGE_SIZE-alignment), FLASH_PAGE_SIZE);
	memset(pad_buf,255,sizeof(pad_buf));
	memcpy(pad_buf,
		buf+Cursor,
		writeSize-Cursor
			);
	}
	else {
		//Use the data that is in the pad_buf.

		flash_range_program(roundDownMult(pOFFSET+offset,FLASH_PAGE_SIZE),
				pad_buf,
				FLASH_PAGE_SIZE);//Which should also be the size of pad_buf.
		memset(pad_buf,255,sizeof(pad_buf));
	}
	restore_interrupts(ints);
	sleep_ms(10);
}
long unsigned int roundUpMult(uint32_t val, uint32_t multiple)
{
	if(0 != val%multiple){
		return val + (multiple-(val%multiple));
	}
	else{
		return val;
	}
}

long unsigned int roundDownMult(uint32_t val, uint32_t multiple)
{
	return val -(val%multiple);
}

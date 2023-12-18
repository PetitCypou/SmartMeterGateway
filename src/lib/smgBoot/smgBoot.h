#ifndef SMGBOOT_H_
#define SMGBOOT_H_

#include <stdio.h>

//STRUCTURE CONFIG
struct smgBoot {
	char FW_SEL;
	uint8_t SHA_A[41];
	uint32_t LEN_A;
	uint8_t SHA_B[41];
	uint32_t LEN_B;
};


void smgBoot_Save(void);
void smgBoot_Load(void);
char smgGetBoot(void);
void smgSetBoot(char FW);
uint8_t smgSwitchBoot(char SHA[41],uint32_t fwLength);
void smgReBoot(void);
void smgStoreFirmware(uint8_t buf[4096],uint16_t writeSize,uint32_t offset);;
long unsigned int roundUpMult(uint32_t val, uint32_t multiple);
long unsigned int roundDownMult(uint32_t val, uint32_t multiple);

#endif /* SMGBOOT_H_ */

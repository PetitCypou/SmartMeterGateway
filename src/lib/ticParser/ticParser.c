
/*
 * ticParser.c
 *
 *  Created on: 3 nov. 2023
 *      Author: cyp
 */
#define NULL 0
#include "ticParser.h"
#include <string.h>
#include <stdlib.h>

#include "lib/smgIo/smgIo.h"

//FUNCTION PROTOTYPES
uint8_t ticParse(struct ticFrame* TIC,char* pUartWord);
uint8_t checkSum(char* word);
/*
 * Function:  ticParse
 * --------------------
 *  La fonction ticParse observe un mot de trame TIC (0x0A + x*caractères + NULL),
 *  ou un caractère spécial (caractère spécial + NULL).
 *	Elle l'analyse, vérifie son intégrité et édite une structure d'information ticFrame pour
 *	mettre à jour les valeurs selon les données reçues.
 *
 *  TIC: Structure de données ticFrame à éditer.
 *  pUartWord: Pointeur vers un buffer contenant le mot à analyser.
 *
 *  returns: 0 si la trame est jetée à la poubelle à cause d'un checksum invalide ou un caractère inconnu.
 *  		 1 si le mot ou caractère est compris mais que la trame n'est pas terminée.
 *  		 2 si le mot ou caractère est compris et que toute la trame a correctement été enregistrée.
 */
uint8_t ticParse(struct ticFrame* TIC,char* pUartWord){

	//TODO : CHECK FOR SPECIAL CHARACTERS
	char* toul;
	static uint8_t frameHasStarted = 0;
	static uint8_t foundAlarm = 0;

	if(0x0A==pUartWord[0])//If the first character is a line feed, ignore it.
	{
		pUartWord++;
	}

	//SPECIAL CHARACTERS
	if(0x00==pUartWord[0]){
		//Do nothing, empty string.
	}
	else if(0x0D==pUartWord[0]){
		//Do nothing
	}
	else if(0x02==pUartWord[0])				//Frame has begun !
	{
		foundAlarm = 0;
		frameHasStarted = 1;
	}
	else if(0x03==pUartWord[0])				//Frame has ended !
	{
		if(frameHasStarted){ 		//If we were here from the beginning.
			if(foundAlarm == 0){
				TIC->ADPS = 0;
				smgIoSetAlarm(0);
			}
			frameHasStarted = 0;
			return 2;				//Frame is valid
		}
	}
	else if(checkSum(pUartWord)){ 		//REGISTER CHECKSUM:

		if(strstr(pUartWord,"ADCO") != NULL){
			TIC->ADCO = strtoull((pUartWord+5),&toul,10);
		}
		else if(strstr(pUartWord,"OPTARIF") != NULL){
			if(strstr(pUartWord+8,"BASE") != NULL) TIC->OPTARIF = BASE;
			if(strstr(pUartWord+8,"HC..") != NULL) TIC->OPTARIF = HCxx;
			if(strstr(pUartWord+8,"EJP.") != NULL) TIC->OPTARIF = EJPx;
			if(strstr(pUartWord+8,"BBR") != NULL)  TIC->OPTARIF = BBRx;
		}
		else if(strstr(pUartWord,"ISOUSC") != NULL){
			TIC->ISOUSC = atoi((pUartWord+7));
		}
		else if(strstr(pUartWord,"BASE") != NULL){
			TIC->BASE = strtoull((pUartWord+5),&toul,10);
		}
		else if(strstr(pUartWord,"PTEC") != NULL){
			if(strstr(pUartWord+5,"TH..") != NULL) TIC->PTEC = TH;
			if(strstr(pUartWord+5,"HC..") != NULL) TIC->PTEC = HC;
			if(strstr(pUartWord+5,"HP..") != NULL) TIC->PTEC = HP;
			if(strstr(pUartWord+5,"HN..") != NULL)  TIC->PTEC = HN;
			if(strstr(pUartWord+5,"PM..") != NULL)  TIC->PTEC = PM;
			if(strstr(pUartWord+5,"HCJB") != NULL)  TIC->PTEC = HCJB;
			if(strstr(pUartWord+5,"HPJB") != NULL)  TIC->PTEC = HPJB;
			if(strstr(pUartWord+5,"HCJW") != NULL)  TIC->PTEC = HCJW;
			if(strstr(pUartWord+5,"HPJW") != NULL)  TIC->PTEC = HPJW;
			if(strstr(pUartWord+5,"HCJR") != NULL)  TIC->PTEC = HCJR;
			if(strstr(pUartWord+5,"HPJR") != NULL)  TIC->PTEC = HPJR;
		}
		else if(strstr(pUartWord,"IINST") != NULL){
			TIC->IINST = atoi((pUartWord+6));
		}
		else if(strstr(pUartWord,"IMAX") != NULL){
			TIC->IMAX = atoi((pUartWord+5));
		}
		else if(strstr(pUartWord,"PAPP") != NULL){
			TIC->PAPP = atoi((pUartWord+5));
		}
		else if(strstr(pUartWord,"MOTDETAT") != NULL){
			memcpy(TIC->MOTDETAT,pUartWord+9,6);
			memset(TIC->MOTDETAT+6,0,1); //String null termination.
		}
		else if(strstr(pUartWord,"ADPS") != NULL){
			TIC->ADPS = atoi((pUartWord+5));
			smgIoSetAlarm(1);
			foundAlarm = 1;
		}

	}
	else {
		frameHasStarted = 0; //Invalidate frame because we received a corrupted register or command.
		return 0;
	}
	return 1;
}


/*
 * Function:  checkSum
 * --------------------
 *  La fonction checkSum observe un mot de trame TIC (x*caractères + NULL).
 *	Elle vérifie l'intégrité du mot et retourne un indicateur de celle-ci.

 *  pUartWord: Pointeur vers un buffer contenant le mot à analyser.
 *
 *  returns: 0 si le mot est invalide ou corrompu.
 *  		 1 si le mot est valide.
 */
uint8_t checkSum(char* pUartWord){

	uint8_t sum = 0;
	uint8_t len = strlen(pUartWord);

	for (int i = 0;i < len-2; i++)
	{
		sum += (uint8_t) pUartWord[i];	//Somme de tous les charactères.
	}

	sum = (sum & 0b00111111) + 0x20; 	//Opération de fermeture pour assurer que le charactère n'est pas un spécial.

	if (sum == pUartWord[len-1])		//Si le calcul = valeur du charactère de checksum, OK.
	{
		return 1;
	}
	else{
		return 0;
	}
}

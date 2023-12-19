/*
 * ticParser.h
 *
 *  Created on: 3 nov. 2023
 *      Author: cyp
 */

#ifndef TICPARSER_H_
#define TICPARSER_H_
#include <stdint.h>

//ENUMERATIONS
enum eOPTARIF{
	BASE,
	HCxx,
	EJPx,
	BBRx
};


enum ePTEC{
	TH,
	HC,
	HP,
	HN,
	PM,
	HCJB,
	HPJB,
	HCJW,
	HPJW,
	HCJR,
	HPJR
};

enum eDEMAIN{
	NONE,
	BLEU,
	BLANC,
	ROUGE
};

//STRUCTURE TIC
struct ticFrame {
	unsigned long long ADCO; 		//Adresse compteur	FIXME:Pourrait être falsifiée en HW ?
	enum eOPTARIF OPTARIF; 			//Option tarifaire
	uint8_t  ISOUSC;				//Intensité sosucrite
	unsigned long long BASE;		//Indice Base			(Optionnel)
	uint8_t PEJP;					//Préavis effacement jour de pointe (30 min)
	enum ePTEC PTEC;				//Période tarifaire en cours
	enum eDEMAIN DEMAIN;			//Couleur de la période tarifaire du lendemain
	uint8_t IINST;					//Intensité instantanée
	uint8_t ADPS;					//Avertissement de dépassement de puissance souscrite
	uint8_t IMAX;					//Intensité maximale
	uint32_t PAPP;					//Puissance apparente
	char HHPHC;						//Code horaire Heures pleines - Heures creuses
	char MOTDETAT[6];				//Mot d'état compteur
};

//FUNCTION PROTOTYPES
void on_uart_rx();
void ticParser_init();
uint8_t ticCheck(uint8_t* word, uint8_t len);
uint8_t ticParse(struct ticFrame* TIC,char* pUartWord);

#endif /* TICPARSER_H_ */

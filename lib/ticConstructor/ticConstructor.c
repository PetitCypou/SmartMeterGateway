/*
 * ticConstructor.c
 *  Created on: 3 nov. 2023
 *      Author: cyp
 */

#include "ticConstructor.h"
#include <stdio.h>
#include "ticParser/ticParser.h"

//FUNCTION PROTOTYPES
void ticConstruct(char* frame, struct ticFrame TAC);

void ticConstruct(char* frame, struct ticFrame TAC){

	const char* aOPTARIF[] = {"BASE","HC","EJP.","BBRx"};
	const char* aPTEC[] = {"TH","HC","HP","HN","PM","HCJB","HPJB","HCJW","HPJW","HCJR","HPJR"};

	snprintf(frame,1024,"%llu;%s;%d;%llu;%s;%d;%d;%d;%s\n",
			TAC.ADCO,
			aOPTARIF[TAC.OPTARIF],
			TAC.ISOUSC,
			TAC.BASE,
			aPTEC[TAC.PTEC],
			TAC.IINST,
			TAC.IMAX,
			TAC.PAPP,
			TAC.MOTDETAT);
}

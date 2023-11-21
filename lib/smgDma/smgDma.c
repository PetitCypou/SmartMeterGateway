/*
 * smgDma.c
 *
 *  Created on: 14 nov. 2023
 *      Author: cyp
 */

#include "smgDma.h"
#include "hardware/uart.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "string.h"
#include "stdio.h"

struct ringBuf* RING;
int data_chan;

//FUNCTION PROTOTYPES
void dma_handler();
void smgDma_Init(uart_inst_t* UART_ID, struct ringBuf* pRING);


/*
 * Function:  dma_handler
 * --------------------
 * Handler de l'interruption du canal DMA initialisé
 *
 *  void
 *
 *  returns: void
 */
void dma_handler() {

    dma_hw->ints0 = 1u << data_chan;								// Clear the interrupt request.
    dma_channel_set_trans_count(data_chan,255,false);	    		// Tell the channel to read 255 new values.
    dma_channel_set_write_addr(data_chan, &RING->buf[0], true);	    // Reset the channel write address, and retrigger it.
}

/*
 * Function:  smgDma_Init
 * --------------------
 * La fonction Init initialise un canal DMA pour qu'il écrive le contenu d'un canal uart donné dans un buffer circulaire donné.
 *
 *  UART ID: Canal UART
 *  pRING: Pointeur vers un buffer circulaire
 *
 *  returns: void
 */
void smgDma_Init(uart_inst_t* UART_ID, struct ringBuf* pRING){
	RING = pRING;										//On se crée un pointeur local du buffer circulaire pour pouvoir l'éditer.
	RING->pRead = &RING->buf[0];						//On initialise le pointeur de lecture.

	data_chan = dma_claim_unused_channel(true);			//Récupérer le premier canal DMA libre, puis le paramétrer :

	dma_channel_config d = dma_channel_get_default_config(data_chan);
	channel_config_set_transfer_data_size(&d, DMA_SIZE_8);
	channel_config_set_read_increment(&d, false);
	channel_config_set_write_increment(&d, true);
	channel_config_set_dreq(&d,uart_get_dreq(UART_ID,0));

	dma_channel_configure(
			data_chan,
			&d,
			RING->buf,
			&uart_get_hw(UART_ID)->dr,
			255,
			false
			);

	RING->pWrite = &dma_hw->ch[data_chan].write_addr;	//On initialise le pointe d'écriture pour le lier à la tête d'écriture du DMA.

    dma_channel_set_irq0_enabled(data_chan, true);		//Relier la DMA à une interruption
    irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    irq_set_enabled(DMA_IRQ_0, true);

    dma_start_channel_mask(1u << data_chan) ;			//Lancer le processus
}



/*
 * Function:  smgDma_Fetch
 * --------------------
 * La fonction Fetch détecte les mots présents dans un buffer circulaire à partir des caractères spéciaux.
 * Il déclenche dès qu'il vois un caractère spécial (<=0x0D) et fournis tous les caractères précédents ainsi que le spécial.
 * Puis les mets en forme et édite un buffer.
 *
 *  patchBuf: Pointeur vers un buffer
 *
 *  returns: 0 si il n'y a pas de mot complet
 *  		 1 si il y a un mot complet qui n'est pas scindé
 *  		 2 si il y a un mot complet qui est scindé par le buffer circulaire et a dû être reconstruit.
 */
uint8_t smgDma_Fetch(char* patchBuf){ //Return the type of info, and edits the string

	char* a = RING->pRead;			//Adresse de recherche. (TAIL -> HEAD)
	uint8_t overFlow = 0;			//Indicateur d'Overflow : Le mot qu'on cherche est scindé par le buffer.

	while ((int) a != *RING->pWrite){											//Tant qu'on a pas rattrapé la tête,

		if(0x0D>=*a && a != RING->pRead){										//Si on vois un nouveau charactère spécial,

			if(overFlow == 0){
																				//TODO:Point directly to the buffer? Give length as well ? no null string termination.
				uint8_t len = a-RING->pRead; 									//Si l'info est disponible en un seul bloc dans le ringBuffer,
				memcpy(patchBuf,RING->pRead,len);								//Remplir le buffer et ajouter un null terminator.
				memset(patchBuf+len,0,1);
			}
			else{
				uint32_t startAdd = (int) &RING->buf[0];
				uint32_t stopAdd = (int) &RING->buf[sizeof(RING->buf)-1];
				uint32_t len1 = (int)stopAdd-(int)RING->pRead;					//Longueur du premier bloc, de la queue au bout du buffer.
				uint32_t len2 = (int)a - startAdd;								//Longueur du second bloc, du début du buffer à la fin du mot.

				memcpy(patchBuf,RING->pRead, len1);								//Remplir la première partie, de la queue au bout du buffer.
				memcpy(patchBuf+len1,&RING->buf[0],len2);
				memset(patchBuf+len1+len2,0,1);
			}

			RING->pRead = a; 													//Mettre à jour le pointeur de lecture pour la prochaine fois.
			return (1+overFlow);//1+Overflow
		}
		if (a == &(RING->buf[sizeof(RING->buf)-2])) 						//Si l'on est à la fin du buffer
		{
			overFlow = 1;													//On indique que le mot qu'on cherche est scindé.
			a = &RING->buf[0];												//On reviens au début du buffer circulaire.
		}else
		{
			a++;
		}

	}
	return 0;

}


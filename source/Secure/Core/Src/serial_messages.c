/*
 * serial_messages.c
 *
 *  Created on: Dec 7, 2022
 *      Author: aj4775
 */
#include "serial_messages.h"
#include <string.h>

#define ACTIVE 1
#define DEACTIVE 0

#define STATUS DEACTIVE


void smessage(messages ID){
	if(STATUS){
		char message[60];
		switch(ID){
			case START:
				strcpy(message,"\nAttestation Protocol Starting.\n");
				break;
			case IFORMATION_1:
				strcpy(message,"Waiting for verifier request ...\n");
				break;
			case START_APPLICATION:
				strcpy(message,"\nApplication is starting.\n");
				break;
			case NEW_LINE:
				strcpy(message,"\n\n");
				break;
			case REPORT:
				strcpy(message,"\nReport:\n\n");
				break;
			default:
				strcpy(message,"empty\n");
		}
		SecureUartTx((uint8_t*)message,strlen(message));
	}
	return;
}

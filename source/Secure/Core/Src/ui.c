/*
 * ui.c
 *
 *  Created on: Feb 23, 2023
 *      Author: nneto
 */

#include <string.h>
#include "cfa_communication.h"
#include "ui.h"
// global vars
short flags;
char message_vector[50];

void ui_send_message(char *msg, int size){
	com_SecureUartTx((uint8_t*) msg, size, NULL, 0);
	return;
}

int ui_read_answer(char * answr){
	if (com_SecureUartRx((uint8_t*) answr, 1,NULL, 0))
		return 1;
	return 0;
}

void ui_print(uint8_t menu){
	if (USE_TOKEN_FLAG){SEND_MESSAGE("|PT");}
	SEND_MESSAGE("SELECT AN OPTION:\n");
	switch(menu){
	case MENU_ID_MAIN:
		SEND_MESSAGE("\tC\t-> Continue\n");
		SEND_MESSAGE("\tS\t-> Settings\n");
		break;

	case MENU_ID_SETTINGS:
		break;

	case MENU_ID_1:
		SEND_MESSAGE("\tC\t-> Run Attestation\n");
		break;

	case MENU_ID_2:
		SEND_MESSAGE("\nINSTRUMENTATION METHOD : \n");
		SEND_MESSAGE("\tC\t-> C-FLAT (NOT WORKING)\n");
		SEND_MESSAGE("\tI\t-> ISC-FLAT (NOT WORKING)\n");
		SEND_MESSAGE("\tO\t-> OAT (NOT WORKING)\n");
		break;

	default:
		break;
	}
	SEND_MESSAGE("\tR\t-> Return/Exit\n\n");
	if (USE_TOKEN_FLAG) {SEND_MESSAGE("|EX");}
}

int ui_settings_menu(){
	PRINT_AND_READ_ANWSER(MENU_ID_SETTINGS);
	switch(answr){

	// Code Here

	END_CASES;
	}
	return 1;
}



int ui_menu_2(){
	PRINT_AND_READ_ANWSER(MENU_ID_2);
	switch(answr){

	case 'I':
		return 1;

	case 'C':
		return 1;

	case 'O':
		return 1;

	// Code Here

	END_CASES;
	}
	return 1;
}


int ui_menu_1(){
	PRINT_AND_READ_ANWSER(MENU_ID_1);
	switch(answr){

	case 'C':
		while(ui_menu_2()){} // Attestation with Iscflat
		return 1;

	// Code Here

	END_CASES;
	}
	return 1;
}

int ui_main_menu(){
	PRINT_AND_READ_ANWSER(MENU_ID_MAIN);
	switch(answr){

	// Options Menu
	case 'C':
		while(ui_menu_1()){}
		return 1;

	// Setting Menu
	case 'S':
		while(ui_settings_menu()){}
		return 1;

	END_CASES;
	}
}

void ui_start(){
	CLEAR_USE_TOKEN_FLAG();
	while(ui_main_menu()){}
	return;
}




/*
 * ISC-FLAT RELATED MENU
 *
 */

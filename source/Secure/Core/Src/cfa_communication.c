/*
 * cfa_communication.c
 *
 *  Created on: Feb 2, 2023
 *      Author: nneto
 */


/*
 * cfa_communication.c
 *
 *  Created on: Feb 1, 2023
 *      Author: nneto
 */

#include "cfa_communication.h"

extern UART_HandleTypeDef hlpuart1;
typedef uint8_t CFA_StatusTypeDef;


int com_init_(){
	int a =1;
	a = 2+1;
	return a;
}

CFA_StatusTypeDef com_wait_for_command(char pattern[],int size){
	int found_size = 0;
	int i = 0;
	char read_char;
	HAL_StatusTypeDef hal_status;

	while(found_size != size){

		hal_status = HAL_UART_Receive(&hlpuart1, (uint8_t*)&read_char, 1, TIMEOUT_RX);

		if (hal_status == HAL_TIMEOUT){
			return ERROR_RX_TIMEOUT;
		}

		if (read_char == pattern[i]){
			i++;
			found_size++;
		}
		else{
			i = 0;
			found_size = 0;
		}
	}
	return SUCCESS;
}


CFA_StatusTypeDef com_SecureUartTx(uint8_t* data, uint16_t len, char* cmd, int cmd_size)
{
	HAL_StatusTypeDef hal_status;

	// Transmit cmd message to sync
	hal_status = HAL_UART_Transmit(&hlpuart1, (uint8_t*)cmd, cmd_size, TIMEOUT_TX);
	if (hal_status == HAL_TIMEOUT)
		return ERROR_TX_TIMEOUT;

	if (len == 0) return SUCCESS;

	// Transmit message
	hal_status = HAL_UART_Transmit(&hlpuart1, (uint8_t*)data, len, TIMEOUT_TX);
	if (hal_status == HAL_TIMEOUT)
		return ERROR_TX_TIMEOUT;


	return SUCCESS;
}


CFA_StatusTypeDef com_SecureUartRx(uint8_t* data, uint16_t len, char* cmd, int cmd_size)
{
	CFA_StatusTypeDef cfas;
	HAL_StatusTypeDef hal_status;

	// Wait for the initial command to sync
	if ((cfas = com_wait_for_command(cmd, cmd_size)))
		return cfas;

	if (len == 0) return SUCCESS;

	// Receive the message
	hal_status = HAL_UART_Receive(&hlpuart1, data, len, TIMEOUT_RX);
	if (hal_status == HAL_TIMEOUT)
		return ERROR_RX_TIMEOUT;

	return SUCCESS;
}




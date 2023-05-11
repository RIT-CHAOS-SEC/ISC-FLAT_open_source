/*
 * cfa_communication.c
 *
 *  Created on: Feb 1, 2023
 *      Author: nneto
 */


#include <main.h>
#include "usart.h"
#include "cfa_engine.h"

#define TIMEOUT_TX 					0xFFFFFF
#define TIMEOUT_RX 					0xFFFFFF

extern UART_HandleTypeDef hlpuart1;
typedef uint8_t CFA_StatusTypeDef;

CFA_StatusTypeDef com_wait_for_command(char pattern[],int size);

CFA_StatusTypeDef com_SecureUartTx(uint8_t* data, uint16_t len, char* cmd, int cmd_size);

CFA_StatusTypeDef com_SecureUartRx(uint8_t* data, uint16_t len, char* cmd, int cmd_size);


int com_init_();

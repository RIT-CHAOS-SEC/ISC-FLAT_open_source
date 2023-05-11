/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Secure/Src/secure_nsc.c
  * @author  MCD Application Team
  * @brief   This file contains the non-secure callable APIs (secure world)
  ******************************************************************************
    * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* USER CODE END Header */

/* USER CODE BEGIN Non_Secure_CallLib */
/* Includes ------------------------------------------------------------------*/
#include "secure_nsc.h"
#include "main.h"
#include "cfa_engine.h"
#include "gpio.h"
/** @addtogroup STM32L5xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Global variables ----------------------------------------------------------*/
void *pSecureFaultCallback = NULL;   /* Pointer to secure fault callback in Non-secure */
void *pSecureErrorCallback = NULL;   /* Pointer to secure error callback in Non-secure */
void *pAttestationFunctionCallback = NULL;
void *pInterruptHandlerCallback = NULL;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Secure registration of non-secure callback.
  * @param  CallbackId  callback identifier
  * @param  func        pointer to non-secure function
  * @retval None
  */

CMSE_NS_ENTRY void SECURE_RegisterCallback(SECURE_CallbackIDTypeDef CallbackId, void *func){
  if(func != NULL)
  {
    switch(CallbackId)
    {
      case SECURE_FAULT_CB_ID:           /* SecureFault Interrupt occurred */
			pSecureFaultCallback = func;
			break;
      case GTZC_ERROR_CB_ID:             /* GTZC Interrupt occurred */
			pSecureErrorCallback = func;
			break;
      case ATTESTATION_APP_ID:
			pAttestationFunctionCallback = func;
			cfae_register_application();
			break;
      case 0x03U:
    	  	pInterruptHandlerCallback = func;
    	  	cfae_register_interrupt_handler();
			break;
      default:
        break;
    }
  }
}

CMSE_NS_ENTRY void SECURE_log_entry_memory_address(void){
	__disable_irq();
	void *addr = __builtin_extract_return_addr (__builtin_return_address (0));
	cfae_add_new_log_entry((uint32_t)addr);
	__enable_irq();
	return;
}

CMSE_NS_ENTRY void SECURE_new_log_entry(uint32_t val){
	__disable_irq();
	cfae_add_new_log_entry(val);
	__enable_irq();
	return;
}

CMSE_NS_ENTRY void  SECURE_run_attestation(void){
	cfae_initialize_attestation();
	return;
};

CMSE_NS_ENTRY void SECURE_run_attestation_with_ISCFLAT(void){
	ism_initialize_attestation();
}
//1.640
//1.585
//1.616
/**
  * @}
  */

/**
  * @}
  */
/* USER CODE END Non_Secure_CallLib */


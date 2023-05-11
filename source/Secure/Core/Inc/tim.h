/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.h
  * @brief   This file contains all the function prototypes for
  *          the tim.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "secure_nsc.h"

/* USER CODE BEGIN Includes */

#define GET_CLOCK_COUNT(TIMx) 	TIMx.Instance->CNT
#define SET_CLOCK(TIMx,VAL) 	TIMx.Instance->CNT = VAL

//#define CLOCK_FREQ 110000000
//#define TIM4_INTERRUPT_FREQ 100000// Frequency in Hz+
//
////#define ENABLE_INTERRUPT 1

//#ifdef INST_ISFLAT
////#define ENABLE_INTERRUPT 1
//#endif
//

/* USER CODE END Includes */

extern TIM_HandleTypeDef htim3;

extern TIM_HandleTypeDef htim4;

extern TIM_HandleTypeDef htim5;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_TIM3_Init(void);
void MX_TIM4_Init(void);
void MX_TIM5_Init(void);

/* USER CODE BEGIN Prototypes */

uint64_t TIM_measure_clock_precise();
uint64_t TIM_measure_clock();
void TIM_reset_clock_precision();
void TIM_reset_clock();
void TIM_start_tim_3();
void TIM_start_tim_4();
void TIM_start_tim_5();
void TIM_syncDebbugTimer();
void TIM_stop_tim_4();

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */


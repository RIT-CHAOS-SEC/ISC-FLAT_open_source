
#ifndef INC_ISM_H_
#define INC_ISM_H_

#include "cfa_engine.h"
#include "mpu_armv8.h"
#include "core_cm33.h"

// Errors
#define ERROR_ISM_CONFIGURING_MPU 	50U
#define ERROR_ISM_CONFIGURING_SAU 	51U
#define ERROR_ISM_BLOCKING_NVIC 	52U
#define ERROR_ISM_KIDNAPPING_NVIC	53U
#define ERROR_ISM_NO_INTERRU_FOUND	54U

// Memory Location
#define MCU_REGION_NUMBER_NVIC 		0U
#define MCU_REGION_NUMBER_STACK		1U

// Memory Permissions
#define PERMISSION_READ_ONLY		3U
#define PERMISSION_READ_WRITE		1U

// Global Values
#define MAX_INTERRUPT_VECTOR_SIZE 	100U

// Non Secure Alias
#define NS_MPU_ALIAS  ((MPU_Type*) 	0xE002ED90UL)
#define SCB_NS_ALIAS ((SCB_Type*)   0xE002ED08UL) // Non-secure SCB alias 0xE002ED08
#define NS_MPU_INITIAL_MEM 		0xe000ed80UL
#define NS_MPU_FINAL_MEM   		0xe000eda0UL

typedef struct NSMPU{
	uint32_t RBAR; //Region Base Address Register value
	uint32_t RLAR; //Region Limit Address Register value
	uint32_t permission;
	uint32_t region;
}NSMPU_config;

typedef struct ISCFLAT_CONFIG{
	funcptr_NS * interrupt_handlers;
	NSMPU_config mpu_config_stack;
	NSMPU_config mpu_config_nvic;

}ISCFLAT_CONFIG;

typedef struct ACTIVE_INTERRUPTS{
	uint32_t active[30];
	uint16_t size;
}ACTIVE_INTERRUPTS;


CFA_StatusTypeDef ism_initialize_attestation();

#endif /* INC_ISM_H_ */

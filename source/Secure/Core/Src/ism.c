#include <secure_nsc.h>
#include <string.h>
#include <tim.h>
#include "cfa_engine.h"
#include "stm32l552xx.h"
#include "blake2s.h"
#include "usart.h"
#include "ism.h"
#define REPET_APPLICATION 1 // 100 for tests

//#define ENERGY_EXPERIMENT 1
extern CFA_ENGINE_CONFIG CFconfig;
extern CFA_PERFORMANCE_REPORT CFperformance;
ISCFLAT_CONFIG ISCconfig;

extern funcptr_NS  ns_interrupt_handler;
extern void *pInterruptHandlerCallback;
ACTIVE_INTERRUPTS ai;

/**
  \brief   XXXX
  \details XXXX
 */

__STATIC_FORCEINLINE void ism_set_mpu_config(NSMPU_config* mpu_config){
	// barrier instruction to context sync
	__DMB();

	ARM_MPU_Disable();

	// Set the MPU region
	NS_MPU_ALIAS->RNR = mpu_config->region;

	// Set the memory attribute
	ARM_MPU_SetMemAttrEx(NS_MPU_ALIAS, mpu_config->region, ARM_MPU_ATTR_NON_CACHEABLE);

	MPU->RBAR = ARM_MPU_RBAR(mpu_config->RBAR,
							0UL,0UL,0UL,1UL);

	NS_MPU_ALIAS->RLAR = ARM_MPU_RLAR(mpu_config->RLAR,1);

	// Set the controller to enable MPU
    MPU->CTRL =
        (1 << MPU_CTRL_PRIVDEFENA_Pos) |            // Use the default memory map for unmapped addresses
        (1 << MPU_CTRL_HFNMIENA_Pos) |              // Keep MPU turned on for faults
        (1 << MPU_CTRL_ENABLE_Pos);                 // Enable MPU

    // barrier instruction to context sync
	__DSB();
	__ISB();
}
// ----- END DEVICE SPECIFIC ------

/* @  - Add Description
 * @
 */
CFA_StatusTypeDef ism_register_interrupt_handler(){
	if (pInterruptHandlerCallback == NULL)
		return ERROR_REGISTERING_IT_HAND;
	ns_interrupt_handler = (funcptr_NS)  pInterruptHandlerCallback;
	return SUCCESS;
}

/* @  - Add Description
 * @
 */
#define get_active_nvic_(IRQn) ((uint32_t)(((NVIC->IABR[(((uint32_t)IRQn) >> 5UL)] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL))
uint32_t* ism_get_interrupt_handler(void){
	uint32_t* result;
	for (int i = 0; i < ai.size ; i++){
		if (get_active_nvic_(ai.active[i])){
			result = (uint32_t*) SCB_NS_ALIAS->VTOR;
			return result[ai.active[i+16]];
		}
	}
	return NULL;
}

/* @  - Add Description
 * @
 */
CFA_StatusTypeDef ism_dispatch_interrupt(){

	__disable_irq();

	// Store previous LAC and set LAC=False
	uint8_t previous_LAC = LAC;
	uint32_t current_mpu_RLAR;
	uint32_t current_mpu_permission;

	CLEAR_LAC();

	// Get Current NS_SP
	uint32_t current_SP = __TZ_get_MSP_NS();

	// Get current mpu limit address
	current_mpu_RLAR = ISCconfig.mpu_config_stack.RLAR; //
	current_mpu_permission = ISCconfig.mpu_config_stack.permission; //

	 // Set MPU to block writing permission of the non secure stack
	ISCconfig.mpu_config_stack.RLAR = current_SP;
	ISCconfig.mpu_config_stack.permission = PERMISSION_READ_ONLY;
	ism_set_mpu_config(&ISCconfig.mpu_config_stack);

	__enable_irq();

	// find interrupt handler and run the application
	funcptr_NS interrupt_handler = (funcptr_NS)ism_get_interrupt_handler();
	if(interrupt_handler == NULL)
		return ERROR_ISM_NO_INTERRU_FOUND;
	interrupt_handler();

	__disable_irq();

	// Recover MPU config
	if(current_mpu_RLAR){
		ISCconfig.mpu_config_stack.RLAR = current_mpu_RLAR;
		ISCconfig.mpu_config_stack.permission = current_mpu_permission;
		ism_set_mpu_config(&ISCconfig.mpu_config_stack);
	}
	else{
		ARM_MPU_ClrRegionEx(NS_MPU_ALIAS , ISCconfig.mpu_config_stack.region );
	}

	// Recover NS-SP
	__TZ_set_MSP_NS(current_SP);

	// Recover previous LAC
	if (previous_LAC)
		SET_LAC();

	__enable_irq();

	return SUCCESS;
}

/* @  - Add Description
 * @
 */
CFA_StatusTypeDef ism_final_setup(){

	CFA_StatusTypeDef cfas;
	// Set LAC == FALSE
	CLEAR_LAC();

#if defined ACTIVATE_SIG_MEASUREMENT && defined ACTIVATE_PERFORMANCE_EVAL
	TIM_reset_clock_precision();
	time_hp = (uint64_t) TIM_measure_clock_precise();
#endif

	// Sign the final report
	if ((cfas = cfae_sign_report()))
		return cfas;

#if defined ACTIVATE_MEMHASH_MEASUREMENT && defined ACTIVATE_PERFORMANCE_EVAL
	CFperformance.signature_time = (uint64_t) TIM_measure_clock_precise() - time_hp;
#endif

	// Send final report to vrf
	if ((cfas = cfae_send_report()))
		return cfas;

	// Set the flag indicatinig that there is no attestation running;
	CLEAR_ATTESTATION_STATUS();

	ism_free_NVIC();
	ism_free_SAU();

	return SUCCESS;
}

/* @  - Add Description
 * @
 */
CFA_StatusTypeDef ism_kidnap_NVIC(){

	for(int IRQn = 0; IRQn <= MAX_INTERRUPT_VECTOR_SIZE; IRQn++){  // it can start with -15 if considering ARM Cortex-M33 Specific Interrupt
		if (NVIC_GetTargetState(IRQn)){
			__NVIC_SetVector(IRQn,(uint32_t) &ism_dispatch_interrupt);
			NVIC_SetTargetState(IRQn); // "i" must be positive in this func
			ai.active[ai.size] = IRQn;
			ai.size++;
		}
	}
	return SUCCESS;
}


/* @  - Add Description
 * @
 */
CFA_StatusTypeDef ism_kidnap_NSMPU(){
	  SAU->CTRL &= ~(1UL);
	  SAU->RNR = 7;
	  SAU->RBAR = (NS_MPU_INITIAL_MEM);
	  SAU->RLAR = (NS_MPU_FINAL_MEM | 3UL);
	  SAU->CTRL |= (3UL);
	  return SUCCESS;
}

/* @  - Add Description
 * @ S
 */
CFA_StatusTypeDef ism_free_SAU(){
	  SAU->RNR = 7;
	  SAU->CTRL &= ~(1UL);
	  return SUCCESS;
}


/* @  - Add Description
 * @
 */
CFA_StatusTypeDef ism_free_NVIC(){
	for(int IRQn = 0; IRQn <= MAX_INTERRUPT_VECTOR_SIZE; IRQn++){  // it can start with -15 if considering ARM Cortex-M33 Specific Interrupt
		if (  ((uint32_t)__NVIC_GetVector(IRQn)) ==  ( (uint32_t)&ism_dispatch_interrupt)  )   {
			__NVIC_SetVector(IRQn, (uint32_t) NULL);
			NVIC_ClearTargetState(IRQn); // "i" must be positive in this func
		}
	}
	return SUCCESS;
}

/* @  - Add Description
 * @
 */
CFA_StatusTypeDef ism_initial_setup(){

	CFA_StatusTypeDef cfas;

	// Check if attestation is already active
	if(ATTESTATION_STATUS)
		return ERROR_ATT_ALREADY_ACTIVE;
	__disable_irq();

	// Set SAU to block NS-MPU
	if(cfas = ism_kidnap_NSMPU())
		return cfas;

//	// Configure NVIC Table with NS-MPU -> Just make sense when we consider interrupts related with app
	//									   Not the case for now
//	if(cfas = ism_block_nvic_table_from_ns_w())
//		return cfas;

#if defined ACTIVATE_MEMHASH_MEASUREMENT && defined ACTIVATE_PERFORMANCE_EVAL
	TIM_reset_clock_precision();
	time_hp = (uint64_t) TIM_measure_clock_precise();
#endif

	// Generate the binary hash of app before the execution
	if ((cfas = cfae_update_memhash()))
		return cfas;

#if defined ACTIVATE_MEMHASH_MEASUREMENT && defined ACTIVATE_PERFORMANCE_EVAL
	CFperformance.hash_memory_time = (uint64_t) TIM_measure_clock_precise() - time_hp;
#endif

	ai.size = 0;
	// Assign all interrupts as secure to be handled by the dispatcher
	if((cfas = ism_kidnap_NVIC()))
		return cfas;

	// Save initial ns stacker point
	ISCconfig.mpu_config_stack.RBAR = __TZ_get_MSP_NS(); // SAVE INITIAL STACKER POINT

	// Init MPU Variables
	ISCconfig.mpu_config_stack.RLAR = 0;
	ISCconfig.mpu_config_stack.region = MCU_REGION_NUMBER_STACK;
	ISCconfig.mpu_config_stack.permission = PERMISSION_READ_WRITE;

	// Avtivate MPU

	// Set LAC == True
	SET_LAC();

	return SUCCESS;
}


/* @  - Add Description
 * @
 */
CFA_StatusTypeDef ism_run_initialization_module(){
	uint64_t time_hp;

	// check if the application to be attested is registered
	if(CFconfig.application == NULL)
		return ERROR_NO_REGISTERED_APP;

	CFA_StatusTypeDef cfas;

	// Check if application pointer in inside the memory bound
	if ((cfas = cfae_check_application_bounds()))
		return cfas;

	// Update challange
	if ((cfas = cfae_update_challange()))
		return cfas;

	// Clean cfreport
	cfae_clean_attestation_structure();

	// Initial Setup
	ism_initial_setup();

#if defined ACTIVATE_ENERGY_MEASUREMENT_TRIGGER && defined ACTIVATE_PERFORMANCE_EVAL
#if ACTIVATE_ENERGY_MEASUREMENT_TRIGGER == 1
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
	delay(100)
#endif
#endif

	cfae_run_attestatation();

#if defined ACTIVATE_APLICATION_EVAL && defined ACTIVATE_PERFORMANCE_EVAL
#if ACTIVATE_PERFORMANCE_EVAL==1
	TIM_reset_clock_precision();
	time_hp = (uint64_t) TIM_measure_clock_precise();
#endif
#endif

	//Enable interrupts
	__enable_irq();

	// Call ns application "REPET_APPLICATION" times
	#ifdef	REPET_APPLICATION
	for(int k=1;k<REPET_APPLICATION;k++)
	#endif
		CFconfig.application();

	//Disable interrupts
	__disable_irq();

#if defined ACTIVATE_ENERGY_MEASUREMENT_RS && defined ACTIVATE_PERFORMANCE_EVAL
#if ACTIVATE_ENERGY_MEASUREMENT_RS == 1
	NVIC_SystemReset();
#endif
#endif

#if defined ACTIVATE_APLICATION_EVAL && defined ACTIVATE_PERFORMANCE_EVAL
#if ACTIVATE_PERFORMANCE_EVAL==1
	CFperformance.application_time = (uint64_t) TIM_measure_clock_precise() - time_hp;
#endif
#endif

	// Invoke finalization routine
	ism_final_setup();

	return SUCCESS;
}

/* @  - Add Description
 * @
 */
CFA_StatusTypeDef ism_initialize_attestation(){

	CFA_StatusTypeDef cfas;

	// Wait for attestation request
	if (( cfas = cfae_wait_for_attestation_request())){
		return cfas;
	}

	//Disable interrupts
	__disable_irq();

	// run attestation with ISCFLAT
	if (( cfas = ism_run_initialization_module())){
		return cfas;
	}

#ifdef ACTIVATE_PERFORMANCE_EVAL
	// Send Performance Report
	if ((cfas = cfae_send_performance_evaluation()))
		return cfas;
#endif

	//Enable interrupts
	__enable_irq();

#if defined ACTIVATE_ENERGY_MEASUREMENT_RS && defined ACTIVATE_PERFORMANCE_EVAL
#if ACTIVATE_ENERGY_MEASUREMENT_RS == 2
	NVIC_SystemReset();
#endif
#endif

	return SUCCESS;
}


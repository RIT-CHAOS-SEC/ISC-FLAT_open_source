#include<cfa_engine.h>
#include<secure_nsc.h>
#include<string.h>
#include<tim.h>
#include"stm32l552xx.h"
#include "blake2s.h"
#include"usart.h"

#define REPET_APPLICATION 1 // 100 for tests

//#define ENERGY_EXPERIMENT 1

CFA_ENGINE_REPORT CFreport;
CFA_ENGINE_CONFIG CFconfig;
CFA_PERFORMANCE_REPORT CFperformance;

funcptr_NS  ns_interrupt_handler;

extern void *pAttestationFunctionCallback;
extern void *pInterruptHandlerCallback;

uint32_t calculate_report_size(){
	return (CFreport.log_counter*4 + BLAKE2B_OUTBYTES + 2);
}

/* @ Add description
 * @
 */
CFA_StatusTypeDef cfae_register_application_memory_range(uint32_t memory_init, uint32_t memory_end){
	if  (memory_end <= memory_init)  // ADD MORE RESTRICTIONS
		return ERROR_REGISTERING_APP_MEM;

	CFconfig.app_memory_range[0] = memory_init;
	CFconfig.app_memory_range[1] = memory_end;
	return SUCCESS;
}

/* @ Add description
 * @
 */
CFA_StatusTypeDef cfae_initialize_cf_engine(){
	// Test if it is already initialized
	if (INITIALIZATION_STATUS) return SUCCESS;

	// ADD RESTRICTIONS
	// Set the address range  of the application // TEMP
	cfae_register_application_memory_range(0x080401f8,0x08040c1c); // TEMP

	// Initialize application pointer as NULL -> Must be registered by the ns world
	CFconfig.application = NULL;

	// Set initialized flag
	CLEAR_ATTESTATION_STATUS();
	SET_INITIALIZATION_STATUS();
	return SUCCESS;
}

CFA_StatusTypeDef cfae_check_application_bounds(){
	if ((uint32_t)CFconfig.application < (uint32_t)CFconfig.app_memory_range[1] &&
		(uint32_t)CFconfig.application >= (uint32_t)CFconfig.app_memory_range[0])
		return SUCCESS;
	return ERROR_APPLICATION_OUT_OF_B;
}

/* @ Add description
 * @
 */
CFA_StatusTypeDef cfae_register_application(){
	// Check if app function was registered
	if (pAttestationFunctionCallback != NULL){ // ADD MORE RESTRICTIONS
		// register the ns application in the attestation structure
		CFconfig.application = (funcptr_NS)  pAttestationFunctionCallback;
		return SUCCESS;
	}
	return ERROR_REGISTERING_APP;
}

/* @ Add description
 * @
 */
CFA_StatusTypeDef cfae_add_new_log_entry(uint32_t value){

	// Check if LAC is true
	if (!LAC) return ERROR_LAC_LOCKED;

	// Check if the CFlog is full
	if (CFreport.log_counter >= MAX_CF_LOG_SIZE)
		return ERROR_LOG_FULL;

	// Loop optimization
	if (CFconfig.last_log_entry == value){
		CFconfig.last_log_entry_counter ++;
		return SUCCESS;
	}

	// End of loop optimization
	if (CFconfig.last_log_entry_counter > 0){
		CFreport.CFLog[CFreport.log_counter] = CFconfig.last_log_entry_counter ;
		CFreport.log_counter ++;
		CFconfig.last_log_entry_counter = 0;
		return SUCCESS;
	}

	// Simply add the new entry if not in loop optimization
	CFreport.CFLog[CFreport.log_counter] = value;
	CFreport.log_counter ++;
	CFconfig.last_log_entry  = value;
	return SUCCESS;
}

/* @ Add description
 * @
 */
CFA_StatusTypeDef cfae_update_memhash(){
	if(blake2s_init(&CFconfig.blake_context, 32, NULL,32))
		return ERROR_HASH;
	// TODO ADD DAT VALIDATION
	blake2s_update(&CFconfig.blake_context,(char*)CFconfig.app_memory_range[0], CFconfig.app_memory_range[1]);
	blake2s_final(&CFconfig.blake_context, &CFconfig.mem_hash);
	return SUCCESS;
}

/* @ Add description
 * @
 */
CFA_StatusTypeDef cfae_sign_report(){
	if(blake2s_init(&CFconfig.blake_context, 32, CFconfig.key, 32)) // TODO FIX TO 64
		return ERROR_HASH;
	blake2s_update(&CFconfig.blake_context,(char*)&CFreport, calculate_report_size());
	blake2s_final(&CFconfig.blake_context, &CFconfig.mem_hash);
	return SUCCESS;
}


/* @ Add description
 * @
 */
CFA_StatusTypeDef cfae_update_challange(){
	uint8_t chl[CHALLANGE_SIZE];
	CFA_StatusTypeDef cfas;

	if ((cfas=com_SecureUartRx(chl,CHALLANGE_SIZE,"CHAL",4)))
		return ERROR_RECEIVING_CHL;

	#ifdef DEBUG
		if((cfas = com_SecureUartTx(chl,CHALLANGE_SIZE,"CHAL",4)))
			return ERROR_SENDING_CHL;
	#endif

	memcpy(CFconfig.challenge,chl,CHALLANGE_SIZE);

	return SUCCESS;
}

/* @ Add description
 * @
 */
void cancel_attestation(){
	return;
}

/* @ Add description
 * @
 */
CFA_StatusTypeDef cfae_send_report(){

	CFA_StatusTypeDef cfas;
	if((cfas = com_SecureUartTx((uint8_t*)&CFreport,calculate_report_size(),"REPT",4)))
		return ERROR_SENDING_REPORT;

	return SUCCESS;

}

CFA_StatusTypeDef cfae_send_performance_evaluation(){

	CFA_StatusTypeDef cfas;
	if((cfas = com_SecureUartTx((uint8_t*)&CFperformance,64*4,"PERF",4)))
		return ERROR_SENDING_REPORT;
	return SUCCESS;
}


CFA_StatusTypeDef cfae_wait_for_attestation_request(){
	CFA_StatusTypeDef cfas;

	if ((cfas=com_SecureUartRx(NULL,0,"STAR",4))){
		return ERROR_REC_REQUEST_TOKEN;
	}

	if((cfas = com_SecureUartTx(NULL,0,"ACPT",4))){
		return ERROR_SEN_REQUEST_TOKEN;
	}

	return SUCCESS;
}

void cfae_clean_attestation_structure(){
	CFconfig.last_log_entry_counter = 0;
	CFconfig.last_log_entry = -1;
	memset((void*)&CFreport,0,sizeof(CFA_ENGINE_REPORT));
}

/* @ Add description
 * @
 */
/////////////////////////////////////////////////////////////////////

CFA_StatusTypeDef cfae_run_attestatation(){
	uint64_t time_hp;
	uint64_t time_lp;
	uint32_t time_core;

	if(!INITIALIZATION_STATUS)
		return ERROR_ATT_STRUCK_NOT_INIT;

	// Check if attestation is already active
	if(ATTESTATION_STATUS)
		return ERROR_ATT_ALREADY_ACTIVE;
	SET_ATTESTATION_STATUS();

	// check if the application to be attested is registered
	if(CFconfig.application == NULL)
		return ERROR_NO_REGISTERED_APP;

	CFA_StatusTypeDef cfas;

	// Check if application pointer in inside the memory bound
//	if ((cfas = cfae_check_application_bounds()))
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

	// Update challange
	if ((cfas = cfae_update_challange()))
		return cfas;

	// Clean cfreport
	cfae_clean_attestation_structure();

#if defined ACTIVATE_ENERGY_MEASUREMENT_TRIGGER && defined ACTIVATE_PERFORMANCE_EVAL
#if ACTIVATE_ENERGY_MEASUREMENT_TRIGGER == 1
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
	delay(100)
#endif
#endif


#if defined ACTIVATE_APLICATION_EVAL && defined ACTIVATE_PERFORMANCE_EVAL
#if ACTIVATE_PERFORMANCE_EVAL==1
	TIM_reset_clock_precision();
	time_hp = (uint64_t) TIM_measure_clock_precise();
#endif
#endif

	//Enable interrupts
	__enable_irq();

	// Call ns application "REPET_APPLICATION" times
	for(int k=1;k<REPET_APPLICATION;k++)
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

	// Set Attestation as inactive
	CLEAR_ATTESTATION_STATUS();

	return SUCCESS;
}

/* @ Add description
 * @
 */
CFA_StatusTypeDef cfae_initialize_attestation(){

	CFA_StatusTypeDef cfas;

	// Wait for attestation request
	if (( cfas = cfae_wait_for_attestation_request())){
		return cfas;
	}

	//Disable interrupts
	__disable_irq();

	// run attestation
	cfas = cfae_run_attestatation();

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

/* @ Add description
 * @
 */
void cfae_register_interrupt_handler(){
	if (pInterruptHandlerCallback == NULL)
		return;
	ns_interrupt_handler = (funcptr_NS)  pInterruptHandlerCallback;
	return;
}

/* @ Add description
 * @
 */
void cfae_dispatch_interrupt(){
	if (ns_interrupt_handler == NULL)
		return;
	ns_interrupt_handler();
	return;
}




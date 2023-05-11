#ifndef INC_CFA_ENGINE_H_
#define INC_CFA_ENGINE_H_

#include <blake2s.h>
#include <stdlib.h>
#include "secure_nsc.h"
#include "stm32l5xx.h"
#include "stm32l5xx_hal.h"
#include "main.h"
#include "cmsis_gcc.h"
#include "cfa_communication.h"

#if defined ( __ICCARM__ )
#  define CMSE_NS_CALL  __cmse_nonsecure_call
#  define CMSE_NS_ENTRY __cmse_nonsecure_entry
#else
#  define CMSE_NS_CALL  __attribute((cmse_nonsecure_call))
#  define CMSE_NS_ENTRY __attribute((cmse_nonsecure_entry))
#endif

// Messages
#define ERROR_LAC_LOCKED   			20U
#define ERROR_ATT_STRUCK_NOT_INIT   19U
#define ERROR_ATT_ALREADY_ACTIVE    18U
#define ERROR_APPLICATION_OUT_OF_B  17U
#define ERROR_DISPATCHING_IT_HAND   16U
#define ERROR_REGISTERING_IT_HAND   15U
#define ERROR_REGISTERING_APP		14U
#define ERROR_REGISTERING_APP_MEM	13U
#define ERROR_HASH					12U
#define ERROR_REQUEST_NOT_RECEIVED	11U
#define ERROR_LOG_FULL				10U
#define ERROR_SEN_REQUEST_TOKEN		9U
#define ERROR_REC_REQUEST_TOKEN		8U
#define ERROR_SENDING_REPORT		7U
#define ERROR_SENDING_CHL		 	6U
#define ERROR_RECEIVING_CHL		 	5U
#define ERROR_TX_TIMEOUT		 	4U
#define ERROR_RX_TIMEOUT		 	3U
#define ERROR_NO_REGISTERED_APP 	2U
#define ERROR_MEMORY_ALLOCATION 	1U
#define SUCCESS 					0U

// Generic
#define TRUE						1U
#define FALSE						0U

// Active/Deactive debug mode
#define DEBUG						1U
#define ENERGY_EXPERIMENT 			1U

// Parameters
#define MAX_CF_LOG_SIZE 1000U
#define CHALLANGE_SIZE 64U
#define BLAKE2B_OUTBYTES 32U
#define CHAL_SIZE 64U
#define KEY_SIZE 64U


// FLAGS
#define GET_FLAG(POS) 					((CFconfig.flags & (1U << POS))>> POS)
#define SET_FLAG(POS) 					CFconfig.flags = (CFconfig.flags | (1U << POS))
#define CLEAR_FLAG(POS) 				CFconfig.flags = (CFconfig.flags & (~(1U << POS)))

#define ATTESTATION_STATUS_POS 			0
#define ATTESTATION_STATUS 				GET_FLAG(ATTESTATION_STATUS_POS)
#define SET_ATTESTATION_STATUS()		SET_FLAG(ATTESTATION_STATUS_POS)
#define CLEAR_ATTESTATION_STATUS()		CLEAR_FLAG(ATTESTATION_STATUS_POS)

#define INITIALIZATION_STATUS_POS 		1
#define INITIALIZATION_STATUS 			GET_FLAG(INITIALIZATION_STATUS_POS)
#define SET_INITIALIZATION_STATUS()		SET_FLAG(INITIALIZATION_STATUS_POS)
#define CLEAR_INITIALIZATION_STATUS()	CLEAR_FLAG(INITIALIZATION_STATUS_POS)

#define LAC_POS							2
#define LAC								GET_FLAG(LAC_POS)
#define SET_LAC(void)					SET_FLAG(LAC_POS)
#define CLEAR_LAC(void)					CLEAR_FLAG(LAC_POS)

// Macros
#define report_binary_size

/* typedef for non-secure callback functions */
typedef funcptr funcptr_NS;

typedef struct CFA_ENGINE_CONFIG{
	funcptr_NS  		application;
	uint32_t			app_memory_range[2];
	uint32_t			last_log_entry;
	uint32_t			last_log_entry_counter;
	blake2s_ctx 		blake_context;
	uint8_t				flags;
	uint8_t 			key[KEY_SIZE];
	uint8_t				challenge[CHAL_SIZE];
	uint8_t 			mem_hash [BLAKE2B_OUTBYTES];
}CFA_ENGINE_CONFIG;

typedef struct CFA_ENGINE_REPORT{
	uint8_t     		signature[BLAKE2B_OUTBYTES];
	uint16_t 			log_counter;// init = 0
	uint32_t 			CFLog[MAX_CF_LOG_SIZE];
}CFA_ENGINE_REPORT;

typedef struct PERFORMANCE_REPORT{
	uint64_t			hash_memory_time;
	uint64_t			signature_time;
	uint64_t			application_time;
	uint64_t			total_time;
}CFA_PERFORMANCE_REPORT;

typedef uint8_t CFA_StatusTypeDef;


// Public Functions
CFA_StatusTypeDef cfae_initialize_cf_engine(); // Initialzie from secure world

CFA_StatusTypeDef cfae_register_application();
CFA_StatusTypeDef cfae_add_new_log_entry(uint32_t value);
CFA_StatusTypeDef cfae_initialize_attestation();
CFA_StatusTypeDef cfae_check_application_bounds(void);
CFA_StatusTypeDef cfae_update_challange(void);
CFA_StatusTypeDef cfae_wait_for_attestation_request(void);
CFA_StatusTypeDef cfae_update_memhash(void);
CFA_StatusTypeDef cfae_send_report(void);
CFA_StatusTypeDef cfae_sign_report(void);

void cfae_clean_attestation_structure(void);
void cfae_dispatch_interrupt();
void cfae_register_interrupt_handler();








#endif /* INC_CFA_ENGINE_REAL_H_ */

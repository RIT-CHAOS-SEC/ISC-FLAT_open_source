///*
// * protocol.h
// *
// *  Created on: Dec 7, 2022
// *      Author: ac7717
// */
//
//#ifndef INC_PROTOCOL_H_
//#define INC_PROTOCOL_H_
//
//#include "blake2.h"
//
//#define MAX_CF_LOG_SIZE		100
//
//// Attestation Parameters
//#define KEY_SIZE	BLAKE2B_KEYBYTES
//#define CHAL_SIZE	BLAKE2B_KEYBYTES
//
//// Secure / NonSecure world memory boundaries
//// Update by inspecting objdump output
//#define SECURE_FLASH_START				0x0c0001f8
//#define SECURE_FLASH_END				0x0c03e03d
//#define SECURE_FLASH_SIZE				(SECURE_FLASH_END-1)-SECURE_FLASH_START
//// Update by inspecting objdump output
//#define NONSECURE_FLASH_START			0x080401f8
//#define	NONSECURE_FLASH_END				0x08040c1c
//#define	NONSECURE_FLASH_SIZE			(NONSECURE_FLASH_END-1)-NONSECURE_FLASH_START
//
//// STRUCTS
//typedef struct CFA_REPORT{
//	uint16_t			report_size;
//	uint8_t 			CF_Log[MAX_CF_LOG_SIZE];
//}CFA_REPORT;
//
//// Communication wrappers
//void SecureUartTx(uint8_t* data, uint16_t len);
//void SecureUartRx(uint8_t* data, uint16_t len);
//
//// Protocol blocks
//void invokeProtocol();
//void PROTOCOL_Attest();
//int PROTOCOL_Wait();
//void PROTOCOL_Heal();
//
//void CFA_ENGINE_new_log_entry(uint32_t value);
//void CFA_ENGINE_invokeProtocol();
//void CFA_ENGINE_reset_report();
//
//// For Testing
//void setup_data();
//
//#endif /* INC_PROTOCOL_H_ */

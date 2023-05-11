/*
 * attestation.h
 *
 *  Created on: Jun 29, 2022
 *      Author: nneto
 */

#ifndef INC_ATTESTATION_H_
#define INC_ATTESTATION_H_

#define LOG_MAX_SIZE 100

typedef struct CFA_Memory{
	unsigned int logs [LOG_MAX_SIZE];
	int count;
}CFA_Memory;

void CFA_Log(unsigned int logValue);
void initCFAM(void);

#endif /* INC_ATTESTATION_H_ */

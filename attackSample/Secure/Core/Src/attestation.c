/*
 * attestation.c
 *
 *  Created on: Jun 28, 2022
 *      Author: nneto
 */
#include "attestation.h"
#include <stdlib.h>

CFA_Memory  CFAM ;

void initCFAM(){
	CFAM.count = 0;
}

void CFA_Log(unsigned int logValue){

	CFAM.logs[ CFAM.count % LOG_MAX_SIZE] = logValue;
	CFAM.count ++;

}

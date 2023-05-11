/*
 * serial_messages.h
 *
 *  Created on: Dec 7, 2022
 *      Author: aj4775
 */

#ifndef INC_SERIAL_MESSAGES_H_
#define INC_SERIAL_MESSAGES_H_

#include "usart.h"

typedef enum MESSAGES{START, IFORMATION_1,START_APPLICATION,NEW_LINE,REPORT} messages;

void smessage(messages ID);

#endif /* INC_SERIAL_MESSAGES_H_ */

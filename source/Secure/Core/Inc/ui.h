/*
 * ui.h
 *
 *  Created on: Feb 24, 2023
 *      Author: nneto
 */

#ifndef INC_UI_H_
#define INC_UI_H_


// MENU TYPES
#define MENU_ID_MAIN 0U
#define MENU_ID_1 1U
#define MENU_ID_2 2U
#define MENU_ID_3 3U
#define MENU_ID_4 4U
#define MENU_ID_5 5U
#define MENU_ID_SETTINGS 99U
#define INVALID_RESPONSE 100U

// FLAGS
#define GET_FLAG_(POS) 					((flags & (1U << POS))>> POS)
#define SET_FLAG_(POS) 					flags = (flags | (1U << POS))
#define CLEAR_FLAG_(POS) 				flags = (flags & (~(1U << POS)))

#define USE_TOKEN_FLAG_POS				1U
#define USE_TOKEN_FLAG					GET_FLAG_(USE_TOKEN_FLAG_POS)
#define SET_USE_TOKEN_FLAG(void)		SET_FLAG_(USE_TOKEN_FLAG_POS)
#define CLEAR_USE_TOKEN_FLAG(void)		CLEAR_FLAG_(USE_TOKEN_FLAG_POS)

// FUNCTION
#define SEND_MESSAGE(msg)     strcpy(message_vector, msg); \
							  ui_send_message	(message_vector,strlen(message_vector));


// PARTIAL CODE
#define PRINT_AND_READ_ANWSER(menu) 	char answr; \
										ui_print(menu); \
										if(ui_read_answer(&answr))return 1

#define END_CASES 	case 'R':	\
						return 0;	\
					default:	\
						ui_print(INVALID_RESPONSE);	\
						return 1


void ui_start();


#endif /* INC_UI_H_ */

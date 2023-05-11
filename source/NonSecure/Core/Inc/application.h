#ifndef __APPLICATION_H
#define __APPLICATION_H

#include "stdint.h"
#include "secure_nsc.h"

#define DEFAULT	0
#define SYRINGE	1
#define TEMP	2
#define ULT		3
#define ENERGY_TEST		4
#define APP_SEL TEMP


#define INSTRUMENTATION_METHOD 5


#ifdef INSTRUMENTATION_METHOD
	#if INSTRUMENTATION_METHOD == 1
		#define instrumentation(val) SECURE_new_log_entry(val);
		#define instrumentation_2(val) SECURE_new_log_entry(val);
	#elif INSTRUMENTATION_METHOD == 2
		#define instrumentation(val) SECURE_new_log_entry(val);
		#define instrumentation2(val);
	#elif INSTRUMENTATION_METHOD == 3
		#define instrumentation(val) SECURE_log_entry_memory_address();
		#define instrumentation2(val) SECURE_log_entry_memory_address();
	#elif INSTRUMENTATION_METHOD == 4
		#define instrumentation(val) SECURE_log_entry_memory_address();
		#define instrumentation2(val);
	#elif INSTRUMENTATION_METHOD == 5
		#define instrumentation(val) SECURE_log_entry_memory_address();
		#define instrumentation2(val) SECURE_new_log_entry(val);
	#endif
#else
	#define instrumentation(val) {}
	#define instrumentation2(val) {}
#endif

/******************* SYRINGE PUMP READING *********************/

//
//#if APP_SEL == SYRINGE
//// Syringe pump
///* -- Constants -- */
//#define SYRINGE_VOLUME_ML 30.0
//#define SYRINGE_BARREL_LENGTH_MM 8.0
//
//#define THREADED_ROD_PITCH 1.25
//#define STEPS_PER_REVOLUTION 4.0
//#define MICROSTEPS_PER_STEP 16.0
//
//#define SPEED_MICROSECONDS_DELAY 100 //longer delay = lower speed
//
//#define false  0
//#define true   1
//
//#define LED_OUT_PIN 0
///* -- Enums and constants -- */
//int steps;
//
////syringe movement direction
//enum{PUSH,PULL};
//
//void delayMicroseconds(unsigned int delay);
//char getserialinput(uint8_t inputserialpointer);
//#endif // Syringe
//

/******************* TEMPERATURE SENSOR READING *********************/

#if APP_SEL == TEMP



#endif // TMP

/******************* ULTRASONIC SENSOR READING *********************/

//
#if APP_SEL == ULT

#define ULT_PIN         0x02
#define MAX_DURATION    1000

#endif//ult

void application();
void TIM4_InterruptHandler();
#endif /* __APPLICATION_H */

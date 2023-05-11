#include "application.h"
#include "stm32l552xx.h"
#include "stm32l5xx_hal.h"
#include "gpio.h"
void delay(unsigned int us);


// APPS DEFITION
//#define APP_SEL TEMP
//#define APP_SEL ULT

#define APP_SEL TEMP


#if APP_SEL == TEMP

	#define MAX_READINGS	83

#endif // TMP



/******************* DEFAULT APP *********************/
#if APP_SEL == DEFAULT
//void application(){
//	unsigned int i = 0;
//	while (i < 50)
//	{
//		instrumentation(1);
//		if(i < 25)
//		{
//			instrumentation(2);
//			//toggle secure LEDs
////			NscTogglePin();
//		}
//
//		//toggle non-secure LED
//		if(i%2 == 1){
//			instrumentation(3);
//			delay(100);
//		}
//		i++;
//		/* USER CODE END WHILE */
//		instrumentation(4);
//		/* USER CODE BEGIN 3 */
//	}
//	instrumentation(5);
//}
#endif

///******************* SYRINGE APP *********************/
//#if APP_SEL == SYRINGE
//
//char getserialinput(uint8_t inputserialpointer)
//{
//    uint8_t maxinputpointer = 2;
//    char input[2] = "+\n";
//    instrumentation(30);
//    if (inputserialpointer < maxinputpointer)
//    {
//    	instrumentation(31);
//        return input[inputserialpointer];
//    }
//    instrumentation(32);
//    return 0;
//}
//
//void application()
//{
//    /* -- Global variables -- */
//    // Input related variables
//    volatile uint8_t inputserialpointer = -1;
//    uint16_t inputStrLen = 0;
//    char inputStr[10]; //input string storage
//
//    // Bolus size
//    uint16_t mLBolus =  5;
//
//    // Steps per ml
//    int ustepsPerML = (MICROSTEPS_PER_STEP * STEPS_PER_REVOLUTION * SYRINGE_BARREL_LENGTH_MM) / (SYRINGE_VOLUME_ML * THREADED_ROD_PITCH );
//
//    //int ustepsPerML = 10;
//    int inner = 0;
//    int outer = 0;
//    steps = 0;
//
//    while(outer < 1)
//    {
//       instrumentation(1);
//	   char c = getserialinput(++inputserialpointer);
//	   // hex to char reader
//	   while (inner < 10)
//	   {
//		   instrumentation(2);
//		   if(c == '\n') // Custom EOF
//		   {
//			   instrumentation(3);
//			   break;
//		   }
//		   instrumentation(4);
//		   if(c == 0)
//		   {
//			   instrumentation(5);
//			   outer = 10;
//			   break;
//		   }
//		   instrumentation(6);
//		   inputStr[inputStrLen++] = c;
//		   c = getserialinput(++inputserialpointer);
//		   instrumentation(7);
//		   inner += 1;
//	   }
//	   instrumentation(8);
//	   inputStr[inputStrLen++] = '\0';
//	   steps = mLBolus * ustepsPerML;
//
//	   if(inputStr[0] == '+' || inputStr[0] == '-')
//	   {
//		   for(int i=0; i < steps; i++)
//		   {
//				instrumentation(9);
//				// write 0xff to port
//				delay(SPEED_MICROSECONDS_DELAY);
//				// write 0x00 to port
//				instrumentation(11);
//				delay(SPEED_MICROSECONDS_DELAY);
//				instrumentation(12);
//			}
//		   instrumentation(13);
//		}
//	   instrumentation(14);
//		inputStrLen = 0;
//		outer += 1;
//	}
//}
//#endif


 /******************* TEMPERATURE APP *********************/
#if APP_SEL == TEMP

	int temp;
	int humidity;
	uint8_t data[5] = {0,0,0,0,0};
	uint8_t valid_reading = 0;


	void read_data(){
		uint8_t counter = 0;
		uint16_t j = 0, i;

		/// pull signal high & delay
		instrumentation(32); // INSTRUMENTATION POINT
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
		instrumentation(33); // INSTRUMENTATION POINT
		delay(4);
		instrumentation(34); // INSTRUMENTATION POINT

		/// pull signal low for 20us
		instrumentation(35); // INSTRUMENTATION POINT
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
		instrumentation(36); // INSTRUMENTATION POINT
		delay(4);
		instrumentation(37); // INSTRUMENTATION POINT

		/// pull signal high for 40us
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
		instrumentation(38); // INSTRUMENTATION POINT
		delay(8);
		instrumentation(30); // INSTRUMENTATION POINT

		//read timings
		for(i=0; i<MAX_READINGS; i++){
			instrumentation(40); // INSTRUMENTATION POINT
//			counter += (P2IN & TEMP_PIN);   ///////// READ ADC <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

			counter += (data[0] & 0x2); //Change to ///////// READ ADC <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

			// ignore first 3 transitions
			if ((i >= 4) && ( (i & 0x01) == 0x00 ))  {
				instrumentation(41); // INSTRUMENTATION POINT
				// shove each bit into the storage bytes
				data[j >> 3] <<= 1;
				if (counter > 6){
					instrumentation(42); // INSTRUMENTATION POINT
					data[j >> 3] |= 1;
				}
				instrumentation(43); // INSTRUMENTATION POINT
				j++;
			}
			instrumentation(45); // INSTRUMENTATION POINT
		}
		instrumentation(46); // INSTRUMENTATION POINT

		// check we read 40 bits and that the checksum matches
		if ((j >= 40) && (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) ) {
			instrumentation(47); // INSTRUMENTATION POINT
			valid_reading = 1;
		} else {
			instrumentation(48); // INSTRUMENTATION POINT
			valid_reading = 0;
		}
		instrumentation(49); // INSTRUMENTATION POINT
	}

	uint16_t get_temperature(){
		// Get temperature readings
		instrumentation(10); // INSTRUMENTATION POINT
		read_data();
		instrumentation(11); // INSTRUMENTATION POINT

		uint16_t t = data[2];
		t |= (data[3] << 8);
		instrumentation2(13);
		return t;
	}


	uint16_t get_humidity(){
		// Get humidity readings
		instrumentation(30); // INSTRUMENTATION POINT
		read_data();
		instrumentation(31); // INSTRUMENTATION POINT

		uint16_t h = data[0];
		h |= (data[1] << 8);
		return h;
	}

	void application(){
		// Get sensor readings
		instrumentation(0); // INSTRUMENTATION POINT
		temp = get_temperature();
		instrumentation(1); // INSTRUMENTATION POINT
		humidity = get_humidity();
		instrumentation(2); // INSTRUMENTATION POINT
	}
#endif // TEMP


/******************* ENERGY TEST  *********************/
/*
*
*/
#if APP_SEL == ENERGY_TEST
	void application(){
		while(1){

		}
	}
#endif


/******************* ULTRASONIC SENSOR READING *********************/
/*
 * Adaptation of https://github.com/Seeed-Studio/LaunchPad_Kit/tree/master/Grove_Modules/ultrasonic_ranger
 */
#if APP_SEL == ULT

	long pulseIn(void) {

		instrumentation(12); // INSTRUMENTATION POINT
		unsigned long duration = 0;
		int i = 0;

		while(i < MAX_DURATION){
			instrumentation(13); // INSTRUMENTATION POINT
			duration += GPIOA->IDR & GPIO_PIN_9;
			i++;
		}

		instrumentation2(14); // INSTRUMENTATION POINT
		return duration;
	 }

	unsigned long getUltrasonicReadingCentimeters(void){

		unsigned long duration;
		unsigned long RangeInCentimeters;

		// Set as output
		instrumentation(2); // INSTRUMENTATION POINT
		GPIO_set_as_output(GPIOA, GPIO_PIN_9);
		instrumentation(3); // INSTRUMENTATION POINT

		//Set signal low for 50us
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);

		instrumentation(4); // INSTRUMENTATION POINT
		delay(10);
		instrumentation(5); // INSTRUMENTATION POINT

		// Set signal high for 200us
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);

		instrumentation(6); // INSTRUMENTATION POINT
		delay(40);
		instrumentation(7); // INSTRUMENTATION POINT

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);

		// Set as input
		instrumentation(8); // INSTRUMENTATION POINT
		GPIO_set_as_input(GPIOA, GPIO_PIN_9);
		instrumentation(9); // INSTRUMENTATION POINT

		instrumentation(10); // INSTRUMENTATION POINT
		duration = pulseIn();
		instrumentation(11); // INSTRUMENTATION POINT

		RangeInCentimeters = duration/29/2;

		instrumentation2(77);
		return RangeInCentimeters;
	}

	void application(){

		instrumentation(0); // INSTRUMENTATION POINT
		unsigned long ult_reading = getUltrasonicReadingCentimeters();
		instrumentation2(1); // INSTRUMENTATION POINT

	}

#endif

void delay(unsigned int Delay){

	  instrumentation(99); // INSTRUMENTATION POINT
	  uint32_t tickstart = HAL_GetTick();
	  uint32_t wait = Delay;

	  /* Add a period to guaranty minimum wait */
	  if (wait < HAL_MAX_DELAY)
	  {
		instrumentation(100); // INSTRUMENTATION POINT
		wait += (uint32_t)uwTickFreq;
	  }

	  instrumentation(101);

	  // NOTE> This instrumentation point can reduce a little bit the delay's precision
	  while ((HAL_GetTick() - tickstart) < wait)
	  {
		  instrumentation(102); // INSTRUMENTATION POINT
	  }

	  instrumentation2(103); // INSTRUMENTATION POINT
}


int ddd = 0;
void TIM4_InterruptHandler(){
	ddd++;
	for(int i = 0; i< 5; i++)
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
	return;
}

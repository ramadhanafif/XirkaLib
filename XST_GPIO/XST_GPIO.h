/**********************
* Library GPIO untuk Xirka
**********************/

#ifndef _XST_GPIO

#include "CMSDK_CM3.h"

#define GPIO0 CMSDK_GPIO0 
#define GPIO1 CMSDK_GPIO1 

#define OUTPUT 1
#define INPUT 0
#define HIGH 1
#define LOW 0

class XST_GPIO{
	public:
		//Contstructor
		XST_GPIO(CMSDK_GPIO_TypeDef* GPIO_number);
		
		//set a pin as OUTPUT or INPUT
		void pinMode(uint8_t pin, uint8_t io);	
		
		//set digital HIGH or LOW on pin
		void digitalWrite(uint8_t pin, uint8_t state);	
		
		uint8_t digitalRead(uint8_t pin);
	private:
		CMSDK_GPIO_TypeDef* GPIOx;
};

#endif
#include "XST_GPIO.h"

/******************************
*Public Arguments and Functions
*******************************/

XST_GPIO::XST_GPIO(CMSDK_GPIO_TypeDef* GPIO_number) {
  GPIOx = GPIO_number;
  GPIOx->OUTENABLECLR = 0xFFFF;
};

void XST_GPIO::pinMode(uint8_t pin, uint8_t io) {
  if (io == OUTPUT) //set as output
  {
    GPIOx->OUTENABLESET |= 1 << pin;
    GPIOx->OUTENABLECLR &= ~1 << pin;
  }
  else if (io == INPUT) //SET AS INPUT
  {
    GPIOx->OUTENABLESET &= ~1 << pin;
    GPIOx->OUTENABLECLR |= 1 << pin;
  }
};
void XST_GPIO::digitalWrite(uint8_t pin, uint8_t state) {
  if (state == HIGH)
    GPIOx->DATAOUT |= 1 << pin;
  else if (state == LOW)
    GPIOx->DATAOUT &= ~1 << pin;
};

uint8_t XST_GPIO::digitalRead(uint8_t pin) {

  //Manually reset register
  pinMode(pin, OUTPUT);
  GPIOx->DATA &= ~1 << pin;
  GPIOx->DATAOUT &= ~1 << pin;

  uint8_t out = ((GPIOx->DATA & (1 << pin)) > 0);
  
  //re-set as input
  pinMode(pin, INPUT);

  return out;
};

/******************************
*Private Arguments and Functions
*******************************/

CMSDK_GPIO_TypeDef* GPIOx;

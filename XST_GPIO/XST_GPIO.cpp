#include "XST_GPIO.h"

XST_GPIO::XST_GPIO(CMSDK_GPIO_TypeDef* GPIO_number) { //Contstructor
  GPIOx = GPIO_number;
  GPIOx->OUTENABLECLR = 0xFFFF;
};

void XST_GPIO::pinOutClear (uint8_t pin) {
  GPIOx->OUTENABLECLR |= 1 << pin;
};

void XST_GPIO::pinMode(uint8_t pin, uint8_t io) {
  if (io == OUTPUT) //set as output
  {
    GPIOx->OUTENABLESET |= 1 << pin;
    GPIOx->OUTENABLECLR &= ~1 << pin;
  }
  else if (io == INPUT) //un-set as output, but still capable of output
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
  /*
    Fungsi ini belum sesuai harapan.
    pembacaan sudah berhasil namun setelah pembacaan register DATA dan DATAOUT
    jadi bernilai 1, tidak direset lagi jadi 0
    sehingga dilakukan reset manual. Reset manual dikhawatirkan dapat mengganggu
    pin yang lain.
  */

  //Reset data register
  pinMode(pin, OUTPUT);
  GPIOx->DATA &= ~1 << pin;
  GPIOx->DATAOUT &= ~1 << pin;

  uint8_t out = ((GPIOx->DATA & (1 << pin)) > 0);
  pinMode(pin, INPUT);

  return out;
};

CMSDK_GPIO_TypeDef* GPIOx;

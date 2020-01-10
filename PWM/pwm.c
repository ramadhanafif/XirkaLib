#include "CMSDK_CM3.h"

#define TIMER_CTRL_DEFAULT 0x20
#define TIMER_LOAD_DEFAULT 65535

#define PRESCALE_1_1   0
#define PRESCALE_1_16  1
#define PRESCALE_1_256 2

#define PWM_16_MAX 65535

/**************************************************************************/
/*!
    @brief   Enable or disable PWM functionality on pin
    @param   pin pin number (4 or 5)
    @param   pwm 1 to enable pwm, 0 to disable pwm
*/
/**************************************************************************/
void pinMode(uint32_t pin, uint32_t pwm) {
  CMSDK_DUALTIMER_SINGLE_TypeDef* CMSDK_DUALTIMERx;
  if (pin == 4)
    CMSDK_DUALTIMERx = CMSDK_DUALTIMER1;
  else if (pin == 5)
    CMSDK_DUALTIMERx = CMSDK_DUALTIMER2;

  /*Reset Timer Settings*/
  CMSDK_DUALTIMERx->TimerControl = TIMER_CTRL_DEFAULT |
                                   (PRESCALE_1_1 & 0x3) << CMSDK_DUALTIMER_CTRL_PRESCALE_Pos |
                                   CMSDK_DUALTIMER_CTRL_MODE_Msk;

  //PWM Enable
  if (pwm == 1)
    CMSDK_DUALTIMERx->TimerControl |= 0x100; //enable pwm
  else
    CMSDK_DUALTIMERx->TimerControl &= ~0x100; //disable pwm

  //GPIO Enable
  CMSDK_GPIO0->OUTENABLESET = 1 << pin;
  CMSDK_GPIO0->ALTFUNCSET = 1 << pin;

  CMSDK_DUALTIMERx->TimerLoad = TIMER_LOAD_DEFAULT; //default load value
}

/**************************************************************************/
/*!
    @brief   Set PWM on cycle
    @param   pin pin number (4 or 5)
    @param   on_cycle on cycle, with default period cycle is 65535 (0 to period cycle)
*/
/**************************************************************************/
void pwmWrite(uint32_t pin, uint32_t on_cycle) {
  CMSDK_DUALTIMER_SINGLE_TypeDef* CMSDK_DUALTIMERx;
  if (pin == 4)
    CMSDK_DUALTIMERx = CMSDK_DUALTIMER1;
  else if (pin == 5)
    CMSDK_DUALTIMERx = CMSDK_DUALTIMER2;

  CMSDK_DUALTIMERx->TimerComp = CMSDK_DUALTIMERx->TimerLoad - on_cycle; //dibalik karena down counting

  CMSDK_DUALTIMERx->TimerControl |= CMSDK_DUALTIMER_CTRL_EN_Msk; //enable PWM and timer
}

/**************************************************************************/
/*!
    @brief   Change PWM period
    @param   pin pin number (4 or 5)
    @param   load maximum counting value, 0 to 65535
*/
/**************************************************************************/
void pwmChangeLoad(uint32_t pin, uint32_t load) {
  CMSDK_DUALTIMER_SINGLE_TypeDef* CMSDK_DUALTIMERx;
  if (pin == 4)
    CMSDK_DUALTIMERx = CMSDK_DUALTIMER1;
  else if (pin == 5)
    CMSDK_DUALTIMERx = CMSDK_DUALTIMER2;

  CMSDK_DUALTIMERx->TimerLoad = load;
}

/**************************************************************************/
/*!
    @brief   Function to convert ducy cycle percentage to on_cycle 
    @param   pin pin number (4 or 5)
    @param   percent duty cycle percentage, 0 to 100
    @return  32-bit integer to be passed to on_cycle
*/
/**************************************************************************/

uint32_t pwmDutyCalc(uint32_t pin, double percent) {
  CMSDK_DUALTIMER_SINGLE_TypeDef* CMSDK_DUALTIMERx;
  if (pin == 4)
    CMSDK_DUALTIMERx = CMSDK_DUALTIMER1;
  else if (pin == 5)
    CMSDK_DUALTIMERx = CMSDK_DUALTIMER2;

  //limit percentage value to avoid error
  if (percent >= 100)
  {
    percent = 100;
  }
  else if (percent <= 0)
  {
    percent = 0;
  }

  return (uint32_t) (CMSDK_DUALTIMERx->TimerLoad * percent / 100);
}

/**************************************************************************/
/*!
    @brief   Change dualtimer clock divider
    @param   pin pin number (4 or 5)
    @param   pwm clock divider (1/1, 1/16, or 1/256)
*/
/**************************************************************************/
void pwmChangeDiv(uint32_t pin, uint32_t prescaler) {
  CMSDK_DUALTIMER_SINGLE_TypeDef* CMSDK_DUALTIMERx;
  if (pin == 4)
    CMSDK_DUALTIMERx = CMSDK_DUALTIMER1;
  else if (pin == 5)
    CMSDK_DUALTIMERx = CMSDK_DUALTIMER2;

  CMSDK_DUALTIMERx->TimerControl |= (prescaler & 0x3) << CMSDK_DUALTIMER_CTRL_PRESCALE_Pos;
}
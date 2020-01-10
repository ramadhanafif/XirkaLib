/**
    Example for pwm.h
    Created 10-Jan-2020
*/

#include "CMSDK_CM3.h"
#include "pwm.h"

int main() {
  unsigned int pin;
  unsigned int pwmvalue;

  /*Only pin 4 and 5 from GPIO 0 supports PWM.
    In this example, GPIO 0 pin 4 is used*/
  pin = 4;

  /*Enable PWM in pin 4*/
  pwmEnable(pin, PWM_ON);

  /*Optional: Use 32-bit dual-timer load with */
  pwmLoadWidth(pin, WIDTH_32);

  /*To obtain the desired frequency, Load must be changed accordingly:
                 fclk
    Load = ----------------
           fpwm * prescaler
    In this example, default 1/1 prescaler is used, with desired frequency is 200KHz, thus the Load is 100
    By default, Load cannot exceed 65535 for 16-bit width. Use pwmLoadWidth to change the load width to 32-bit
  */
  pwmChangeLoad(pin, 100);

  /*No change: setting prescaler to default 1/1*/
  pwmChangeDiv(pin, PRESCALE_1_1);

  /*PWM signal starts with 30% duty cycle*/
  pwmvalue = pwmDutyCalc(pin, 30);
  pwmWrite(pin, pwmvalue);
}

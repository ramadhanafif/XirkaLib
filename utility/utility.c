#include "utility.h"

volatile unsigned long _millis = 0;
volatile unsigned long _micros = 0;
volatile unsigned long micros_old = 0;

volatile uint8_t __timer_active__ = 0;

void timer_start(void) {
  SysTick_Config(SystemFrequency / 100000); //1MHz
  __timer_active__ = 1;
  _millis = 0;
}

void SysTick_Handler(void) {
  _micros+=10;
	if (_micros - micros_old >=1000)
	{
		micros_old = _micros;
		_millis++;
	}
}

unsigned long millis(void) {
  if (__timer_active__)
    return _millis;
  else
    return 0;
}

unsigned long micros(void) {
  if (__timer_active__)
    return _micros;
  else
    return 0;
}

unsigned int delaymilis(uint32_t ms) {
  if (__timer_active__)
  {
    uint32_t end = millis() + ms;
    while (end > millis());
    return ms;
  }
  else
  {
    return 0;
  }
}

unsigned int delaysecond(uint32_t s)
{
  uint32_t ms = s * 1000;
  return delaymilis(ms);
}

unsigned int delaymicros(uint32_t n)
{
  if (__timer_active__)
  {
    uint32_t end = micros() + n;
    while (end > micros());
    return n;
  }
  else
  {
    return 0;
  }
}

unsigned int delay(uint32_t n) {
  return delaymilis(n);
}

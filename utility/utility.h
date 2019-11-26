#ifndef __UTILITY_H
#define __UTILITY_H


#include "CMSDK_CM3.h"

#ifdef __cplusplus
extern "C" {
#endif
unsigned int delaymilis(uint32_t ms);
unsigned int delaysecond(uint32_t s);
unsigned int delaymicros(uint32_t);
unsigned int delay(uint32_t);
void timer_start(void);
static void SysTick_Handler(void);
unsigned long millis(void);
unsigned long micros(void);
#ifdef __cplusplus
}
#endif
#endif //__UTILITY_H 

/**
 * Watchdog Library
 * Version 1
 */
 

#ifndef WATCHDOG_H
#define WATCHDOG_H

#include "CMSDK_CM3.h"

#ifdef __cplusplus
extern "C" {
#endif
uint32_t to_wdCycle(uint32_t ms, uint32_t fclk);
uint32_t wdCycle_to_ms(uint32_t wdCycle, uint32_t fclk);
uint32_t get_wdCycle(void);
void watchdog_unlock(void);
void watchdog_lock(void);
void watchdog_irq_clear(void);
void NMI_Handler(void);
void watchdog_init(unsigned int cycle);
void watchdog_disable(void);
void watchdog_set(unsigned int cycle);
uint32_t prev_reset_source(void);
void clear_rstinfo(void);
#ifdef __cplusplus
}
#endif
#endif
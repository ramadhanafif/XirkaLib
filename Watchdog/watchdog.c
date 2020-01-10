#include "watchdog.h"

/**************************************************************************/
/*!
    @brief   Convert time from millisecond to watchdog cycle
    @param   ms time in millisecond
    @param   fclk hardware clock, default value is referred from SystemFrequency
    @return  watchdog cycle
*/
/**************************************************************************/
uint32_t to_wdCycle(uint32_t ms, uint32_t fclk)
{
  return (uint32_t) ms * (fclk / 1000);
}

/**************************************************************************/
/*!
    @brief   Convert watchdog cycle to millisecond
    @param   wdCycle watchdog cycle
    @param   fclk hardware clock, default value is referred from SystemFrequency
    @return  time in millisecond
*/
/**************************************************************************/
uint32_t wdCycle_to_ms(uint32_t wdCycle, uint32_t fclk)
{
  return (uint32_t) (1000 / fclk)*wdCycle;
}

/**************************************************************************/
/*!
    @brief   get current watchdog cycle
    @return  current watchdog cycle
*/
/**************************************************************************/
uint32_t get_wdCycle(void) {
  return CMSDK_WATCHDOG->VALUE;
}

/**************************************************************************/
/*!
    @brief   unlock watchdog register access
*/
/**************************************************************************/
void watchdog_unlock(void)
{
  CMSDK_WATCHDOG->LOCK = 0x1ACCE551;
}

/**************************************************************************/
/*!
    @brief   lock watchdog register access
*/
/**************************************************************************/
void watchdog_lock(void)
{
  CMSDK_WATCHDOG->LOCK = 0;
}

/**************************************************************************/
/*!
    @brief   watchdog interrupt clear
*/
/**************************************************************************/
void watchdog_irq_clear(void)
{
  watchdog_unlock();
  CMSDK_WATCHDOG->INTCLR = CMSDK_Watchdog_INTCLR_Msk;
  watchdog_lock();
}

/**************************************************************************/
/*!
    @brief   Non-maskable interrupt watchdog handler
*/
/**************************************************************************/
void NMI_Handler(void)
{
  if (CMSDK_WATCHDOG->CTRL & CMSDK_Watchdog_CTRL_RESEN_Msk) {
    /* Watchdog reset enabled. To reset, must stay in NMI handler*/
    //puts("\nWATCHDOG RESET\n");
    while (1) {
      // wait for reset...
    }
  }
  else {
    watchdog_irq_clear();
  }
}

/**************************************************************************/
/*!
    @brief   initialize watchdog cycle count and register setting
    @param   cycle watchdog cycle
*/
/**************************************************************************/
void watchdog_init(unsigned int cycle)
{
  watchdog_unlock();
  CMSDK_WATCHDOG->LOAD = cycle;
  CMSDK_WATCHDOG->CTRL = CMSDK_Watchdog_CTRL_RESEN_Msk | CMSDK_Watchdog_CTRL_INTEN_Msk;
  watchdog_lock();
}

/**************************************************************************/
/*!
    @brief   disable watchdog
*/
/**************************************************************************/
void watchdog_disable(void)
{
  watchdog_unlock();
  CMSDK_WATCHDOG->LOAD = 0;
  CMSDK_WATCHDOG->CTRL = 0;
}

/**************************************************************************/
/*!
    @brief   set watchdog cycle
    @param   cycle watchdog cycle
*/
/**************************************************************************/
void watchdog_set(unsigned int cycle)
{
  watchdog_unlock();
  CMSDK_WATCHDOG->LOAD = cycle;
  watchdog_lock();
}

/**************************************************************************/
/*!
    @brief   check previous reset source
*/
/**************************************************************************/
uint32_t prev_reset_source(void) 
{
  /* Check CMSDK_SYSCON->RSTINFO Reset Information register */
  /*  0 = cold reset */
  /*  1 = reset from SYSRESETREQ */
  /*  2 = reset from Watchdog */
  return CMSDK_SYSCON->RSTINFO;
}

/**************************************************************************/
/*!
    @brief   clear all CMSDK_SYSCON->RSTINFO to 0
*/
/**************************************************************************/
void clear_rstinfo(void) 
{
  CMSDK_SYSCON->RSTINFO = 0xFFFFFFFF;
}
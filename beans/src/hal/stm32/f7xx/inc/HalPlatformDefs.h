// ***************************************************************************
// TITLE
//
// PROJECT
//
// ***************************************************************************
//
// FILE
//      $Id$
// HISTORY
//      $Log$
// ***************************************************************************

#ifndef __HALPLATFORMDEFS_H__
#define __HALPLATFORMDEFS_H__

#include <stm32f7xx_hal.h>
#include <stm32f7xx_hal_spi.h>
#include <stm32f7xx_ll_gpio.h>
#include <stm32f7xx_ll_rcc.h>
#include <system_stm32f7xx.h>
#include <stm32f7xx_hal_iwdg.h>

#if (defined(HAL_ETH_MODULE_ENABLED))
#include <stm32f7xx_hal_eth.h>
#endif


#if defined(STM32F764xx)
#include <stm32f764xx.h>
#endif

#define PLATFORM_STM32F7XX


#ifdef __cplusplus
extern "C" {
#endif 
void HAL_GetUID(uint32_t *UID);
#ifdef __cplusplus
}
#endif 

typedef GPIO_TypeDef * HALGPIO;

#endif // #ifndef __HALPLATFORMDEFS_H__

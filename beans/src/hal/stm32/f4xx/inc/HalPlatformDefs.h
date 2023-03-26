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

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"
#include "stm32f4xx_ll_gpio.h"
#include <stm32f4xx_ll_rcc.h>
#include <system_stm32f4xx.h>

#if (defined(HAL_ETH_MODULE_ENABLED))
#include <stm32f4xx_hal_eth.h>
#endif


#if defined(STM32F407xx)
#include <stm32f407xx.h>
#endif

#define PLATFORM_STM32F4XX

typedef GPIO_TypeDef * HALGPIO;

#endif // #ifndef __HALPLATFORMDEFS_H__

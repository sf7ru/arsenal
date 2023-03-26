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

#include <stm32l4xx_hal.h>
#include <stm32l4xx_hal_spi.h>
#include <stm32l4xx_ll_gpio.h>
#include <stm32l4xx_hal_iwdg.h>
#include <stm32l4xx_hal_gpio.h>
#include <stm32l4xx_hal_tim.h>
#include <stm32l4xx_ll_rcc.h>
#include <stm32l4xx_hal_tim.h>

#include <Legacy/stm32_hal_legacy.h>

#define PLATFORM_STM32L4XX

typedef GPIO_TypeDef * HALGPIO;

#endif // #ifndef __HALPLATFORMDEFS_H__

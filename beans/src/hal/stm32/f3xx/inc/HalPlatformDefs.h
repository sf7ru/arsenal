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

#include "stm32f3xx_hal.h"
#include "stm32f3xx_ll_gpio.h"

#define PLATFORM_STM32F3XX

#define BSRR_HIGH_AND_LOW

typedef GPIO_TypeDef * HALGPIO;

#endif // #ifndef __HALPLATFORMDEFS_H__

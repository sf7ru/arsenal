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

#include <stm32f1xx_hal.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_hal_iwdg.h>
#include <stm32f1xx_hal_spi.h>
#include <stm32f1xx_hal_i2c.h>

typedef GPIO_TypeDef * HALGPIO;

#define PLATFORM_STM32F1XX

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------


#ifdef __cplusplus
extern "C" {
#endif

void                retarget                    (long               baudrate);


#ifdef __cplusplus
}
#endif

#endif // #ifndef __HALPLATFORMDEFS_H__

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

#include <arsenal.h>
#include <axtime.h>

#include <customboard.h>

#include <HalPlatformDefs.h>

#include <Usb.h>

void usbSetDiscovery(BOOL value)
{
//#ifdef CBRD_USB_DISC
//    Hal & hal = Hal::getInstance();
//
//    hal.gpio.setMode(CBRD_USB_DISC);
//
//    hal.gpio.setPin(CBRD_USB_DISC, value);
//#else
//#if defined(STM32F4)
//    GPIO_InitTypeDef GPIO_InitStruct;
//
//    if (value)
//    {
///**USB GPIO Configuration
//    PA11     ------> USB_DM
//    PA12     ------> USB_DP
//    */
//        GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
//        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//        GPIO_InitStruct.Pull = GPIO_NOPULL;
//        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//        GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
//        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//    }
//    else
//    {
//        GPIO_InitStruct.Pin = GPIO_PIN_12;
//        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//        GPIO_InitStruct.Pull = GPIO_NOPULL;
//        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
//    }
//#endif
//
//#endif
}

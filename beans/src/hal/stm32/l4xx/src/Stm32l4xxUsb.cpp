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


#include <Hal.h>
#include <Spi.h>
#include <SpiDevice.h>
#include <string.h>

#include <app.h>

#include <customboard.h>

#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_pcd.h"

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

extern "C"
{
PCD_HandleTypeDef hpcd_USB_FS;
}


// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------




//#define CBRD_USB_DSCVR_GPIO      GPIOB
//#define CBRD_USB_DSCVR_SEL_BIT   GPIO_PIN_7
//#define CBRD_USB_DSCVR_ONE       0

/**
  * @brief Connect device (enable internal pull-up)
  * @param  pdev: device instance
  * @retval : None
  */
void  USB_DevConnect (void /*USB_CORE_HANDLE *pdev*/)
{


  /* Enabling DP Pull-Down bit to Connect internal pull-up on USB DP line */
  HAL_PCD_DevConnect(&hpcd_USB_FS);
  /*Device is in default state*/
//  pdev->dev.device_status  = USB_DEFAULT;
}

/**
  * @brief Disconnect device (disable internal pull-up)
  * @param  pdev: device instance
  * @retval : None
  */
void  USB_DevDisconnect (void /*USB_CORE_HANDLE *pdev*/)
{
 
  /* Disable DP Pull-Down bit*/
  HAL_PCD_DevDisconnect(&hpcd_USB_FS);
  
  /*Device is in unconnected state*/
//  pdev->dev.device_status  = USB_UNCONNECTED;
}


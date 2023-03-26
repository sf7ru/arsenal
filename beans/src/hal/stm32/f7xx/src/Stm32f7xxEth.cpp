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


#include <stm32f7xx_hal.h>

#ifdef HAL_ETH_MODULE_ENABLED

#include <stm32f7xx_hal_eth.h>
#include <Eth.h>

#include <customboard.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#ifndef CBRD_ETH_RMII_TXD1_Pin
  #define CBRD_ETH_RMII_TXD1_Pin          GPIO_PIN_13
  #define CBRD_ETH_RMII_TXD1_GPIO_Port    GPIOB
  #define CBRD_ETH_RMII_MDC_Pin           GPIO_PIN_1
  #define CBRD_ETH_RMII_MDC_GPIO_Port     GPIOC
  #define CBRD_ETH_RMII_RXD0_Pin          GPIO_PIN_4
  #define CBRD_ETH_RMII_RXD0_GPIO_Port    GPIOC
  #define CBRD_ETH_RMII_RXD1_Pin          GPIO_PIN_5
  #define CBRD_ETH_RMII_RXD1_GPIO_Port    GPIOC
  #define CBRD_ETH_RMII_REF_CLK_Pin       GPIO_PIN_1
  #define CBRD_ETH_RMII_REF_CLK_GPIO_Port GPIOA
  #define CBRD_ETH_RMII_MDIO_Pin          GPIO_PIN_2
  #define CBRD_ETH_RMII_MDIO_GPIO_Port    GPIOA
  #define CBRD_ETH_RMII_CRS_DV_Pin        GPIO_PIN_7
  #define CBRD_ETH_RMII_CRS_DV_GPIO_Port  GPIOA
  #define CBRD_ETH_RMII_TX_EN_Pin         GPIO_PIN_11
  #define CBRD_ETH_RMII_TX_EN_GPIO_Port   GPIOG
  #define CBRD_ETH_RMII_TXD0_Pin          GPIO_PIN_13
  #define CBRD_ETH_RMII_TXD0_GPIO_Port    GPIOG
#endif

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

extern "C"
{
void HAL_ETH_MspInit(ETH_HandleTypeDef* ethHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  //if(ethHandle->Instance==ETH)
  {
  /* USER CODE BEGIN ETH_MspInit 0 */

  /* USER CODE END ETH_MspInit 0 */
    /* Enable Peripheral clock */
    __HAL_RCC_ETH_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    /**ETH GPIO Configuration
    PC1     ------> ETH_MDC
    PA1     ------> ETH_REF_CLK
    PA2     ------> ETH_MDIO
    PA7     ------> ETH_CRS_DV
    PC4     ------> ETH_RXD0
    PC5     ------> ETH_RXD1
    PB13     ------> ETH_TXD1
    PG11     ------> ETH_TX_EN
    PG13     ------> ETH_TXD0
    */
    GPIO_InitStruct.Pin = CBRD_ETH_RMII_MDC_Pin|CBRD_ETH_RMII_RXD0_Pin|CBRD_ETH_RMII_RXD1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CBRD_ETH_RMII_REF_CLK_Pin|CBRD_ETH_RMII_MDIO_Pin|CBRD_ETH_RMII_CRS_DV_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CBRD_ETH_RMII_TXD1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(CBRD_ETH_RMII_TXD1_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CBRD_ETH_RMII_TX_EN_Pin|CBRD_ETH_RMII_TXD0_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    /* Peripheral interrupt init */
    HAL_NVIC_SetPriority(ETH_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(ETH_IRQn);
  /* USER CODE BEGIN ETH_MspInit 1 */

  /* USER CODE END ETH_MspInit 1 */
  }
}
}

BOOL Eth::initMsp()
{
    BOOL                result              = true;

    ENTER(true);

    HAL_ETH_MspInit(nil);

    RETURN(result);
}

#endif // #ifdef HAL_ETH_MODULE_ENABLED
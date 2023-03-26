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


#include <stm32f4xx_hal.h>

#ifdef HAL_ETH_MODULE_ENABLED

#include <stm32f4xx_hal_eth.h>
#include <Eth.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

extern "C"
{
void HAL_ETH_MspInit(ETH_HandleTypeDef *ethHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    //    if(ethHandle->Instance==ETH)
    {
        /* USER CODE BEGIN ETH_MspInit 0 */


        __HAL_RCC_GPIOE_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        /* USER CODE END ETH_MspInit 0 */
        /* Enable Peripheral clock */
        __HAL_RCC_ETH_CLK_ENABLE();
        /**ETH GPIO Configuration
        PE2     ------> ETH_TXD3
        PC1     ------> ETH_MDC
        PC2     ------> ETH_TXD2
        PC3     ------> ETH_TX_CLK
        PA0-WKUP     ------> ETH_CRS
        PA1     ------> ETH_RX_CLK
        PA2     ------> ETH_MDIO
        PA3     ------> ETH_COL
        PA7     ------> ETH_RX_DV
        PC4     ------> ETH_RXD0
        PC5     ------> ETH_RXD1
        PB0     ------> ETH_RXD2
        PB1     ------> ETH_RXD3
        PB10     ------> ETH_RX_ER
        PB11     ------> ETH_TX_EN
        PB12     ------> ETH_TXD0
        PB13     ------> ETH_TXD1
        */
        GPIO_InitStruct.Pin       = GPIO_PIN_2;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_NOPULL;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

        GPIO_InitStruct.Pin       = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4
                                    | GPIO_PIN_5;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_NOPULL;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin       = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3
                                    | GPIO_PIN_7;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_NOPULL;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin       = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_10 | GPIO_PIN_11
                                    | GPIO_PIN_12 | GPIO_PIN_13;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_NOPULL;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
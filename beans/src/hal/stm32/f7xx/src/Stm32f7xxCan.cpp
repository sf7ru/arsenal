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

#ifdef HAL_CAN_MODULE_ENABLED

#include <stm32f7xx_hal_can.h>
#include <Can.h>

#include <customboard.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------


// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

// CAN_RxHeaderTypeDef pRxHeader; //declare header for message reception
// uint32_t TxMailbox;
// uint8_t a,r; //declare byte to be transmitted //declare a receive byte

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------


extern "C"
{
    void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
    {
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        if(hcan->Instance==CAN1)
        {
        /* USER CODE BEGIN CAN1_MspInit 0 */

        /* USER CODE END CAN1_MspInit 0 */
            /* Peripheral clock enable */
            __HAL_RCC_CAN1_CLK_ENABLE();

            __HAL_RCC_GPIOB_CLK_ENABLE();
            /**CAN1 GPIO Configuration
            PB8     ------> CAN1_RX
            PB9     ------> CAN1_TX
            */
            GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
            GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
            HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

            /* CAN1 interrupt Init */
            HAL_NVIC_SetPriority(CAN1_TX_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
            HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
            // HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 0, 0);
            // HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
        }
    }
};
BOOL Can::initMsp(PVOID         h)
{
    BOOL                result              = true;

    ENTER(true);

    HAL_CAN_MspInit((CAN_HandleTypeDef*)h);

    RETURN(result);
}


#endif // #ifdef HAL_CAN_MODULE_ENABLED
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

#include <I2c.h>

#include <I2cDevice.h>

#include <customboard.h>
#include <string.h>
#include <stdint.h>
#include <axutils.h>

#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_i2c.h"

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

extern  BOOL                hi2csReady      [ I2C_MAX_IFACES ];
extern  I2C_HandleTypeDef   hi2cs           [ I2C_MAX_IFACES ];

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      I2c::init
// PURPOSE
//
// PARAMETERS
//      I2cDevice *              dev --
//      UINT           ifaceNo = 0   --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL I2c::init(UINT           speed)
{
    BOOL                        result          = false;
    I2C_HandleTypeDef *         hi2c;
    RCC_PeriphCLKInitTypeDef    PeriphClkInit;
    GPIO_InitTypeDef            GPIO_InitStruct;

    ENTER(true);

    ifaceNo         = getSingletoneIndex();
    hi2c            = &hi2cs[ ifaceNo ];

    if (ifaceNo < I2C_MAX_IFACES)
    {
        if (!hi2csReady[ifaceNo])
        {
            switch (ifaceNo)
            {
                case 0:
                    __HAL_RCC_GPIOB_CLK_ENABLE();

                    hi2c->Instance                     = I2C1;
                    //PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;

                    /**I2C1 GPIO Configuration
                     PB8     ------> I2C1_SCL
                     PB9     ------> I2C1_SDA
                     */
                    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
                    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
                    GPIO_InitStruct.Pull = GPIO_PULLUP;
                    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
                    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
                    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
                    /* Peripheral clock enable */
                    __HAL_RCC_I2C1_CLK_ENABLE();
                    HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0, 1);
                    HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
                    HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 2);
                    HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);

                    result = true;
                    break;

                case 1:
                    __HAL_RCC_GPIOB_CLK_ENABLE();

                    hi2c->Instance                     = I2C2;
                    //PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C2;

                    //                PB10     ------> I2C2_SCL
                    //                PB11     ------> I2C2_SDA

                    GPIO_InitStruct.Pin       = GPIO_PIN_10 | GPIO_PIN_11;
                    GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
                    GPIO_InitStruct.Pull      = GPIO_PULLUP;
                    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
                    GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
                    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

                    /* Peripheral clock enable */
                    __HAL_RCC_I2C2_CLK_ENABLE();
                    HAL_NVIC_SetPriority(I2C2_ER_IRQn, 0, 1);
                    HAL_NVIC_EnableIRQ(I2C2_ER_IRQn);
                    HAL_NVIC_SetPriority(I2C2_EV_IRQn, 0, 2);
                    HAL_NVIC_EnableIRQ(I2C2_EV_IRQn);
                    break;

                case 2:
                    __HAL_RCC_GPIOA_CLK_ENABLE();
                    __HAL_RCC_GPIOC_CLK_ENABLE();

                    hi2c->Instance                     = I2C3;
                    //PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C3;

                    //                PA8     ------> I2C3_SCL
                    //                PC9     ------> I2C3_SDA

                    GPIO_InitStruct.Pin       = GPIO_PIN_8;
                    GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
                    GPIO_InitStruct.Pull      = GPIO_PULLUP;
                    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
                    GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
                    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

                    GPIO_InitStruct.Pin       = GPIO_PIN_9;
                    GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
                    GPIO_InitStruct.Pull      = GPIO_PULLUP;
                    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
                    GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
                    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

                    /* Peripheral clock enable */
                    __HAL_RCC_I2C3_CLK_ENABLE();
                    HAL_NVIC_SetPriority(I2C3_ER_IRQn, 0, 1);
                    HAL_NVIC_EnableIRQ(I2C3_ER_IRQn);
                    HAL_NVIC_SetPriority(I2C3_EV_IRQn, 0, 2);
                    HAL_NVIC_EnableIRQ(I2C3_EV_IRQn);
                    break;

                default:
//                    hi2c->Instance                     = I2C1;
//                    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
                    break;
            }

//            hi2c->Init.ClockSpeed = speed;
//            hi2c->Init.DutyCycle = I2C_DUTYCYCLE_2;
//            hi2c->Init.OwnAddress1 = 0;
//            hi2c->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
//            hi2c->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
//            hi2c->Init.OwnAddress2 = 0;
//            hi2c->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
//            hi2c->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
            

            if (HAL_I2C_Init(hi2c) == HAL_OK)
            {
                // Configure Analogue filter


                hi2csReady[ifaceNo] = true;
                result = true;
            }
        }
        else
            result = true;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      I2c::deinit
// PURPOSE
//
// PARAMETERS
//      UINT ifaceNo --
// RESULT
//      void --
// ***************************************************************************
void I2c::deinit()
{
    ENTER(true);

    QUIT;
}

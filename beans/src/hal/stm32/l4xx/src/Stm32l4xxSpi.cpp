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

#include <Spi.h>
#include <stm32l4xx_hal.h>

#include <customboard.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

BOOL Spi::initMsp()
{
    BOOL                result              = false;
    GPIO_InitTypeDef    GPIO_InitStruct;

    ENTER(true);

    switch (ifaceNo)
    {
        case 0:
            break;

#ifdef CBRD_SPI_IDX_1_PINSET
        case 1:
            __HAL_RCC_SPI2_CLK_ENABLE();

            switch (CBRD_SPI_IDX_1_PINSET)
            {
                case 0:
                    __HAL_RCC_GPIOB_CLK_ENABLE();

                    /**SPI2 GPIO Configuration
                    PB12     ------> SPI2_NSS
                    PB13     ------> SPI2_SCK
                    PB14     ------> SPI2_MISO
                    PB15     ------> SPI2_MOSI
                    */
                    if (mode.master)
                    {
                        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                        GPIO_InitStruct.Pull = GPIO_NOPULL;
                        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
                        GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;

                        GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15|
                                              (mode.softSelect ? 0 : GPIO_PIN_12);

                        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
                    }
                    else
                    {
//                        printf("Stm32l4xxSpi.cpp:63 stage new right way init\n");

                        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                        GPIO_InitStruct.Pull = GPIO_NOPULL;
                        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
                        GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;

                        GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;

                        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


//                        GPIO_InitStruct.Pin       = GPIO_PIN_14;
//                        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
//                        GPIO_InitStruct.Pull      = GPIO_PULLUP;
//                        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
//                        GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
//
//                        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
//                        GPIO_InitStruct.Pin  = GPIO_PIN_13 | GPIO_PIN_15;
//                        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
//                        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
//                        GPIO_InitStruct.Pin  = GPIO_PIN_12;
//                        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
//                        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
                    }


                    result = true;
                    break;

                default:
                    break;
            }
            break;
#endif // #ifdef CBRD_SPI_IDX_1_PINSET

        default:
            break;
    }

    RETURN(result);
}

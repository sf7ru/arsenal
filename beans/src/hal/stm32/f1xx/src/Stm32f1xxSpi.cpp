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
#include <stm32f1xx_hal.h>

#include <customboard.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

BOOL Spi::initMsp()
{
    BOOL                result              = false;
    GPIO_InitTypeDef    GPIO_InitStruct     = { 0 };

    ENTER(true);

    switch (ifaceNo)
    {
#ifdef CBRD_SPI_IDX_0_PINSET
        case 0:
            __HAL_RCC_SPI1_CLK_ENABLE();

            switch (CBRD_SPI_IDX_0_PINSET)
            {
                case 0:
                    __HAL_RCC_GPIOA_CLK_ENABLE();

                    /**SPI1 GPIO Configuration
                    PA5     ------> SPI1_SCK
                    PA6     ------> SPI1_MISO
                    PA7     ------> SPI1_MOSI
                    */
                    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_7;
                    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
                    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

                    GPIO_InitStruct.Pin = GPIO_PIN_6;
                    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
                    GPIO_InitStruct.Pull = GPIO_NOPULL;
                    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//                    if (mode.master)
//                    {
//                        GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|
//                                              (mode.softSelect ? 0 : GPIO_PIN_4);
//                    }
//                    else
//                    {
//                        GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
//                    }
//
//                    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

                    result = true;
                    break;

                default:
                    break;
            }
            break;
#endif // #ifdef CBRD_SPI_IDX_1_PINSET

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
                        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
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

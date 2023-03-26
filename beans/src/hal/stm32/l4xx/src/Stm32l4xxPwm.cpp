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


#include <HalPlatformDefs.h>

#ifdef HAL_TIM_MODULE_ENABLED

#include <Pwm.h>
#include <customboard.h>
#include <stdint.h>




BOOL Pwm::enableChannel(UINT channel)
{
    BOOL                result              = false;
    GPIO_InitTypeDef    GPIO_InitStruct;

    ENTER(true);

    switch (channel)
    {
#ifdef  CBRD_PWM_CH1_LEG
        case 1:
            if (CBRD_PWM_CH1_LEG == 63)
            {
                __HAL_RCC_GPIOC_CLK_ENABLE();

                GPIO_InitStruct.Pin = GPIO_PIN_6;
                GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                GPIO_InitStruct.Pull = GPIO_PULLUP;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
                GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
                HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

                result = true;
            }
            break;
#endif

#ifdef  CBRD_PWM_CH2_LEG
        case 2:
            if (CBRD_PWM_CH2_LEG == 64)
            {
                __HAL_RCC_GPIOC_CLK_ENABLE();

                GPIO_InitStruct.Pin = GPIO_PIN_7;
                GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                GPIO_InitStruct.Pull = GPIO_PULLUP;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
                GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
                HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

                result = true;
            }
            break;
#endif

#ifdef  CBRD_PWM_CH3_LEG
        case 3:
            if (CBRD_PWM_CH3_LEG == 65)
            {
                __HAL_RCC_GPIOC_CLK_ENABLE();

                GPIO_InitStruct.Pin = GPIO_PIN_8;
                GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                GPIO_InitStruct.Pull = GPIO_PULLUP;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
                GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
                HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
            }
            break;
#endif

        default:
            break;
    }

    RETURN(result);
}


#endif // HAL_TIM_MODULE_ENABLED


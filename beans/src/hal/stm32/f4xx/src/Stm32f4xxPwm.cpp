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
#include <stm32f4xx_hal_tim.h>
#include <stm32pins.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#ifndef CBRD_PWM_TIMER
#define CBRD_PWM_TIMER          3
#endif


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#if (CBRD_PWM_TIMER == 3)

BOOL Pwm::enableChannel(UINT channel)
{
    BOOL                result              = false;
    GPIO_InitTypeDef    GPIO_InitStruct;

    ENTER(true);

    switch (channel)
    {
#ifdef  CBRD_PWM_CH1_LEG
        case 1:
            if (CBRD_PWM_CH1_LEG == PP_INDEX(PA6))
            {
                __HAL_RCC_GPIOA_CLK_ENABLE();

                /**TIM3 GPIO Configuration
                PA6     ------> TIM3_CH1
                */
                GPIO_InitStruct.Pin = GPIO_PIN_6;
                GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                GPIO_InitStruct.Pull = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
                HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

                result = true;
            }
            else if (CBRD_PWM_CH1_LEG == PP_INDEX(PB4))
            {
                __HAL_RCC_GPIOB_CLK_ENABLE();

                /**TIM3 GPIO Configuration
                PB4     ------> TIM3_CH1
                */
                GPIO_InitStruct.Pin = GPIO_PIN_4;
                GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                GPIO_InitStruct.Pull = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
                HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

                result = true;
            }
            else if (CBRD_PWM_CH1_LEG == PP_INDEX(PC6))
            {
                __HAL_RCC_GPIOC_CLK_ENABLE();

                /**TIM3 GPIO Configuration
                PC6     ------> TIM3_CH1
                */
                GPIO_InitStruct.Pin = GPIO_PIN_6;
                GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                GPIO_InitStruct.Pull = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
                HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

                result = true;
            }

            break;
#endif

#ifdef  CBRD_PWM_CH2_LEG
        case 2:
            if (CBRD_PWM_CH2_LEG == PP_INDEX(PA7))
            {
                __HAL_RCC_GPIOA_CLK_ENABLE();

                /**TIM3 GPIO Configuration
                PA7     ------> TIM3_CH2
                */
                GPIO_InitStruct.Pin = GPIO_PIN_7;
                GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                GPIO_InitStruct.Pull = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
                HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

                result = true;
            }
            else if (CBRD_PWM_CH2_LEG == PP_INDEX(PB5))
            {
                __HAL_RCC_GPIOB_CLK_ENABLE();

                /**TIM3 GPIO Configuration
                PB5     ------> TIM3_CH2
                */
                GPIO_InitStruct.Pin = GPIO_PIN_5;
                GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                GPIO_InitStruct.Pull = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
                HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

                result = true;
            }
            else if (CBRD_PWM_CH2_LEG == PP_INDEX(PC7))
            {
                __HAL_RCC_GPIOC_CLK_ENABLE();

                /**TIM3 GPIO Configuration
                PC7     ------> TIM3_CH2
                */
                GPIO_InitStruct.Pin = GPIO_PIN_7;
                GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                GPIO_InitStruct.Pull = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
                HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

                result = true;
            }
            break;
#endif

#ifdef  CBRD_PWM_CH3_LEG
        case 3:
            if (CBRD_PWM_CH3_LEG == PP_INDEX(PB0))
            {
                __HAL_RCC_GPIOB_CLK_ENABLE();

                /**TIM3 GPIO Configuration
                PB0     ------> TIM3_CH3
                */
                GPIO_InitStruct.Pin = GPIO_PIN_0;
                GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                GPIO_InitStruct.Pull = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
                HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

                result = true;
            }
            else if (CBRD_PWM_CH3_LEG == PP_INDEX(PC8))
            {
                __HAL_RCC_GPIOC_CLK_ENABLE();

                /**TIM3 GPIO Configuration
                PC8     ------> TIM3_CH3
                */
                GPIO_InitStruct.Pin = GPIO_PIN_8;
                GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                GPIO_InitStruct.Pull = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
                HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

                result = true;
            }
            break;
#endif

#ifdef  CBRD_PWM_CH4_LEG
        case 3:
            if (CBRD_PWM_CH4_LEG == PP_INDEX(PB1))
            {
                __HAL_RCC_GPIOB_CLK_ENABLE();

                /**TIM3 GPIO Configuration
                PB1     ------> TIM3_CH4
                */
                GPIO_InitStruct.Pin = GPIO_PIN_1;
                GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                GPIO_InitStruct.Pull = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
                HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

                result = true;
            }
            else if (CBRD_PWM_CH4_LEG == PP_INDEX(PC9))
            {
                __HAL_RCC_GPIOC_CLK_ENABLE();

                /**TIM3 GPIO Configuration
                PC9     ------> TIM3_CH4
                */
                GPIO_InitStruct.Pin = GPIO_PIN_9;
                GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                GPIO_InitStruct.Pull = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
                HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

                result = true;
            }
            break;
#endif

        default:
            break;
    }

    RETURN(result);
}

#elif (CBRD_PWM_TIMER == 8)

BOOL Pwm::enableChannel(UINT channel)
{
    BOOL                result              = false;
    GPIO_InitTypeDef    GPIO_InitStruct;

    ENTER(true);

    switch (channel)
    {
#ifdef  CBRD_PWM_CH1_LEG
        case 1:
            if (CBRD_PWM_CH1_LEG == PP_INDEX(PC6))
            {
                __HAL_RCC_GPIOC_CLK_ENABLE();

                /**TIM3 GPIO Configuration
                PC6     ------> TIM8_CH1
                */
                GPIO_InitStruct.Pin = GPIO_PIN_6;
                GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                GPIO_InitStruct.Pull = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
                HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

                result = true;
            }

            break;
#endif

#ifdef  CBRD_PWM_CH2_LEG
        case 2:
            if (CBRD_PWM_CH2_LEG == PP_INDEX(PC7))
            {
                __HAL_RCC_GPIOC_CLK_ENABLE();

                /**TIM3 GPIO Configuration
                PC7     ------> TIM8_CH2
                */
                GPIO_InitStruct.Pin = GPIO_PIN_7;
                GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                GPIO_InitStruct.Pull = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
                HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

                result = true;
            }
            break;
#endif

#ifdef  CBRD_PWM_CH3_LEG
        case 3:
            if (CBRD_PWM_CH3_LEG == PP_INDEX(PC8))
            {
                __HAL_RCC_GPIOC_CLK_ENABLE();

                /**TIM3 GPIO Configuration
                PC8     ------> TIM8_CH3
                */
                GPIO_InitStruct.Pin = GPIO_PIN_8;
                GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                GPIO_InitStruct.Pull = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
                HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

                result = true;
            }
            break;
#endif

#ifdef  CBRD_PWM_CH4_LEG
        case 4:
            if (CBRD_PWM_CH4_LEG == PP_INDEX(PC9))
            {
                __HAL_RCC_GPIOC_CLK_ENABLE();

                /**TIM3 GPIO Configuration
                PC9     ------> TIM8_CH4
                */
                GPIO_InitStruct.Pin = GPIO_PIN_9;
                GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                GPIO_InitStruct.Pull = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
                HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

                result = true;
            }
            break;
#endif

        default:
            break;
    }

    RETURN(result);
}
#elif (CBRD_PWM_TIMER == 1)

BOOL Pwm::enableChannel(UINT channel)
{
    BOOL                result              = false;
    GPIO_InitTypeDef    GPIO_InitStruct;
    int index;

    ENTER(true);

    switch (channel)
    {
#ifdef  CBRD_PWM_CH1_LEG
        case 1:
            index = PP_INDEX(PE9);
            if (CBRD_PWM_CH1_LEG == index)
            {
                __HAL_RCC_GPIOE_CLK_ENABLE();
                /**TIM1 GPIO Configuration
                PE9     ------> TIM1_CH1
                */
                GPIO_InitStruct.Pin = GPIO_PIN_9;
                GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                GPIO_InitStruct.Pull = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
                HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

                result = true;
            }

            break;
#endif


        default:
            break;
    }

    RETURN(result);
}

#endif // #if (CBRD_PWM_TIMER == 3)

#endif // HAL_TIM_MODULE_ENABLED


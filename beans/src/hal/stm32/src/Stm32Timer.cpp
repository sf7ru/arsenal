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
#include <stdio.h>

#include <stm32pins.h>

#include <HalPlatformDefs.h>
#include <customboard.h>

#if (defined(HAL_TIM_MODULE_ENABLED) && !defined(CBRD_USE_NO_TIMERS))

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define TOTAL_TIMERS        2

#define SBIT_MR0I    0
#define SBIT_MR0R    1
#define PCLK_TIMER1  0, 4
#define PCLK_TIMER2  1, 12
#define PCLK_TIMER3  1, 14
#define MiliToMicroSec(x)  (x*1000)  /* ms is multiplied by 1000 to get us*/
#define SBIT_CNTEN   0

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef  struct __tag_TIMERCTRL
{
    PFNTIMERCB          callback;
    PVOID               param;
    TIM_HandleTypeDef   def;
} TIMERCTRL, * PTIMERCTRL;

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------


static  int                 next    = 1;
static  TIMERCTRL           timers  [ TOTAL_TIMERS ] = { 0 };


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------


#define PROCESS_TIMER(a) { HAL_TIM_IRQHandler(&timers[a].def); if (timers[a].callback) { (*timers[a].callback)(timers[a].param); } }

/*

#define PROCESS_TIMER(a) { HAL_TIM_IRQHandler(&timers[a].def);      \
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);         \
        if (timers[a].callback) { (*timers[a].callback)(timers[a].param); } \
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);                   \
        }
*/

#if defined(PLATFORM_STM32F4XX) || defined(PLATFORM_STM32F7XX)
extern "C" void TIM6_DAC_IRQHandler(void)
#else
extern "C" void TIM6_IRQHandler(void)
#endif
{
    PROCESS_TIMER(0);
}

extern "C" void TIM7_IRQHandler()
{
    PROCESS_TIMER(1);
}

HAXHANDLE Hal::Timer::set(UINT           periodUs,
                          PFNTIMERCB     callback,
                          PVOID          param,
                          INT            priority)
{
    PTIMERCTRL  result  = nil;
    TIM_HandleTypeDef * def;
    TIM_ClockConfigTypeDef sClockSourceConfig;
    TIM_MasterConfigTypeDef sMasterConfig;

    U32 PCLK2_freq = __LL_RCC_CALC_PCLK2_FREQ(SystemCoreClock, LL_RCC_GetAPB2Prescaler());


    if (next <= TOTAL_TIMERS)
    {
        switch (next)
        {
            case 1:
#ifdef __HAL_RCC_TIM6_CLK_ENABLE
                result  = &timers[next - 1];
                def     = &result->def;

                //Hal::getInstance().gpio.setMode(PP_PIN(PP_OUT, PA4), HAL_SETMODE_FLAG_HIGH);

                __HAL_RCC_TIM6_CLK_ENABLE();

                def->Instance = TIM6;
                //def->Init.Prescaler = (PCLK2_freq / 1000000) - 1;
                def->Init.Prescaler = (PCLK2_freq / 1000000);
                def->Init.CounterMode = TIM_COUNTERMODE_UP;
                def->Init.Period = periodUs - 1; // periodUs *
                def->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
#ifdef TIM_AUTORELOAD_PRELOAD_DISABLE
                def->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
#endif
                def->Init.RepetitionCounter = 0;

                HAL_TIM_Base_Init(def);
                __HAL_TIM_SetCounter(def, 0);

                sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
                HAL_TIM_ConfigClockSource(def, &sClockSourceConfig);

                sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
                sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
                HAL_TIMEx_MasterConfigSynchronization(def, &sMasterConfig);

#if defined(PLATFORM_STM32F4XX) || defined(PLATFORM_STM32F7XX)
                HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 5, 0);
                HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
#else
                HAL_NVIC_SetPriority(TIM6_IRQn, 5, 0);
                HAL_NVIC_EnableIRQ(TIM6_IRQn);
#endif
                HAL_TIM_Base_Start_IT(def);

#endif // #ifdef __HAL_RCC_TIM6_CLK_ENABLE
                break;

            case 2:
#ifdef __HAL_RCC_TIM7_CLK_ENABLE            
                result  = &timers[next - 1];
                def     = &result->def;

                __HAL_RCC_TIM7_CLK_ENABLE();

                def->Instance = TIM7;
                def->Init.Prescaler = 40000;
                def->Init.CounterMode = TIM_COUNTERMODE_UP;
                def->Init.Period = periodUs;
                def->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
                def->Init.RepetitionCounter = 0;
                HAL_TIM_Base_Init(def);
                HAL_TIM_Base_Start(def);

                HAL_NVIC_SetPriority(TIM7_IRQn, 5, 0);
                HAL_NVIC_EnableIRQ(TIM7_IRQn);
#endif //  __HAL_RCC_TIM7_CLK_ENABLE                            
                break;

            default:
                break;
        }

        if (result != nil)
        {
            result->param       = param;
            result->callback    = callback;

        }
    }

    return (HAXHANDLE)result;
}
#endif // #ifdef HAL_TIM_MODULE_ENABLED
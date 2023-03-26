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


#include <Fm.h>

//#include <Pwm.h>
#include <customboard.h>
#include <stdint.h>
#include <stm32f4xx_hal_tim.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define CHECK_CH(a) ((channel > 0) && (channel <= channels))


#define HARDCODED_FREQ_BASE 10

#ifndef STM32_PWM_TIM
#define STM32_PWM_TIM       12
#endif

#define ___HAL_RCC_CLK_ENABLE(a)     __HAL_RCC_TIM##a##_CLK_ENABLE()
#define __HAL_RCC_CLK_ENABLE(a)     ___HAL_RCC_CLK_ENABLE(a)

#define _TIM_Instance(a)             TIM##a
#define TIM_Instance(a)             _TIM_Instance(a)

#define _TIM_IRQn(a)                TIM##a##_IRQn
#define TIM_IRQn(a)                 _TIM_IRQn(a)

#define STM32_FM_CHS      4


// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

static  volatile  FMCHANNEL chCtrl[ STM32_FM_CHS ]    = { 0 };

static  TIM_HandleTypeDef htim3;


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

extern "C"
{
    void TIM8_BRK_TIM12_IRQHandler(void)
    {
        HAL_TIM_IRQHandler(&htim3);

        Fm & fm = Fm::getInstance();
        fm.timerCb(&fm);
    }
}


// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      Fm::Fm
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
Fm::Fm()
{
    baseFreq    = 0;
    channels    = 0;
    inited      = false;
}
void  Fm::compensateDrift(volatile FMCHANNEL *   cc,
                          INT                    period)
{
//    Fm & fm = Fm::getInstance();
//
//    INT periodCount         = period / fm.baseFreq;
//
//    //periodCount              = (UINT)(fm.baseFreqKoef * periodCount);
//
//    if (periodCount < 2)
//        periodCount          = 2;
//
//    cc->zeroMultiplier = periodCount - cc->oneMultiplier;
//    cc->oneMultiplier  = (cc->oneDuration / baseFreq);
}
// ***************************************************************************
// FUNCTION
//      Fm::init
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL Fm::init()
{
    BOOL            result          = false;

    ENTER(true);

    TIM_ClockConfigTypeDef sClockSourceConfig;
    TIM_MasterConfigTypeDef sMasterConfig;

    htim3.Instance = TIM_Instance(STM32_PWM_TIM);
    htim3.Init.Prescaler = 0;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 100;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    if (HAL_TIM_Base_Init(&htim3) == HAL_OK)
    {
        __HAL_RCC_CLK_ENABLE(STM32_PWM_TIM);
        HAL_NVIC_SetPriority(TIM8_BRK_TIM12_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(TIM8_BRK_TIM12_IRQn);

        sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
        if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) == HAL_OK)
        {
            if (HAL_TIM_PWM_Init(&htim3) == HAL_OK)
            {
                sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
                sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;

                if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) == HAL_OK)
                {
                    result = true;
                }
            }
        }
    }

    if (result)
    {
//    baseFreqKoef    = HARDCODED_FREQ_KOEF;
        setBase(HARDCODED_FREQ_BASE);

//    result = true;

    }


    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      Fm::setChannel
// PURPOSE
//
// PARAMETERS
//      UINT channel --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
UINT Fm::setChannel(PORTPIN        pp,
                    UINT           oneDuration,
                    PFNPWMINTCB    intCbPtr,
                    PVOID          intCbParam)
{
    UINT                channel         = 0;
    Hal &               hal             = Hal::getInstance();
    GPIO_InitTypeDef    GPIO_InitStruct;

    ENTER(true);

    switch (PP_INDEX_PP(pp))
    {
        case PP_INDEX_N(1, 14):
            __HAL_RCC_GPIOB_CLK_ENABLE();

            GPIO_InitStruct.Pin = GPIO_PIN_14;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Pull = GPIO_PULLUP;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
            GPIO_InitStruct.Alternate = GPIO_AF9_TIM12;
            HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

            channel = 1;
            break;

        case PP_INDEX_N(1, 15):
            __HAL_RCC_GPIOB_CLK_ENABLE();

            GPIO_InitStruct.Pin = GPIO_PIN_15;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Pull = GPIO_PULLUP;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
            GPIO_InitStruct.Alternate = GPIO_AF9_TIM12;
            HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

            channel = 2;
            break;
    }

//    if (channels < STM32_FM_CHS)
//    {
//        channels++;
//        channel = channels;
//
//        volatile FMCHANNEL *cc  = &chCtrl[channel - 1];
//
//        cc->port        = (GPIO_TypeDef *)globalGpioByIndex[pp.port];
//        cc->pin         = pp.pin;
//        cc->oneLevel    = pp.one;
//
//        hal.gpio.setMode(pp, HAL_SETMODE_FLAG_HIGH);
//
//        SET_SIGNAL(cc, 0);
//
//        cc->intCbPtr        = intCbPtr;
//        cc->intCbParam      = intCbParam;
//
//        if (oneDuration < baseFreq)
//            oneDuration = baseFreq;
//
//        cc->oneDuration         = oneDuration;
//        cc->oneMultiplier       = (oneDuration / baseFreq);
//    }

    RETURN(channel);
}
void Fm::timerCb(Fm *           that)
{
    volatile FMCHANNEL *cc;
    int     newPeriod;

//    for (int idx = 0; idx < STM32_FM_CHS; idx++)
//    {
//        cc = &chCtrl[idx];
//
//        if (!cc->oneMultiplierCount)
//        {
//            // High level phase done
//
//            if (!cc->zeroMultiplierCount || (!(--cc->zeroMultiplierCount)))
//            {
//                // low level phase done
//
//                if (cc->enabled)
//                {
//                    // Getting new period
//
//                    if ((newPeriod = (*cc->intCbPtr)(cc->intCbParam, 0)) > 0)
//                    {
//                        Fm::getInstance().compensateDrift(cc, newPeriod);
//
//                        SET_SIGNAL(cc, 1);
//
//                        cc->oneMultiplierCount = cc->oneMultiplier;
//                    }
//                    else
//                    {
//                        cc->enabled = false;
//                    }
//                }
//            }
//        }
//        else if (!(--cc->oneMultiplierCount))
//        {
//            // High level phase just done, setting signal low
//
//            SET_SIGNAL(cc, 0);
//
//            cc->zeroMultiplierCount = cc->zeroMultiplier;
//        }
//    }
}
// ***************************************************************************
// FUNCTION
//      Fm::outPulse
// PURPOSE
//
// PARAMETERS
//      UINT  channel --
//      float freq    --
// RESULT
//      void --
// ***************************************************************************
void Fm::setBase(UINT periodUs)
{
    if (periodUs != baseFreq)
    {
        baseFreq = periodUs;
        htim3.Init.Prescaler = (SystemCoreClock / 1000000) - 1;
        htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
        htim3.Init.Period = periodUs;
        htim3.Init.RepetitionCounter = 0;
        htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

        if (HAL_TIM_Base_Init(&htim3) == HAL_OK)
        {
            // OK
        }
    }
}
void Fm::stopChannel(UINT           channel)
{
    if (CHECK_CH(channel))
    {
        volatile FMCHANNEL *cc = &chCtrl[channel - 1];


//        ((LPC_GPIO_T*)cc->port)->CLR    = ((uint32_t) 1 << cc->pin);
        cc->enabled                     = false;
    }
}
BOOL Fm::setEnable(UINT           channel,
                   BOOL           enable)
{
    BOOL        result          = false;

    if (CHECK_CH(channel))
    {
        volatile FMCHANNEL * cc = &chCtrl[channel - 1];

        result          = cc->enabled;
        cc->enabled     = enable;
    }

    return result;
}


#endif // #ifdef HAL_TIM_MODULE_ENABLED
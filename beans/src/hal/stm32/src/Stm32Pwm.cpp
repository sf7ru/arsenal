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

#if (defined(HAL_TIM_MODULE_ENABLED) && !defined(CBRD_USE_NO_PWM))

#include <Pwm.h>
#include <customboard.h>
#include <stdint.h>


// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#ifndef CBRD_PWM_TIMER
#define CBRD_PWM_TIMER          3
#endif

#if (CBRD_PWM_TIMER == 1)
#define IRQ_HANDLER         TIM1_IRQHandler
#define TIM_IRQn            TIM1_CC_IRQn
#elif (CBRD_PWM_TIMER == 3)
#define IRQ_HANDLER         TIM3_IRQHandler
#define TIM_IRQn            TIM3_IRQn
#elif (CBRD_PWM_TIMER == 8)
#define IRQ_HANDLER         TIM8_IRQHandler
#define TIM_IRQn            TIM8_IRQn
#else
#error ("No PWM TIMER specified (CBRD_PWM_TIMER)")
#endif

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct __tag_CHANNEL
{
    UINT                    chId;
    UINT                    count;
} CHANNEL, * PCHANNEL;


// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

static BOOL flagPwmIsOver       = false;

static TIM_HandleTypeDef htimClock = { 0 };
static TIM_HandleTypeDef htim = { 0 };
static TIM_OC_InitTypeDef sConfigOC = { 0 };

#define ___HAL_RCC_CLK_ENABLE(a)     __HAL_RCC_TIM##a##_CLK_ENABLE()
#define __HAL_RCC_CLK_ENABLE(a)     ___HAL_RCC_CLK_ENABLE(a)

#define _TIM_Instance(a)             TIM##a
#define TIM_Instance(a)             _TIM_Instance(a)

//#define _TIM_IRQn(a)                TIM##a##_IRQn
//#define TIM_IRQn(a)                 _TIM_IRQn(a)

        CHANNEL             channels    [ PWM_CHANNELS_NUM ] =
            {
                { TIM_CHANNEL_1, 0 },
                { TIM_CHANNEL_2, 0 },
                { TIM_CHANNEL_3, 0 },
                { TIM_CHANNEL_4, 0 },
            };


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

static void channelTimerproc(PCHANNEL c)
{
    HAL_TIM_PWM_Stop(&htim, c->chId);
    flagPwmIsOver = true;
}


extern "C"
{
    void IRQ_HANDLER(void)
    {
        if (__HAL_TIM_GET_FLAG(&htimClock, TIM_FLAG_UPDATE))
        {
            if (__HAL_TIM_GET_ITSTATUS(&htimClock, TIM_IT_UPDATE))
            {
                __HAL_TIM_CLEAR_FLAG(&htimClock, TIM_FLAG_UPDATE);
                channelTimerproc(&channels[0]);
            }
        }
    }
}

Pwm::Pwm()
{

}
BOOL Pwm::init()
{
    BOOL            result          = false;

    ENTER(true);

    //TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
    //TIM_MasterConfigTypeDef sMasterConfig = { 0 };

    htim.Instance = TIM_Instance(CBRD_PWM_TIMER);
    htim.Init.Prescaler = (SystemCoreClock / 1000000) - 1;
    htim.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim.Init.Period = 0;
    htim.Init.RepetitionCounter = 0;
    htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    if (HAL_TIM_Base_Init(&htim) == HAL_OK)
    {
        __HAL_RCC_CLK_ENABLE(CBRD_PWM_TIMER);

//        HAL_NVIC_SetPriority(TIM8_UP_TIM13_IRQn, 5, 0);
//        HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);

//        sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
//        if (HAL_TIM_ConfigClockSource(&htim, &sClockSourceConfig) == HAL_OK)
        {
            if (HAL_TIM_PWM_Init(&htim) == HAL_OK)
            {
//                sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE; // TIM_TRGO_RESET;
//                sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
//
//                if (HAL_TIMEx_MasterConfigSynchronization(&htim, &sMasterConfig) == HAL_OK)
                {
                    result = true;

                    __HAL_RCC_CLK_ENABLE(CBRD_PWM_TIMER);

                    HAL_NVIC_SetPriority(TIM_IRQn, 5, 0);
                    HAL_NVIC_EnableIRQ(TIM_IRQn);

                    htimClock.Instance = TIM_Instance(CBRD_PWM_TIMER);
                    htimClock.Init.Prescaler = (SystemCoreClock / 1000000) - 1;
                    htimClock.Init.CounterMode = TIM_COUNTERMODE_UP;
                    htimClock.Init.Period = 1000 - 2;
                    htimClock.Init.RepetitionCounter = 0;
                    htimClock.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

                    HAL_TIM_Base_Init(&htimClock);


                }
            }
        }
    }

    RETURN(result);
}
/*
    Pwm::deinit()
    {
      __HAL_RCC_TIM3_CLK_DISABLE();
      HAL_NVIC_DisableIRQ(TIM3_IRQn);
    }
*/
void Pwm::outPulse(UINT           channel,
                   float          freq,
                   float          width)
{

}
void Pwm::setBase(UINT periodUs)
{
    htim.Init.Period = periodUs - 1;
    htim.Init.RepetitionCounter = 0;

    if (HAL_TIM_Base_Init(&htim) == HAL_OK)
    {
        // OK
    }
}
void Pwm::setChannel(UINT           channel,
                     UINT           width,
                     UINT           count,
                     PFNPWMINTCB    intCb,
                     PVOID          intCbParam,
                     UINT           intCbTag,
                     UINT           basePeriodUs)
{
    INT             ch      = -1;
    PCHANNEL        c       = &channels[channel - 1];

    HAL_TIM_PWM_Stop(&htim, c->chId);
    HAL_TIM_Base_Stop_IT(&htimClock);

    sConfigOC.OCMode        = TIM_OCMODE_PWM1;
    sConfigOC.Pulse         = width;
    sConfigOC.OCPolarity    = TIM_OCPOLARITY_LOW;
    sConfigOC.OCFastMode    = TIM_OCFAST_DISABLE;
    sConfigOC.OCNIdleState  = TIM_OCNIDLESTATE_RESET;
    sConfigOC.OCIdleState   = TIM_OCIDLESTATE_RESET;

    if (basePeriodUs)
    {
        htim.Init.Period = basePeriodUs - 1;
        HAL_TIM_Base_Init(&htim);
    }

#define MAC_CH(a) case a: ch = TIM_CHANNEL_##a; break
    switch (channel)
    {
        MAC_CH(1);
        MAC_CH(2);
        MAC_CH(3);
        MAC_CH(4);

        default:
            break;
    }
#undef  MAC_CH

    if (ch != -1)
    {
        if (HAL_TIM_PWM_ConfigChannel(&htim, &sConfigOC, ch) == HAL_OK)
        {
            flagPwmIsOver = false;
//            __HAL_TIM_ENABLE_IT(&htim, TIM_IT_CC1);

            HAL_TIM_Base_Start_IT(&htimClock);
            HAL_TIM_PWM_Start(&htim, ch);


            //HAL_TIM_PWM_Start_IT(&htim, ch);
        }
    }
}

#endif // HAL_TIM_MODULE_ENABLED
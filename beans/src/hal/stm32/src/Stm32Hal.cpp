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

#include <app.h>
#include <string.h>

#include <axtime.h>
#include <Watchdog.h>
#include "customboard.h"
//#include <stdio.h>

#if (TARGET_SYSTEM == __AX_freertos__)
#include "FreeRTOS.h"
#include "task.h"
#endif

#if (defined(UPDATE_BY_BOOTLD) && defined(CONFIG_BOOTLD))
#include <Config.h>
#endif

#include "HalPlatformDefs.h"
//#include "cmsis_os.h"

#ifdef UPDATE_BY_BOOTLD
#include <bootld.h>
#endif

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#if (defined(STM32F4) || defined(STM32L4))
#define HAL_HAVE_GPIOD
#define HAL_HAVE_GPIOE
#define HAL_HAVE_GPIO_InitTypeDef_Alternate
#endif

#if (defined(STM32F7))
#define HAL_HAVE_GPIOD
#define HAL_HAVE_GPIOE
#define HAL_HAVE_GPIOF
#define HAL_HAVE_GPIOG
#define HAL_HAVE_GPIOH
#define HAL_HAVE_GPIO_InitTypeDef_Alternate
#endif



#define VALUE_ONE(a)    (HAL_GPIO_WritePin(CBRD_##a##_GPIO, CBRD_##a##_SEL_BIT,\
                            CBRD_##a##_ONE ? GPIO_PIN_SET : GPIO_PIN_RESET))

#define VALUE_ZERO(a)   (HAL_GPIO_WritePin(CBRD_##a##_GPIO, CBRD_##a##_SEL_BIT,\
                            CBRD_##a##_ONE ? GPIO_PIN_RESET : GPIO_PIN_SET))

#define VALUE_READ(a)   (HAL_GPIO_ReadPin(CBRD_##a##_GPIO, CBRD_##a##_PIN) ==  \
                            (CBRD_##a##_ONE ? GPIO_PIN_SET : GPIO_PIN_RESET))

#define VALUE_VAL(a, b) (a ? VALUE_ONE(b) : VALUE_ZERO(b))
/*
#define TOGGLE(a,b)         if(a){ CBRD_##b##_GPIO->SET = (1 << CBRD_##b##_PIN);}\
                            else { CBRD_##b##_GPIO->CLR = (1 << CBRD_##b##_PIN);}
*/

//#define MAC_PULL(a)         ((a) == GpioPullDown ? GPIO_PULLDOWN : ((a) == GpioPullUp ? GPIO_PULLUP : GPIO_NOPULL))

#define BITNR(x) (x&0x1?0:x&0x2?1:x&0x4?2:x&0x8?3: \
                 x&0x10?4:x&0x20?5:x&0x40?6:x&0x80?7: \
                 x&0x100?8:x&0x200?9:x&0x400?10:x&0x800?11: \
                 x&0x1000?12:x&0x2000?13:x&0x4000?14:x&0x8000?15: \
                 x&0x10000?16:x&0x20000?17:x&0x40000?18:x&0x80000?19: \
                 x&0x100000?20:x&0x200000?21:x&0x400000?22:x&0x800000?23: \
                 x&0x1000000?24:x&0x2000000?25:x&0x4000000?26:x&0x8000000?27: \
                 x&0x10000000?28:x&0x20000000?29:x&0x40000000?30:x&0x80000000?31:32)

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

//U64     g_ax_MS_PER_TICK    = HAL_GetTickFreq();

GPIO_TypeDef *   globalGpioByIndex         [] =
{
    GPIOA,
    GPIOB,
    GPIOC,
#ifdef HAL_HAVE_GPIOD
    GPIOD,
#else
    0,
#endif
#ifdef HAL_HAVE_GPIOE
    GPIOE,
#else
    0,
#endif
#ifdef HAL_HAVE_GPIOF
    GPIOF,
#else
    0,
#endif
#ifdef HAL_HAVE_GPIOG
    GPIOG,
#else
    0,
#endif
#ifdef HAL_HAVE_GPIOH
    GPIOH,
#else
    0,
#endif
};

#define NUMBER_OF_PORTS     (sizeof(globalGpioByIndex) / sizeof(GPIO_TypeDef *))


static UINT clkEnableFlags = 0;

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

extern "C"
{
int boardInit(void);
}

static void clkEnable(UINT port)
{
    if (!(clkEnableFlags & (1 << port)))
    {
        switch (port)
        {
            case 0: __HAL_RCC_GPIOA_CLK_ENABLE(); break;
            case 1: __HAL_RCC_GPIOB_CLK_ENABLE(); break;
            case 2: __HAL_RCC_GPIOC_CLK_ENABLE(); break;
#ifdef HAL_HAVE_GPIOD
            case 3: __HAL_RCC_GPIOD_CLK_ENABLE(); break;
#endif
#ifdef HAL_HAVE_GPIOE
            case 4: __HAL_RCC_GPIOE_CLK_ENABLE(); break;
#endif
#ifdef HAL_HAVE_GPIOF
            case 5: __HAL_RCC_GPIOF_CLK_ENABLE(); break;
#endif
#ifdef HAL_HAVE_GPIOG
            case 6: __HAL_RCC_GPIOG_CLK_ENABLE(); break;
#endif
#ifdef HAL_HAVE_GPIOH
            case 7: __HAL_RCC_GPIOH_CLK_ENABLE(); break;
#endif

            default:
                break;
        }

        clkEnableFlags |=  (1 << port);
    }
}

//// ***************************************************************************
//// FUNCTION
////      vConfigureTimerForRunTimeStats
//// PURPOSE
////
//// PARAMETERS
////        void -- None
//// RESULT
////      void --
//// ***************************************************************************
//void vConfigureTimerForRunTimeStats( void )
//{
//    LPC_TIMER0->TCR = 1;  // Enable Timer
//    LPC_TIMER0->CTCR = 0; // Increment on PCLK
//
//    // Set CPU / 1 PCLK
//    lpc_pclk(SYSCTL_PCLK_TIMER0, SYSCTL_CLKDIV_1);
//
//    /**
//     * Enable resolution of 2 uS per timer tick.
//     * This provides 2uS * 2^32 = 140 minutes of runtime before counter over-flows
//     */
//    LPC_TIMER0->PR = (SystemCoreClock * TIMER0_US_PER_TICK) / (1000*1000);
//}

// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      Hal::Hal
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
Hal::Hal()
{
    powerState  = 0;
}
void Hal::init()
{
    boardInit();
}

Hal::~Hal()
{

}
// ***************************************************************************
// FUNCTION
//      Hal::Gpio::setPin
// PURPOSE
//
// PARAMETERS
//      HALGPIO bank  --
//      UINT    pin   --
//      UINT    value --
// RESULT
//      void --
// ***************************************************************************
void Hal::Gpio::setPin(PORTPIN        pp,
                       UINT           value)
{

    if (pp.port < NUMBER_OF_PORTS)
    {
        HAL_GPIO_WritePin(globalGpioByIndex[pp.port], 1 << pp.pin,
                          PP_IS_ONE(pp, value) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
}
// ***************************************************************************
// FUNCTION
//      Hal::Gpio::getPin
// PURPOSE
//
// PARAMETERS
//      HALGPIO bank --
//      UINT    pin  --
// RESULT
//      UINT --
// ***************************************************************************
UINT Hal::Gpio::getPin(PORTPIN pp)
{
    UINT        result          = 0;

    ENTER(true);

    if (pp.port < NUMBER_OF_PORTS)
    {

        result = PP_IS_ONE(pp, HAL_GPIO_ReadPin(globalGpioByIndex[pp.port], 1 << pp.pin));
    }

    RETURN(result);
}

void Hal::Gpio::setMode(PORTPIN        pp,
                        UINT           flags)
{
    int intLine = -1;

    GPIO_InitTypeDef GPIO_InitStruct = { 0 };

    //printf("setMode port %d pin %d flags = %d %d %d\n", pp.port, pp.pin, flags, (flags & HAL_SETMODE_FLAG_HIGH), HAL_SETMODE_FLAG_HIGH);

    if (HAL_SETMODE_FLAG_HIGH == (flags & HAL_SETMODE_FLAG_HIGH))
    {
        //printf("  setMode port %d  pin %d -- HIGH!!!!\n", pp.port, pp.pin);

#ifdef GPIO_SPEED_FREQ_VERY_HIGH
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
#else
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
#endif
    }
    else if (HAL_SETMODE_FLAG_FAST == (flags & HAL_SETMODE_FLAG_FAST))
    {
        //printf("  setMode port %d  pin %d -- FAST!!!!\n", pp.port, pp.pin);

        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    }
    else
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    if (pp.port < NUMBER_OF_PORTS)
    {
        if (GpioDirOut == pp.dir)
        {
            switch (pp.mode)
            {
                case GpioOpenDrain:
                    GPIO_InitStruct.Pull = GPIO_NOPULL;
                    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
                    break;

                case GpioPullDown:
                    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
                    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
                    break;

                case GpioPullUp:
                    GPIO_InitStruct.Pull = GPIO_PULLUP;
                    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
                    break;

                default:
                    GPIO_InitStruct.Pull = GPIO_NOPULL;
                    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
                    break;
            }
        }
        else
        {
            switch ((flags >> 1) & 3)
            {
                case SETVECTOR_EDGE_RISE:
                    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
                    intLine = pp.pin;
                    break;

                case SETVECTOR_EDGE_FALL:
                    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
                    intLine = pp.pin;
                    break;

                case SETVECTOR_EDGE_RISE | SETVECTOR_EDGE_FALL:
                    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
                    intLine = pp.pin;
                    break;

                default:
                    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
                    break;
            }

            switch (pp.mode)
            {
                case GpioPullDown:
                    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
                    break;

                case GpioPullUp:
                    GPIO_InitStruct.Pull = GPIO_PULLUP;
                    break;

                default:
                    GPIO_InitStruct.Pull = GPIO_NOPULL;
                    break;
            }
        }

        clkEnable(pp.port);

        GPIO_InitStruct.Pin       = (1 << pp.pin);
#ifdef HAL_HAVE_GPIO_InitTypeDef_Alternate
        GPIO_InitStruct.Alternate = 0;
#endif
        HAL_GPIO_Init(globalGpioByIndex[pp.port], &GPIO_InitStruct);

        if (GpioDirOut == pp.dir)
        {
            setPin(pp, (flags & 1));
        }
    }

    if (intLine != -1)
    {
        if ((intLine >= 10) && (intLine <= 15))
        {
            HAL_NVIC_SetPriority(EXTI15_10_IRQn, 3, 0);
            HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
        }
        else if ((intLine >= 5) && (intLine <= 9))
        {
            HAL_NVIC_SetPriority(EXTI9_5_IRQn, 3, 0);
            HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
        }
        else
        {
            switch (intLine)
            {
                case 0:
                    HAL_NVIC_SetPriority(EXTI0_IRQn, 3, 0);
                    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
                    break;

                case 1:
                    HAL_NVIC_SetPriority(EXTI1_IRQn, 3, 0);
                    HAL_NVIC_EnableIRQ(EXTI1_IRQn);
                    break;

                case 2:
                    HAL_NVIC_SetPriority(EXTI2_IRQn, 3, 0);
                    HAL_NVIC_EnableIRQ(EXTI2_IRQn);
                    break;

                case 3:
                    HAL_NVIC_SetPriority(EXTI3_IRQn, 3, 0);
                    HAL_NVIC_EnableIRQ(EXTI3_IRQn);
                    break;

                case 4:
                    HAL_NVIC_SetPriority(EXTI4_IRQn, 3, 0);
                    HAL_NVIC_EnableIRQ(EXTI4_IRQn);
                    break;

                default:
                    break;
            }
        }
    }

}
void Hal::Gpio::changeDir(PORTPIN        pp,
                          GpioDir        dir,
                          UINT           flags)
{
    if (pp.port < NUMBER_OF_PORTS)
    {
        if (dir == GpioDirOut)
        {
            LL_GPIO_SetPinMode(globalGpioByIndex[pp.port], 1 << pp.pin, LL_GPIO_MODE_OUTPUT);
        }
        else
        {
            LL_GPIO_SetPinMode(globalGpioByIndex[pp.port], 1 << pp.pin, LL_GPIO_MODE_INPUT);
        }
    }
}

// ***************************************************************************
// FUNCTION
//      mainLoop
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      void --
// ***************************************************************************
void mainLoop()
{
#if (TARGET_SYSTEM == __AX_freertos__)
    vTaskStartScheduler();
#endif

    while (1)
    {
    }
}
// ***************************************************************************
// FUNCTION
//      Hal::Sys::prepareForUpdate
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL Hal::Sys::prepareForUpdate()
{
    BOOL            result          = false;
//    Watchdog &      wd              = Watchdog::getInstance();


    ENTER(true);

//    wd.stop();

#if (TARGET_SYSTEM == __AX_freertos__)
    vTaskSuspendAll();
    taskDISABLE_INTERRUPTS();
#endif

    result = true;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      Hal::Sys::reboot
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      void --
// ***************************************************************************
void Hal::Sys::reboot()
{
    Watchdog &      wd              = Watchdog::getInstance();

    ENTER(true);

    wd.start(1);

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      Hal::Sys::writeProgram
// PURPOSE
//
// PARAMETERS
//      PU8  program --
//      PU8  update  --
//      UINT size    --
// RESULT
//      void --
// ***************************************************************************
int Hal::Sys::writeProgram(PU8            program,
                           PU8            update,
                           UINT           size)
{
    return -1;
}
// ***************************************************************************
// FUNCTION
//      unprepareForUpdate
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      void --
// ***************************************************************************
void unprepareForUpdate()
{

}
void Hal::Bootld::activate()
{
#if (defined(UPDATE_BY_BOOTLD) && defined(CONFIG_BOOTLD))
    //Snarkel::getInstance().log.message("BOOTLD");
//    PSTR addr = (PSTR)BOOTLD_SIGN_ADDR;
//    memcpy(addr, BOOTLD_SIGN, BOOTLD_SIGN_SZ);

//    Config & cfg = Config::getInstance();
//    cfg.sign.isSigned = false;

//    cfg.save(0);
//    cfg.save(0);

    NVIC_SystemReset();
#endif
}
BOOL Hal::Gpio::setPinFunction(UINT           port,
                               UINT           pin,
                               UINT           fn)
{
    return false;
}
// ***************************************************************************
// FUNCTION
//      Hal::Gpio::setRange
// PURPOSE
//
// PARAMETERS
//      HALGPIO bank     --
//      UINT    startPin --
//      UINT    lenght   --
//      UINT    value    --
// RESULT
//      void --
// ***************************************************************************
void Hal::Gpio::setRange(PORTPIN        pp,
                         UINT           length,
                         UINT           value)
{
    UINT startBitNo = BITNR(pp.pin);

    if ((length + startBitNo) <= 16)
    {
        UINT mask   = ((1 << length) - 1) << startBitNo;

#ifdef BSRR_HIGH_AND_LOW
        globalGpioByIndex[pp.port]->BSRRL  = (value << startBitNo) & mask;
        globalGpioByIndex[pp.port]->BSRRH  = (~(value << startBitNo)) & mask;
#else
        globalGpioByIndex[pp.port]->BSRR  = (value << startBitNo) & mask;
        globalGpioByIndex[pp.port]->BSRR  = ((~(value << startBitNo)) & mask) << 16;
#endif
    }
}
// ***************************************************************************
// FUNCTION
//      Hal::Gpio::getRange
// PURPOSE
//
// PARAMETERS
//      HALGPIO bank     --
//      UINT    startPin --
//      UINT    lenght   --
// RESULT
//      UINT --
// ***************************************************************************
UINT Hal::Gpio::getRange(PORTPIN        pp,
                         UINT           length)
{
    UINT result     = 0;
    UINT startBitNo = BITNR(pp.pin);

    result = globalGpioByIndex[pp.port]->IDR;
    result >>= startBitNo;
    result &= ((1 << length) - 1);

    return result;
}

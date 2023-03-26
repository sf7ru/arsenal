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
#include <Fm.h>

#include <customboard.h>
#include <axsystem.h>
#include <axmath.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define HARDCODED_FREQ_BASE 10

#define STM32_FM_CHS      4

#define CHECK_CH(a) ((channel > 0) && (channel <= channels))

#define SET_SIGNAL(a,b) if (a->oneLevel ^ b) { ((GPIO_TypeDef *)a->port)->BSRR = ((uint32_t) 1 << (16 + a->pin)); } else ((GPIO_TypeDef *)a->port)->BSRR = ((uint32_t) 1 << a->pin)


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

//static  UINT                pwm_clock_mhz   = 0;

extern  GPIO_TypeDef *   globalGpioByIndex         [];

static  volatile  FMCHANNEL chCtrl[ STM32_FM_CHS ]    = { 0 };


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

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
    Fm & fm = Fm::getInstance();

    INT periodCount         = period / fm.baseFreq;

    //periodCount              = (UINT)(fm.baseFreqKoef * periodCount);

    if (periodCount < 2)
        periodCount          = 2;

    cc->zeroMultiplier = periodCount - cc->oneMultiplier;
    cc->oneMultiplier  = (cc->oneDuration / baseFreq);
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

//    baseFreqKoef    = HARDCODED_FREQ_KOEF;
    setBase(HARDCODED_FREQ_BASE);

    result = true;

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
    UINT            channel         = 0;
    Hal &           hal             = Hal::getInstance();

    ENTER(true);

    if (channels < STM32_FM_CHS)
    {
        channels++;
        channel = channels;

        volatile FMCHANNEL *cc  = &chCtrl[channel - 1];

        cc->port        = (GPIO_TypeDef *)globalGpioByIndex[pp.port];
        cc->pin         = pp.pin;
        cc->oneLevel    = pp.one;

        hal.gpio.setMode(pp, HAL_SETMODE_FLAG_HIGH);

        SET_SIGNAL(cc, 0);

        cc->intCbPtr        = intCbPtr;
        cc->intCbParam      = intCbParam;

        if (oneDuration < baseFreq)
            oneDuration = baseFreq;

        cc->oneDuration         = oneDuration;
        cc->oneMultiplier       = (oneDuration / baseFreq);
    }

    RETURN(channel);
}

void Fm::timerCb(Fm *           that)
{
    volatile FMCHANNEL *cc;
    int     newPeriod;

    for (int idx = 0; idx < STM32_FM_CHS; idx++)
    {
        cc = &chCtrl[idx];

        if (!cc->oneMultiplierCount)
        {
            // High level phase done

            if (!cc->zeroMultiplierCount || (!(--cc->zeroMultiplierCount)))
            {
                // low level phase done

                if (cc->enabled)
                {
                    // Getting new period

                    if ((newPeriod = (*cc->intCbPtr)(cc->intCbParam, 0)) > 0)
                    {
                        Fm::getInstance().compensateDrift(cc, newPeriod);

                        SET_SIGNAL(cc, 1);

                        cc->oneMultiplierCount = cc->oneMultiplier;
                    }
                    else
                    {
                        cc->enabled = false;
                    }
                }
            }
        }
        else if (!(--cc->oneMultiplierCount))
        {
            // High level phase just done, setting signal low

            SET_SIGNAL(cc, 0);

            cc->zeroMultiplierCount = cc->zeroMultiplier;
        }
    }
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

        Hal::getInstance().timer.set(periodUs, (PFNTIMERCB)Fm::timerCb, this);
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


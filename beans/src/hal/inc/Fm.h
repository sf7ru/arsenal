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

#ifndef __FM_H__
#define __FM_H__

#include <arsenal.h>
#include <Hal.h>
#include <axtime.h>

#include <Pwm.h>

#include <singleton_template.hpp>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------


#define PWM_FALL_SHIFT      10


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------


//typedef INT (*PFNPWMINTCB)(PVOID, UINT tag);

typedef struct __tag_FMCHANNEL
{
    UINT            oneMultiplier;
    UINT            oneMultiplierCount;
    UINT            zeroMultiplier;
    UINT            zeroMultiplierCount;
    UINT            oneDuration;
    PFNPWMINTCB     intCbPtr;
    PVOID           intCbParam;
    BOOL            enabled;
    UINT            pin;
    PVOID           port;
    UINT            oneLevel;
} FMCHANNEL;

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------


class Fm: public SingletonTemplate<Fm>
{
        BOOL            inited;

        UINT            baseFreq;
//        double          baseFreqKoef;

        void            setBase                 (UINT           periodUs);

        UINT            channels;

public:
static  void            timerCb                 (Fm *           that);

        UINT            getBaseFreq             ()
        { return baseFreq; }

        void            compensateDrift         (volatile FMCHANNEL * cc,
                                                 INT            total);

                        Fm                      ();

        BOOL            init                    ();

        UINT            setChannel              (PORTPIN        pp,
                                                 UINT           oneDuration,
                                                 PFNPWMINTCB    intCbPtr,
                                                 PVOID          intCbParam      = nil);

        void            stopChannel             (UINT           channel);

        BOOL            setEnable               (UINT           channel,
                                                 BOOL           enable);
};

#endif // __FM_H__

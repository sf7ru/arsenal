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

#ifndef __PWM_H__
#define __PWM_H__

#include <arsenal.h>

#include <singleton_template.hpp>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------


#define PWM_FALL_SHIFT      10

#define PWM_CHANNELS_NUM    4


typedef UINT (*PFNPWMINTCB)(PVOID, UINT tag);


// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------


class Pwm: public SingletonTemplate<Pwm>
{
        BOOL            inited;
    
        UINT            baseFreq;

public:
                        Pwm                     ();

        BOOL            init                    ();


        BOOL            enableChannel           (UINT           channel);

        void            outPulse                (UINT           channel,
                                                 float          freq)
        { outPulse (channel, freq, freq / 2); }

        void            outPulse                (UINT           channel,
                                                 float          freq,
                                                 float          width);

        void            setBase                 (UINT           periodUs);
        
        void            setChannel              (UINT           channel,
                                                 UINT           width,
                                                 UINT           count           = 0,
                                                 PFNPWMINTCB    intCb           = nil,
                                                 PVOID          intCbParam      = nil,
                                                 UINT           intCbTag        = 0,
                                                 UINT           basePeriodUs    = 0);
        
        void            printStat               (void);
   
};

#endif // __PWM_H__

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

#ifndef __AXCHECKTIMERH__
#define __AXCHECKTIMERH__

#include <axtime.h>

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------


class AXCheckTimer
{
        AXTIME          last;
        UINT            offset;
        UINT            period;

public:
                        AXCheckTimer            (void);

                        ~AXCheckTimer           (void);

        void            set                     (UINT           periodSecs,
                                                 BOOL           firstEventNow,
                                                 BOOL           roundSeconds);

        void            reset                   (BOOL           eventNow        = true)
        { last = eventNow ? 0 : axutime_get(); }


        BOOL            check                   (void)
        { return check(axutime_get()); }

        BOOL            check                   (AXTIME         now);
};

#endif //  #ifndef __AXCHECKTIMERH__


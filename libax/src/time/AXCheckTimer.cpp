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

#include <stdio.h>
#include <AXCheckTimer.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      AXCheckTimer::AXCheckTimer
// PURPOSE
//      Class construction
// PARAMETERS
//        void -- None
// ***************************************************************************
AXCheckTimer::AXCheckTimer(void)
{
    period      = 0;
    offset      = 0;
    last        = 0;
}
// ***************************************************************************
// TYPE
//      Destructor
// FUNCTION
//      AXCheckTimer::~AXCheckTimer
// PURPOSE
//      Class destruction
// PARAMETERS
//        void -- None
// ***************************************************************************
AXCheckTimer::~AXCheckTimer(void)
{

}
// ***************************************************************************
// FUNCTION
//      AXCheckTimer::set
// PURPOSE
//
// PARAMETERS
//      UINT periodSecs    --
//      BOOL firstEventNow --
//      BOOL roundTime     --
// RESULT
//      void --
// ***************************************************************************
void AXCheckTimer::set(UINT           periodSecs,
                       BOOL           firstEventNow,
                       BOOL           roundSeconds)
{
    AXTIME      now;

    ENTER(true);

    now     = axutime_get();
    period  = periodSecs;
    offset  = roundSeconds  ? (now % 60) : now;
    last    = firstEventNow ? 0          : now;

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      AXCheckTimer::check

// PURPOSE
//
// PARAMETERS
//      A7TIME now --
// RESULT
//      BOOL
//            true:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXCheckTimer::check(AXTIME         now)
{
    BOOL            result          = false;

    ENTER(true);

    if (period)
    {
        if (last > now)
        {
            AXTRACE("TIME SCREEWED! AXCheckTimer: was %u, now %u", last, now);

            last = now;
        }
        else if (last != now)
        {
            if (!last || ( !((now - offset) % period)))
            {
                result  = true;
                last    = now;
            }
        }
    }
    else
    {
        // Timer is off
        // result = true;
    }

    RETURN(result);
}

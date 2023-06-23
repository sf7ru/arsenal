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

#include <time.h>

#include <axtime.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      axutime_get_monotonic
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//      AXUTIME  --
// ***************************************************************************
AXUTIME axutime_get_monotonic(void)
{
    AXUTIME          v_time        = 0;

    ENTER(TRUE);

    // TODO: DWORD WINAPI GetTickCount(void); in milliseconds, owerlaps every 50 days

    v_time  = (AXUTIME)time(NULL);

    RETURN(v_time);
}
// ***************************************************************************
// FUNCTION
//      axtime_get_monotonic
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//      AXTIME  --
// ***************************************************************************
AXTIME axtime_get_monotonic(void)
{
    AXTIME          v_time        = 0;

    ENTER(TRUE);

    // TODO: DWORD WINAPI GetTickCount(void); in milliseconds, owerlaps every 50 days

    v_time  = GetTickCount();


    RETURN(v_time);
}

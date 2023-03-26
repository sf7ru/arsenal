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

#include <string.h>
#include <math.h>
#include <axnavi.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      axnavi_loc_reset
// PURPOSE
//
// PARAMETERS
//      PLOCATION loc --
// RESULT
//      void --
// ***************************************************************************
void axnavi_loc_reset(PLOCATION          loc)
{
    ENTER(loc);

    memset(&loc->date, 0, sizeof(AXDATE));

    loc->latitude   = NAN;
    loc->longitude  = NAN;
    loc->altitude   = NAN;
    loc->radius     = NAN;
    loc->speed      = NAN;
    loc->course     = NAN;

//    loc->pdop       = NAN;
//    loc->hdop       = NAN;
//    loc->vdop       = NAN;

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      axnavi_sats_reset
// PURPOSE
//
// PARAMETERS
//      PSATELLITES sats    --
//      UINT        satsNum --
// RESULT
//      void --
// ***************************************************************************
void axnavi_sats_reset(PSATELLITES        sats,
                       UINT               satsNum)
{
    ENTER(sats && satsNum);

    if (satsNum)
        memset(sats, 0, sizeof(SATELLITES) * satsNum);

    QUIT;
}

// ***************************************************************************
// TITLE
//      Time UNIX Platform
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: time.c,v 1.8 2004/03/24 08:28:26 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************

#include <arsenal.h>
#include <axtime.h>

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------


volatile U64 g_ax_system_uptime_ms = 0;

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

AXTIME axtime_get_monotonic()
{
    return g_ax_system_uptime_ms;
}

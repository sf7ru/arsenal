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

#include <Can.h>

#include <app.h>
#include <string.h>

#include <customboard.h>
#include <axtime.h>
#include <axthreads.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------


void Can::constructClass()
{
    LCK             = nil;
    ifaceNo         = -1;
}
void Can::deinitBase()
{
    SAFEDESTROY(axmutex, LCK);
}
BOOL Can::initBase()
{
    BOOL        result          = false;

    ENTER(true);

    ifaceNo         = getSingletoneIndex();

    if ((LCK = axmutex_create()) != nil)
    {
        result = true;
    }

    RETURN(result);
}
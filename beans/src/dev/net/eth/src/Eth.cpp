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

#include <Eth.h>
#include <EthPhy.h>

#include <app.h>
#include <string.h>

#include <customboard.h>
#include <axtime.h>
#include <axthreads.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

/* ETH protocol address bits */
#define DIR_READ                (1<<7)
#define DIR_WRITE               (0<<7)
#define ADDR_INCREMENT          (1<<6)


//#include <stdio.h>
//#undef AXTRACE
//#define AXTRACE printf

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

void Eth::constructClass()
{
//    lockedDevice    = nil;
//    LCK             = nil;
    ifaceNo         = -1;
}
void Eth::deinitBase()
{
    SAFEDESTROY(axmutex, LCK);
}
BOOL Eth::initBase()
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
INT Eth::read(UINT           reg,
              PVOID          data,
              INT            len)
{
    INT         result          = -1;

    return result;
}
INT Eth::write(UINT           reg,
                    PVOID          data,
                    INT            len)
{
    INT         result          = -1;

    return result;
}

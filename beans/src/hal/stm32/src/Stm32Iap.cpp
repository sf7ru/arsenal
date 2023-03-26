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

#include <arsenal.h>

#include <Iap.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define UID_SZ_NUM          3
#define UID_SZ_BYTES        (sizeof(U32) * UID_SZ_NUM)


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------


// SEE:  https://habrahabr.ru/post/213771/


// ***************************************************************************
// FUNCTION
//      Iap::readSerial
// PURPOSE
//
// PARAMETERS
//      PVOID data --
//      INT   size --
// RESULT
//      INT  --
// ***************************************************************************
INT  Iap::readSerial(PVOID  data,
                     INT    size)
{
    INT             result          = 0;
    PU32            pd;
    PU32            ps;

    ENTER(true);

    if (size >= (INT)UID_SZ_BYTES)
    {
        pd      = (PU32)data;
        ps      = (PU32)0x1FFFF7E8;

        for (int i = 0; i < UID_SZ_NUM; i++)
        {
            pd[i]   = ps[i];
        }

        result  = UID_SZ_BYTES;
    }
    else
        result  = -UID_SZ_BYTES;

    RETURN(result);
}

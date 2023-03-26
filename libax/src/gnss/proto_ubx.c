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

#include <axnavi.h>

#include <stdio.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

INT pubx_cmd_enable_sentence(PSTR               buff,
                              UINT               size,
                              PCSTR              sentence,
                              BOOL               enable)
{
    INT         result;

    ENTER(true);

    result = snprintf(buff, size, "$PUBX,40,%s,0,%d,0,0", sentence, enable ? 1 : 0);
    result = nmea_add_checksum(buff, result);

    RETURN(result);
}

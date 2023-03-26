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

INT nwy_cmd_api_set_fix(PSTR               buff,
                         UINT               size,
                         UINT               period)
{
    INT         result;

    ENTER(true);

    sprintf(buff, "$CCINV,%d,", period);
    result = nmea_add_checksum(buff, size - 1);

    RETURN(result);
}
INT nwy_cmd_set_sentences(PSTR               buff,
                          UINT               size,
                          MTKSENSENCES       set,
                          UINT               index)
{
#define MAC_SENTENCE(id,s) case id: name=#s; value = set.s; break

    INT         result      = 0;
    PCSTR       name        = nil;
    int         value       = 0;

    ENTER(true);

    switch (index)
    {
        MAC_SENTENCE(0, GLL);
        MAC_SENTENCE(1, RMC);
        MAC_SENTENCE(2, VTG);
        MAC_SENTENCE(3, GGA);
        MAC_SENTENCE(4, GSA);
        MAC_SENTENCE(5, GSV);
        MAC_SENTENCE(6, GRS);
        MAC_SENTENCE(7, GST);
        MAC_SENTENCE(8, TXT);
        MAC_SENTENCE(9, ZDA);

        default:
            break;
    }

    if (name)
    {
        sprintf(buff, "$CCMSG,%s,1,%d,", name, value);

        result = nmea_add_checksum(buff, size - 1);
    }

    RETURN(result);

#undef MAC_SENTENCE
}

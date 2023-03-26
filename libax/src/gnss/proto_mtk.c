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

INT pmtk_cmd_api_set_fix(PSTR               buff,
                         UINT               size,
                         UINT               period)
{
    INT         result;

    ENTER(true);

    sprintf(buff, "$PMTK300,%d,0,0,0,0", period);
    result = nmea_add_checksum(buff, size - 1);

    RETURN(result);
}
INT pmtk_cmd_nmea_update_rate(PSTR               buff,
                              UINT               size,
                              UINT               period)
{
    INT         result;

    ENTER(true);

    sprintf(buff, "$PMTK220,%d", period);
    result = nmea_add_checksum(buff, size - 1);

    RETURN(result);
}
INT pmtk_cmd_set_sentences(PSTR               buff,
                           UINT               size,
                           MTKSENSENCES       set)
{
    INT         result;

    ENTER(true);

    sprintf(buff, "$PMTK314,%d,%d,%d,%d,%d,%d,%d,%d,0,0,0,0,0,0,0,0,0",
            set.GLL,
            set.RMC,
            set.VTG,
            set.GGA,
            set.GSA,
            set.GSV,
            set.GRS,
            set.GST);

    result = nmea_add_checksum(buff, size - 1);

    RETURN(result);
}

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
#include <stdlib.h>
#include <string.h>
#include <a7time.h>
#include <a7string.h>
#include <stdarg.h>
#include <string.h>

#include <a7string.h>


BOOL _nmea_validate(PSTR psz_string)
{
    BOOL    b_result        = FALSE;
    UINT    u_sum           = 0;
    PSTR    on              = psz_string;

    ENTER(on);

    if (*on == '$')
    {
        on++;
    }

    while (*on && (*on != '*'))
    {
        u_sum ^= *(on++);
    }

    if (*on == '*')
    {
        printf("calculated = %X, orig = %X\n", u_sum, (UINT)strtol(on + 1, NULL, 16));
        
        if (u_sum == (UINT)strtol(on + 1, NULL, 16))
        {
            *on         = 0;
            b_result    = TRUE;
        }
    }

    RETURN(b_result);
}

// ***************************************************************************
// FUNCTION
//      main
// PURPOSE
//
// PARAMETERS
//      int      argc   --
//      char *   argv[] --
// RESULT
//      int  --
// ***************************************************************************
int main(int        argc,
         char *     argv[])
{
    int             result      = 0;
    
    static PSTR ab_condor_nmea     = (PSTR)"$PMTK314,0,1,0,0,3,3,0,0,0,0,0,0,0,0,0,0,0,1,0*11";
    static PSTR ab_condor_bitrate  = (PSTR)"$PMTK251,4800*22";
    static PSTR ab_condor_flash    = (PSTR)"$PMTK390,1,1,4800,1,1,1,1,1,1,1,0,0,2,4800*33";
    

    
    ENTER(TRUE);

    _nmea_validate(ab_condor_nmea);
    _nmea_validate(ab_condor_bitrate);
    _nmea_validate(ab_condor_flash);    

    RETURN(result);
}

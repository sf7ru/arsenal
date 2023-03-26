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

#include <stdlib.h>
#include <string.h>

#include <arsenal.h>
#include <axstring.h>


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      strz_wildcard
// PURPOSE
//      Matching strings with wildcard
// PARAMETERS
//      PSTR psz_name     -- Pointer to Name to match
//      PSTR psz_wildcard -- Pointer to Wildcard
// RESULT
//      true if name is matched or false if not
// ***************************************************************************
BOOL strz_wildcard(PSTR   psz_name,
                   PSTR   psz_wildcard)
{
    BOOL    b_retcode           = true;
    BOOL    b_star_mode         = false;
    PSTR    psz_saved_name      = NULL;
    PSTR    psz_saved_wildcard  = NULL;


    if (psz_name && psz_wildcard)
    {
        while (*psz_name && b_retcode)
        {
            switch (*psz_wildcard)
            {
                case '?':   psz_wildcard++; break;

                case '*':
                    psz_wildcard++;
                    psz_saved_name      = psz_name;
                    psz_saved_wildcard  = psz_wildcard;
                    b_star_mode         = true;
                    // There is no 'break' - must continue.

                default:
                    if (b_star_mode)
                    {
                        if (*psz_name == *psz_wildcard)
                            psz_wildcard++;
                        else
                        {
                            psz_name     = psz_saved_name++;
                            psz_wildcard = psz_saved_wildcard;
                        }
                    }
                    else
                    {
                        if (*psz_name != *psz_wildcard)
                            b_retcode = false;
                        else
                            psz_wildcard++;
                    }
                    break;
            }
            psz_name++;
        }

        // If we are at the end of the wildcard strings then wildcard is matched.
        if (!b_retcode || (*psz_wildcard != 0 && *psz_wildcard != '*'))
            b_retcode = false;
    }
    else
        b_retcode = false;                  // Parameters are wrong

    return b_retcode;
}

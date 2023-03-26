// ***************************************************************************
// TITLE
//      .INI Scripts
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: ini.c,v 1.4 2003/11/10 17:01:38 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************


#include <string.h>

#include <arsenal.h>
#include <axstring.h>

#include <AXConfig.h>


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------


// ***************************************************************************
// FUNCTION
//      dotini_script_parse
// PURPOSE
//      Parse script
// PARAMETERS
//      PCONFSCRIPT pst_ini      -- Pointer to INI control structure
//      PSTR          psz_tag      -- Pointer to target string for tag
//      DWORD         d_tag_size   -- Size of string for tag
//      PSTR          psz_param    -- Pointer to target string for parameter
//      DWORD         d_param_size -- Size of string for parameter
// RESULT
//      LA6TALKCODE -- Operation result
//          LA6TC_record:   Tag and Parameter strings are cola6ins a values
//          LA6TC_empty:    End of script reached
//          Other:          Error code
// ***************************************************************************
PARSEL dotini_script_parse(PCONFSCRIPT   pst_ini,
                           PSTR          psz_tag,
                           UINT          d_tag_size,
                           PSTR          psz_param,
                           UINT          d_param_size)
{
    PARSEL retcode;

    if ((retcode = dotconf_script_parse(pst_ini, psz_tag, d_tag_size, psz_param, d_param_size)) == PARSEL_value)
    {
        if ( (*psz_tag                         == DEFSCR_DEF_CHAPOPEN)  &&
             (*(psz_tag + strlen(psz_tag) - 1) == DEFSCR_DEF_CHAPCLOSE) )
        {
            *(psz_tag + strlen(psz_tag) - 1) = 0;
            strz_cpy(psz_tag, (psz_tag + 1), d_tag_size);
            retcode = PARSEL_struct;
        }
    }

    return retcode;
}

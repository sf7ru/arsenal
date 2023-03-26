// ***************************************************************************
// TITLE
//      Definition Scripts
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: defs.c,v 1.6 2003/11/10 17:01:38 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************

#include <stdlib.h>
#include <string.h>

#include <arsenal.h>

#include <axfile.h>

#include <AXConfig.h>


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      dotdef_script_destroy
// PURPOSE
//      Destroy script
// PARAMETERS
//      PCONFSCRIPT pst_script -- Pointer to Script control structure
// RESULT
//      PCONFSCRIPT -- Always NULL
// ***************************************************************************
PCONFSCRIPT dotdef_script_destroy(PCONFSCRIPT pst_script)
{
    if(pst_script)
    {
        if(pst_script->p_script)
            FREE(pst_script->p_script);

        FREE(pst_script);
    }

    return NULL;
}
// ***************************************************************************
// FUNCTION
//      dotdef_script_create
// PURPOSE
//      Create script
// PARAMETERS
//      PSTR          psz_script  -- Pointer to script text
//      BOOL          b_copy_data -- TRUE if function should copy text
//                                   of script or FALSE if it should reuse
//                                   given pointer
// RESULT
//      PCONFSCRIPT   --
// ***************************************************************************
PCONFSCRIPT dotdef_script_create(PSTR psz_script, BOOL b_copy)
{
    PCONFSCRIPT   pst_script  = NULL;

    if (psz_script)
    {
        if ((pst_script = CREATE(CONFSCRIPT)) != NULL)
        {
// ---------------------- Copy script if necessary ---------------------------

            if (b_copy)
            {
                if ((pst_script->p_script = MALLOC_D(
                                strlen(psz_script) + sizeof(CHAR), NULL)) != NULL)
                    strcpy((PSTR)pst_script->p_script, psz_script);
            }
            else
                pst_script->p_script = (PU8)psz_script;

// ------------------------ Make default settings ----------------------------

            if (pst_script->p_script)
            {
                pst_script->c_equation      = DEFSCR_DEF_EQU;
                pst_script->c_remark        = DEFSCR_DEF_REM;
                pst_script->p_curr_position = pst_script->p_script;
            }
            else
                pst_script = dotdef_script_destroy(pst_script);
        }
    }

    return pst_script;
}

// ***************************************************************************
// FUNCTION
//      dotdef_script_open
// PURPOSE
//      Load script and create Script control structure
// PARAMETERS
//      PSTR          psz_filename -- Path and name of source script file
// RESULT
//      PCONFSCRIPT -- Pointer to new createtd Script control structure
//                       if script was loaded or NULL if not
// ***************************************************************************
PCONFSCRIPT dotdef_script_open(PSTR psz_filename)
{
    PCONFSCRIPT   pst_script  = NULL;
    PSTR            psz_text;
    UINT            d_size;

    if (psz_filename)
    {
        if ((psz_text = axfile_load(psz_filename, &d_size, 1)) != NULL)
        {
            *(psz_text + d_size) = 0;

            if ((pst_script = dotdef_script_create(psz_text, false)) == NULL)
                FREE(psz_text);
        }
    }

    return pst_script;
}

// ***************************************************************************
// FUNCTION
//      dotdef_script_reset
// PURPOSE
//
// PARAMETERS
//      PCONFSCRIPT pst_script -- Pointer to Script control structure
// RESULT
//      none
// ***************************************************************************
void dotdef_script_reset(PCONFSCRIPT pst_script)
{
    if (pst_script)
    {
        pst_script->d_curr_line     = 0;
        pst_script->p_curr_position = pst_script->p_script;
    }
}

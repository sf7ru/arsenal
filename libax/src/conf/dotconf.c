// ***************************************************************************
// TITLE
//      Simply control scripts handing
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: ctl.c,v 1.6 2003/08/06 12:25:58 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************


#include <stdio.h>
#include <string.h>

#include <arsenal.h>

#include <AXConfig.h>

// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

#define DEF_TAG_SIZE        128



// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------


// ***************************************************************************
// FUNCTION
//      dotconf_script_parse
// PURPOSE
//      Parse script
// PARAMETERS
//      PCONFSCRIPT pst_ctl      -- Pointer to Script control structure
//      PSTR          psz_tag      -- Pointer to buffer for Tag
//      UINT         u_tag_size   -- Size of buffer for Tag
//      PSTR          psz_param    -- Pointer to buffer for Parameter
//      UINT         u_param_size -- Size of buffer for Parameter
// RESULT
//      PARSEL -- Operation result
//          PARSEL_value:   Tag and Parameter buffers are contains values
//          PARSEL_end:    End of script reached
//          Other:          Error code
// ***************************************************************************
PARSEL dotconf_script_parse(PCONFSCRIPT     pst_ctl,
                            PSTR            psz_tag,
                            UINT            u_tag_size,
                            PSTR            psz_param,
                            UINT            u_param_size)
{
    PARSEL          v_el            = PARSEL_error;
    PCSTR           psz_on_script;
    PSTR            psz_on_str;

    if (pst_ctl && psz_tag && psz_param && u_param_size)
    {
        psz_on_script = (PSTR)pst_ctl->p_curr_position;

        while (v_el == PARSEL_error)
        {
            if (*(psz_on_script = strz_notspace(psz_on_script, -1, false)))
            {
                if (*psz_on_script != pst_ctl->c_remark)
                {
// ---------------------------- If not remark --------------------------------

                    v_el = PARSEL_value;

// ----------------------------- Getting Tag ---------------------------------

                    psz_on_str = psz_tag;

                    while (  *psz_on_script                                 &&
                             (*(PU8)psz_on_script != '\n')                &&
                             (*(PU8)psz_on_script != pst_ctl->c_equation) )
                        *(psz_on_str++) = *(psz_on_script++);

                    *psz_on_str = 0;

                    strz_trim(psz_tag);

                    if (*psz_on_script == pst_ctl->c_equation)
                        psz_on_script++;

                    if ( (*(psz_on_script = strz_notspace(psz_on_script, -1, true)))  &&
                         (*psz_on_script != '\n')                                           )
                    {
// -------------------------- Getting Parameter ------------------------------

                        psz_on_str = psz_param;

                        while (  *psz_on_script                     &&
                                 (*(PU8)psz_on_script != '\n')    )
                            *(psz_on_str++) = *(psz_on_script++);

                        *psz_on_str = 0;

                        strz_trim(psz_param);
                    }
                    else
                        *psz_param  = 0;
                }
                else
                    psz_on_script = strz_eol(psz_on_script, -1);
            }
            else
                v_el = PARSEL_end;
        }

        pst_ctl->p_curr_position = (PU8)psz_on_script;
    }

    return v_el;
}
// ***************************************************************************
// FUNCTION
//      dotconf_value_get
// PURPOSE
//
// PARAMETERS
//      PCONFSCRIPT   pst_ctl   --
//      PSTR          psz_value --
//      UINT         u_size    --
//      PSTR          psz_name  --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL dotconf_value_get(PCONFSCRIPT      pst_ctl,
                       PSTR             psz_value,
                       UINT            u_size,
                       PSTR             psz_name)
{
    BOOL        b_result    = false;
    PARSEL      v_el;
    CHAR        sz_tag      [ DEF_TAG_SIZE + 1 ];

    ENTER(pst_ctl && psz_value && u_size && psz_name);

    dotdef_script_reset(pst_ctl);

    while (!b_result)
    {
        if ((v_el = dotconf_script_parse(pst_ctl, sz_tag,
                            DEF_TAG_SIZE, psz_value, u_size)) != PARSEL_value)
        {
            if (v_el == PARSEL_end)
                break;
        }
        else
            b_result = !strcmpi(sz_tag, psz_name);
    }

    RETURN(b_result);
}




// ***************************************************************************
// TITLE
//      Text Templates
// PROJECT
//      Arsenal Library
// ***************************************************************************
//
// FILE
//      $Id: ttempl.c,v 1.1.1.1 2003/02/14 13:17:35 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// HISTORY
//      $Log: ttempl.c,v $
//      Revision 1.1.1.1  2003/02/14 13:17:35  A.Kozhukhar
//      Initial release
//
// ***************************************************************************


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <arsenal.h>
#include <axstring.h>

// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      strz_template_parse
// PURPOSE
//
// PARAMETERS
//      PUINT      pd_size         -- Pointer to size varialble, which will
//                                    receive size of parsed text
//      PSTR       psz_template    -- Pointer to source text template
//      PSTR       psz_l_delim     -- Pointer left delimiter of keyword
//      PSTR       psz_r_delim     -- Pointer right delimiter of keyword
//      PFNAXKEYCB pfn_proceed_key -- Pointer to call-back function
//                                    which will receive every keyword
//                                    for unwrapping.
//                                    Given keyword will be replaced by
//                                    the text, returned in this function
//      PVOID     p_ptr            -- User's parameter for call-back
//                                    function
// RESULT
//      PSTR -- Pointer to allocated buffer with parsed text
// ***************************************************************************
PSTR strz_template_parse(PUINT              pd_size,
                         PSTR               psz_template,
                         PSTR               psz_l_delim,
                         PSTR               psz_r_delim,
                         PFNAXKEYCB         pfn_proceed_key,
                         PVOID              p_ptr)
{
    PSTR    psz_parsed      = NULL;         // Return
    PSTR    psz_tmp_parsed;                 // Pointer to current position
    PSTR    psz_start;                      // Start of keyword
    PSTR    psz_stop;                       // End of keyword
    PSTR    psz_param;                      // Value returned by callback
    PSTR    p_tmp;                          // Temporary
    UINT    d_allocated;                    // Allocated size
    UINT    d_l_size;                       // Size of left delimiter
    UINT    d_r_size;                       // Size of right delimiter
    UINT    d_to_copy;                      // Size to copy
    UINT    d_key_size;                     // Size of keyword
    UINT    d_param_size;                   // Size of parameter
    BOOL    b_dynamic;                      // Is value from callback dymamic

    if (psz_template && psz_l_delim && psz_r_delim && pfn_proceed_key)
    {
// --------------------------- Precalculation --------------------------------

        d_allocated = strlen(psz_template) + sizeof(CHAR);
        d_l_size    = strlen(psz_l_delim);
        d_r_size    = strlen(psz_r_delim);

// ---------------- Allocating memory for parsed version ---------------------

        if ((psz_tmp_parsed = psz_parsed = MALLOC_D(d_allocated, "Parsed Template")) != NULL)
        {

// ------------------------------- Parsing -----------------------------------

//            printf("la6_ttempl_make_parsed stage 0 psz_template = '%s'\n", psz_template);

            while ( psz_parsed                                                        &&
                    ((psz_start = strstr(psz_template,         psz_l_delim)) != NULL) &&
                    ((psz_stop  = strstr(psz_start + d_l_size, psz_r_delim)) != NULL) )
            {
//                printf("la6_ttempl_make_parsed stage 1 psz_start = '%s'\n", psz_start);
//                printf("la6_ttempl_make_parsed stage 1 psz_stop  = '%s'\n", psz_stop);

// ----------------------- Delimited keyword found ---------------------------

                d_key_size = (psz_stop - psz_start - d_l_size);
                psz_param  = (*pfn_proceed_key)(p_ptr, psz_start + d_l_size,
                                                d_key_size, &b_dynamic);

// ----------------- Copying symbols followed to keyword ---------------------

                d_to_copy = psz_start - psz_template + (psz_param ? 0 : d_l_size);
                memcpy(psz_tmp_parsed, psz_template, d_to_copy);
                psz_tmp_parsed += d_to_copy;
                psz_template   += d_to_copy;

// ----------------- Inserting keyword value if expanded ---------------------

                if (psz_param == (PSTR)-1)
                {
                    SAFEFREE(psz_parsed);
                }
                else if (psz_param)
                {
                    d_key_size   += (d_l_size + d_r_size);
                    d_param_size  = strlen(psz_param);

                    // If need to realloc memory
                    if (d_key_size < d_param_size)
                    {
                        if ((p_tmp = REALLOC(psz_parsed,
                            d_allocated + (d_param_size - d_key_size))) != NULL)
                        {
                            d_allocated    += ((d_param_size - d_key_size));
                            psz_tmp_parsed  = p_tmp + (psz_tmp_parsed - psz_parsed);
                            psz_parsed      = p_tmp;
                        }
                        else
                        {
                            FREE(psz_parsed);
                            psz_parsed = NULL;
                            break;          // Exit on error
                        }
                    }

// ----------------------- Copying expanded value ----------------------------

                    memcpy(psz_tmp_parsed, psz_param, d_param_size);
                    psz_tmp_parsed += d_param_size;

                    if (b_dynamic)
                        FREE(psz_param);
                }

                psz_template    = (psz_stop + d_r_size);

//                printf("la6_ttempl_make_parsed stage 10 psz_template = '%s'\n", psz_template);
            }

// -------- Copying leaved template symbols if no errors occured -------------

            if (psz_parsed)
            {
                strcpy(psz_tmp_parsed, psz_template);

                if (pd_size)
                    *pd_size = (psz_tmp_parsed - psz_parsed) + strlen(psz_tmp_parsed) + sizeof(CHAR);
            }
        }
    }

    return psz_parsed;
}

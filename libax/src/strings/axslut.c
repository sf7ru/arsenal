// ***************************************************************************
// TITLE
//      Static Lookup Tables
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: slut.c,v 1.4 2003/09/17 11:57:07 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arsenal.h>
#include <axstring.h>
#include <axhashing.h>
#include <records.h>
#include <pvt_string.h>
#include <pvt_records.h>

// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      axslut_destroy
// PURPOSE
//      Destroy Static Lookup Table and control structure
// PARAMETERS
//      HAXSLUT pst_slut -- Pointer to Static Lookup Table control structure
// RESULT
//      HAXSLUT    -- Always NULL
// ***************************************************************************
HAXSLUT axslut_destroy(HAXSLUT h_slut)
{
    PAXSLUT         pst_slut         = (PAXSLUT)h_slut;

    ENTER(pst_slut);

    if (pst_slut->pst_digest_tab)
        FREE(pst_slut->pst_digest_tab);

    FREE(pst_slut);

    RETURN(nil);
}
// ***************************************************************************
// STATIC FUNCTION
//      __low_case
// PURPOSE
//      Turn characters to upper case
// PARAMETERS
//      PSTR  psz_string -- Pointer to source/target string
//      UINT d_size     -- Size of string
// RESULT
//      PSTR    -- Value of psz_string
// ***************************************************************************
static PSTR __low_case(PSTR psz_string, UINT d_size)
{
    PSTR    psz_tmp;

    if (psz_string && d_size)
    {
        for (psz_tmp = psz_string; *psz_tmp; psz_tmp++)
        {
            if ((*psz_tmp > 64) && (*psz_tmp < 91))
                *psz_tmp |= 0x20;
        }
    }

    return psz_string;
}
// ***************************************************************************
// FUNCTION
//      axslut_create
// PURPOSE
//      Create Static Lookup table from arrays of zerro-separated strings
// PARAMETERS
//      PSTR     psz_initial_table -- Pointer to arrays of strings
//      BOOL     b_case_sensitive  -- TRUE if string are case sensitive
// RESULT
//      HAXSLUT    -- Pointer to the just created Static Lookup Table
//                     control structure
// ***************************************************************************
HAXSLUT axslut_create(PSTR      psz_initial_table,
                      BOOL      b_case_sensitive)
{
    PAXSLUT         result          = nil;
    PSTR            p_tmp_word;
    UINT            d_len;
    AXSLUTENTRY     st_entry;
    HRECARRAY       pst_array;

    ENTER(psz_initial_table);

    if ((result = CREATE(AXSLUT)) != NULL)
    {
        memset(result, 0, sizeof(AXSLUT));
        result->psz_strings       = psz_initial_table;
        result->b_case_sensitive  = b_case_sensitive;

        if ((pst_array = recarray_create(sizeof(AXSLUTENTRY), 10, 10, nil)) != nil)
        {
            p_tmp_word = psz_initial_table;

            while (result && *p_tmp_word != 0)
            {
                d_len                 = strlen(p_tmp_word);
                st_entry.d_digest     = (result->b_case_sensitive ?
                                        crc32_calc((PU8)p_tmp_word, d_len) :
                                        crc32_calc((PU8)
                                            __low_case(p_tmp_word, d_len), d_len));
                st_entry.d_offset     = p_tmp_word - psz_initial_table;
                p_tmp_word           += (++d_len);

                if (recarray_add(pst_array, &st_entry) == AXII)
                {
                    result = (PAXSLUT)axslut_destroy((HAXSLUT)result);
                }
            }

            if (result)
            {
                result->pst_digest_tab = (PAXSLUTENTRY)recarray_freeze(pst_array, &result->d_digest_entries);
            }
            else
                recarray_destroy(pst_array);
        }
        else
            result = (PAXSLUT)axslut_destroy((HAXSLUT)result);
    }


    RETURN((HAXSLUT)result);
}

// ***************************************************************************
// FUNCTION
//      axslut_conv_prepared
// PURPOSE
//      Convert already prepared Static Lookup Table to
// PARAMETERS
//      PSTR     psz_table -- Pointer to text part of table
//      PVOID    p_digest  -- Pointer to array of digests
// RESULT
//      HAXSLUT    -- Pointer to the just created Static Lookup Table
//                     control structure
// ***************************************************************************
HAXSLUT axslut_conv_prepared(PSTR psz_table, PVOID p_digest)
{
    PAXSLUT        result        = NULL;
    UINT           d_size;
    UINT           d_flags;
    PUINT          pd_on;

    ENTER(psz_table && p_digest);

    pd_on   = p_digest;

    if ((result = CREATE(AXSLUT)) != nil)
    {
        memset(result, 0, sizeof(AXSLUT));
        result->psz_strings       = psz_table;
        result->d_digest_entries  = *(pd_on++);
        d_flags                     = *(pd_on++);
        result->b_case_sensitive  = (BOOL)(d_flags & AXSLUT_FLAG_CASESENS);

        d_size = result->d_digest_entries * sizeof(AXSLUTENTRY);

        if ((result->pst_digest_tab = (PAXSLUTENTRY)MALLOC(d_size)) != NULL)
            memcpy(result->pst_digest_tab, pd_on, d_size);
        else
            result = (PAXSLUT)axslut_destroy((HAXSLUT)result);
    }

    RETURN((HAXSLUT)result);
}

// ***************************************************************************
// FUNCTION
//      axslut_find
// PURPOSE
//      Find string in the Static Lookup Table
// PARAMETERS
//      HAXSLUT pst_slut   -- Pointer to Static Lookup Table
//                             control structure
//      PSTR     psz_string -- Pointer to string to find
// RESULT
//      UINT
//          A7II:     Not found
//          Other:              Index of string in table
// ***************************************************************************
UINT axslut_find(HAXSLUT    h_slut,
                 PSTR       psz_string)
{
    PAXSLUT         pst_slut        = (PAXSLUT)h_slut;
    UINT            d_index         = AXII;
    UINT            d_len;
    UINT            d_digest;
    UINT            d_words;
    UINT            d_cnt           = 0;
    PAXSLUTENTRY    pst_slut_ent;

    ENTER(pst_slut && psz_string);

    d_len        = strlen(psz_string);
    d_digest     = (pst_slut->b_case_sensitive ?
                        crc32_calc((PU8)psz_string, d_len) :
                        crc32_calc((PU8)
                            __low_case(psz_string, d_len), d_len));
    d_words      = pst_slut->d_digest_entries;
    pst_slut_ent = pst_slut->pst_digest_tab;

    while (d_cnt < d_words)
    {
        if ( (pst_slut_ent->d_digest == d_digest)               &&
             !memcmp(psz_string, pst_slut->psz_strings +
                                pst_slut_ent->d_offset, d_len)  )
        {
            d_index = d_cnt;
            break;
        }

        d_cnt++;
        pst_slut_ent++;
    }

    RETURN(d_index);
}

// ***************************************************************************
// FUNCTION
//      axslut_find_by_wc
// PURPOSE
//      Find string in the Static Lookup Table
// PARAMETERS
//      HAXSLUT h_slut   -- Pointer to Static Lookup Table
//                             control structure
//      PSTR     psz_wildcard -- Pointer to string to find
// RESULT
//      UINT
//          AXII:     Not found
//          Other:              Index of string in table
// NITE
//      Really not so fast
// ***************************************************************************
UINT axslut_find_by_wc(HAXSLUT      h_slut,
                       PSTR         psz_wildcard)
{
    PAXSLUT         pst_slut        = (PAXSLUT)h_slut;
    UINT            d_index         = AXII;
    UINT            d_words;
    UINT            d_cnt           = 0;
    PAXSLUTENTRY    pst_slut_ent;


    if (pst_slut && psz_wildcard)
    {
        if (  !pst_slut->b_case_sensitive                       ||
              __low_case(psz_wildcard, strlen(psz_wildcard))    )
        {
            d_words      = pst_slut->d_digest_entries;
            pst_slut_ent = pst_slut->pst_digest_tab;

            while (d_cnt < d_words)
            {
                if (strz_wildcard(pst_slut->psz_strings +
                                pst_slut_ent->d_offset, psz_wildcard))
                {
                    d_index = d_cnt;
                    break;
                }

                d_cnt++;
                pst_slut_ent++;
            }
        }
    }

    return d_index;
}

// ***************************************************************************
// FUNCTION
//      axslut_get_ptr_by_idx
// PURPOSE
//
// PARAMETERS
//      HAXSLUT pst_slut --
//      UINT    d_idx    --
// RESULT
//      PSTR    --
// ***************************************************************************
PSTR axslut_get_ptr_by_idx(HAXSLUT      h_slut,
                           UINT         d_idx)
{
    PAXSLUT         pst_slut        = (PAXSLUT)h_slut;
    PSTR            psz_string      = NULL;

    if (pst_slut && (d_idx < pst_slut->d_digest_entries))
    {
        psz_string  = pst_slut->psz_strings +
                      pst_slut->pst_digest_tab[d_idx].d_offset;
    }

    return psz_string;
}

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
#include <stdio.h>
#include <string.h>

#include <arsenal.h>

#include <axstring.h>


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

// ***************************************************************************
// STRUCTURE
//      SUBSTRS
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_SUBSTRS
{
    PSTR                   psz_source;
    PSTR                   psz_on;
    UINT                    u_len;
    CHAR                    c_sep;
    UINT                    u_flags;
} SUBSTRS, * PSUBSTRS;





// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      strz_substrs_destroy
// PURPOSE
//
// PARAMETERS
//      HSUBSTRS   h_substr --
// RESULT
//      HSUBSTRS  --
// ***************************************************************************
HSUBSTRS strz_substrs_destroy(HSUBSTRS     h_substr)
{
    PSUBSTRS         pst_S   = (PSUBSTRS)h_substr;

    ENTER(pst_S);

    if (pst_S->psz_source)
    {
        FREE(pst_S->psz_source);
    }

    FREE(pst_S);

    RETURN(NULL);
}
// ***************************************************************************
// FUNCTION
//      strz_substrs_create
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_source     --
//      UINT   u_len          --
//      CHAR   c_sep          --
//      BOOL   b_sep_is_field --
// RESULT
//      HSUBSTRS  --
// ***************************************************************************
HSUBSTRS strz_substrs_create(PCSTR    psz_source,
                             UINT     u_len,
                             CHAR     c_sep,
                             UINT     u_flags)
{
    PSUBSTRS     pst_S       = NULL;

    ENTER(psz_source);

    if ((pst_S = CREATE(SUBSTRS)) != NULL)
    {
        pst_S->u_len            = u_len ? u_len : strlen(psz_source);
        pst_S->c_sep            = c_sep;
        pst_S->u_flags          = u_flags;

        if (u_flags & SUBSTRS_COPY)
        {
            if ((pst_S->psz_source = strz_dup(psz_source)) != NULL)
            {
                pst_S->psz_on = pst_S->psz_source;
            }
            else
                pst_S = (PSUBSTRS)strz_substrs_destroy((HSUBSTRS)pst_S);
        }
        else
            pst_S->psz_on = (PSTR)psz_source;
    }

    RETURN((HSUBSTRS)pst_S);
}
// ***************************************************************************
// FUNCTION
//      strz_substrs
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_tgt        --
//      UINT   u_tgt          --
//      PSTR   psz_src        --
//      UINT   u_src          --
//      CHAR   c_sep          --
//      BOOL   b_sep_is_field --
// RESULT
//      PSTR  --
// ***************************************************************************
PSTR strz_substrs_get(HSUBSTRS    h_substr)
{
    PSTR            psz_result      = NULL;
    PSUBSTRS        pst_S           = (PSUBSTRS)h_substr;

    ENTER(pst_S);

    if (pst_S->u_len)
    {
        psz_result  = pst_S->psz_on;

        while (pst_S->u_len)
        {
            if (*pst_S->psz_on)
            {
                pst_S->u_len--;

                if (*pst_S->psz_on == pst_S->c_sep)
                {
                    *(pst_S->psz_on++)  = 0;
                    break;
                }
                else
                    pst_S->psz_on++;
            }
            else
                pst_S->u_len = 0;
        }
    }

/*
    if (psz_result)
    {
        A7TRACE("[[[[[ '%s' ]]]]]\n", psz_result);
    }
    else
        A7TRACE("[[[[[ OVER!!! ]]]]]\n");
*/
    RETURN(psz_result);
}
// ***************************************************************************
// FUNCTION
//      strz_substrs_sep
// PURPOSE
//
// PARAMETERS
//      HSUBSTRS   h_substr --
//      CHAR       c_sep    --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL strz_substrs_sep(HSUBSTRS      h_substr,
                      CHAR          c_sep)
{
    BOOL            b_result        = false;
    PSUBSTRS        pst_S           = (PSUBSTRS)h_substr;

    ENTER(true);

    pst_S->c_sep    = c_sep;
    b_result        = true;

    RETURN(b_result);
}


// ***************************************************************************
// FUNCTION
//      strz_substrs
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_tgt        --
//      UINT   u_tgt          --
//      PSTR   psz_src        --
//      UINT   u_src          --
//      CHAR   c_sep          --
//      BOOL   b_sep_is_field --
// RESULT
//      PSTR  --
// ***************************************************************************
PSTR strz_substrs_get_u(PSTR    str,
                        PUINT   plen,
                        CHAR    sep)
{
    PSTR            psz_result      = NULL;
    PSTR            on;
    UINT            left;

    ENTER(str && plen);

    if (*plen)
    {
        psz_result  = str;
        on          = str;
        left        = *plen;

        while (left)
        {
            if (*on)
            {
                if (*on == sep)
                {
                    *on = 0;
                    break;
                }

                on++;
                left--;
            }
            else
            {
                psz_result  = (++on);
                *(plen)     = (--left);
            }
        }

        if (!left)
        {
            *(plen) = 0;
        }
    }

    RETURN(psz_result);
}
PSTR strz_substrs_get_instant(PSTR    str,
                              PUINT   plen,
                              CHAR    sep,
                              CHAR    quote)
{
    PSTR            psz_result      = NULL;
    PSTR            on;
    UINT            left;
    BOOL            quoteState      = false;

    ENTER(str && plen);

    if (*plen)
    {
        psz_result  = str;
        on          = str;
        left        = *plen;

        while (left)
        {
            if (*on)
            {
                if (*on == quote)
                {
                    quoteState = !quoteState;
                }

                if ((*on == sep) && !quoteState)
                {
                    *on = 0;
                    break;
                }

                on++;
                left--;
            }
            else
            {
                psz_result  = (++on);
                *(plen)     = (--left);
            }
        }

        if (!left)
        {
            *(plen) = 0;
        }
    }

    RETURN(psz_result);
}
void strz_unquote(PSTR        t,
                  PSTR        s,
                  UINT        len)
{
    char quote = 0;

    while (*s && --len)
    {
        if (quote)
        {
            if (*s == quote)
            {
                quote = 0;
            }
            else
                *(t++) = *s;
        }
        else
        {
            if (('\"' == *s) || ('\'' == *s) || ('`' == *s))
            {
                quote = *s;
            }
            else
                *(t++) = *s;
        }

        s++;
    }

    *t = 0;
}
// ***************************************************************************
// FUNCTION
//      la6_substring_get
// PURPOSE
//      Get sub-string from string
// PARAMETERS
//      PSTR  psz_tgt_string -- Pointer to target string
//      PSTR  psz_src_string -- Pointer to source string
//      UINT d_tgt_size     -- Size of target string
//      BYTE  b_separator    -- Separator character
// RESULT
//      PSTR  -- Pointer to next symbol after separater
// ***************************************************************************
PSTR strz_substring_get(PSTR    psz_tgt_string,
                        PSTR    psz_src_string,
                        UINT    d_tgt_size,
                        CHAR    b_separator)
{
    PSTR    psz_target = psz_tgt_string;

    if (psz_tgt_string && psz_src_string && *psz_src_string && d_tgt_size)
    {
        *psz_target = 0;

        // Passing leading spaces...
        while (*psz_src_string && (*((PU8)psz_src_string) < 33) && (*((PU8)psz_src_string) != b_separator))
            psz_src_string++;

        // Copying string...
        while (*psz_src_string && *((PU8)psz_src_string) != b_separator && --d_tgt_size)
            *(psz_target++) = *(psz_src_string++);

        // Cutting followed spaces
        while (psz_target != psz_tgt_string && *((PU8)(psz_target - 1)) < 33)
            psz_target--;
        *psz_target = 0;

        // If target string shorter than source
        if (!d_tgt_size)
        {
            while (*((PU8)psz_src_string) != b_separator && *psz_src_string)
                psz_src_string++;
        }

        // Skip separator if neccessary
        if (*psz_src_string)
            psz_src_string++;

//        if (!*psz_tgt_string)
//            psz_src_string = NULL;
    }
    else
        psz_src_string = NULL;

    return psz_src_string;
}

// ***************************************************************************
// FUNCTION
//      la6_substring_get_w_quotes
// PURPOSE
//      Get sub-string from string with quotes
// PARAMETERS
//      PSTR  psz_tgt_string -- Pointer to target string
//      PSTR  psz_src_string -- Pointer to source string
//      UINT d_tgt_size     -- Size of target string
//      BYTE  b_separator    -- Separator character
//      BYTE  b_quote        -- Quote character
// RESULT
//      PSTR  -- Pointer to next symbol after separater
// ***************************************************************************
PSTR strz_substring_get_w_quotes(PSTR    psz_tgt_string,
                                 PSTR    psz_src_string,
                                 UINT    d_tgt_size,
                                 CHAR    b_separator,
                                 CHAR    b_quote)
{
    PSTR    psz_target = psz_tgt_string;

    if (psz_tgt_string && psz_src_string && *psz_src_string && d_tgt_size)
    {
        // Passing leading spaces...
        while (*psz_src_string && (*((PU8)psz_src_string) < 33))
            psz_src_string++;

        if (b_quote && (*((PU8)psz_src_string) == b_quote))
        {
            // Copying string until quote...
            psz_src_string++;
            while (*psz_src_string && (*((PU8)psz_src_string) != b_quote))
            {
                if (d_tgt_size)
                {
                    *(psz_target++) = *(psz_src_string++);
                    d_tgt_size--;
                }
            }

            if (*((PU8)psz_src_string) == b_quote)
                psz_src_string++;

            if (*((PU8)psz_src_string) == b_separator)
                psz_src_string++;

            *psz_target = 0;
        }
        else
            psz_src_string = strz_substring_get(psz_tgt_string,
                                psz_src_string, d_tgt_size, b_separator);
    }
    else
        psz_src_string = NULL;

    return psz_src_string;
}

// ***************************************************************************
// FUNCTION
//      la6_substring_get_std
// PURPOSE
//      Get sub-string from space separated string with quotes and C-escaping
// PARAMETERS
//      PSTR  psz_tgt_string -- Pointer to target string
//      PSTR  psz_src_string -- Pointer to source string
//      UINT  u_tgt_size     -- Size of target string
// RESULT
//      PSTR  -- Pointer to next symbol after separater
// ***************************************************************************
PSTR strz_substring_get_std(PSTR         psz_tgt_string,
                           PSTR         psz_src_string,
                           UINT         u_tgt_size)
{
    PU8       p_on_tgt    = (PU8)psz_tgt_string;
    PU8       p_on_src    = (PU8)psz_src_string;
    UINT        u_left      = u_tgt_size;
    BOOL        b_escape    = false;


    if (p_on_tgt && p_on_src && *p_on_src && u_left)
    {
// ----------------------- Passing leading spaces ----------------------------

        while (*p_on_src && (*p_on_src < '!'))
        {
            p_on_src++;
        }

        if (*p_on_src == '\"')
        {
// --------------------- Copying string until quote --------------------------

            p_on_src++;

            // Note: part of source sub-string that more than
            //       u_left will be skied

            while (*p_on_src && (b_escape || (*p_on_src != '\"')))
            {
                b_escape = (BOOL)(*p_on_src == '\\');

                if (u_left)
                {
                    *(p_on_tgt++) = *(p_on_src++);
                    u_left--;
                }
            }

            if (*((PU8)p_on_src) == '\"')
                p_on_src++;

            if (*((PU8)p_on_src) == ' ')
                p_on_src++;

            *p_on_tgt = 0;
        }
        else
            p_on_src = (PU8)strz_substring_get(psz_tgt_string, psz_src_string, u_left, ' ');
    }
    else
        p_on_src = NULL;

    return (PSTR)p_on_src;
}


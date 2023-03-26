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
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

// ***************************************************************************
// FUNCTION
//      strz_is_num
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_str --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL strz_is_num(PCSTR psz_str)
{
    BOOL            b_result        = false;
    PCSTR           psz_on;

    ENTER(true);

    psz_on = psz_str;

    while (*psz_on && ((*psz_on >= '0') && (*psz_on <= '9')))
    {
        psz_on++;
    }

    b_result = !(*psz_on);

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      strz_is_float
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_str --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL strz_is_float(PCSTR psz_str)
{
    BOOL            b_result        = false;
    PCSTR           psz_on;

    ENTER(true);

    psz_on = psz_str;

    while (*psz_on && (((*psz_on >= '0') && (*psz_on <= '9')) || (*psz_on == '.') || (*psz_on == '-') || (*psz_on == '+')))
    {
        psz_on++;
    }

    b_result = !(*psz_on);

    RETURN(b_result);
}
BOOL strz_is_percent_n_cut(PSTR psz_str)
{
    BOOL        result          = false;
    int         len;
    PSTR        on;

    ENTER(true);

    if ((len = strlen(psz_str)) > 0)
    {
        on = psz_str + len - 1;

        if (*on == '%')
        {
            *on = 0;

            result = true;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      strz_is_num
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_str --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL strz_is_hex(PCSTR psz_str)
{
    BOOL            b_result        = false;
    PCSTR           psz_on;

    ENTER(true);

    psz_on = psz_str;

    while (  *psz_on &&
             (  ((*psz_on >= '0') && (*psz_on <= '9'))                      ||
                (((*psz_on | 0x20) >= 'a') && ((*psz_on | 0x20) <= 'f'))    ))
    {
        psz_on++;
    }

    b_result = !(*psz_on);

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      strz_is_num
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_str --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL strz_is_ip(PCSTR   psz_str,
                INT     octets)
{
    BOOL            b_result        = false;
    PCSTR           psz_on;
    INT             found           = 1;
    UINT            num             = AXII;

    ENTER(true);

    psz_on      = psz_str;
    b_result    = true;

    while (b_result && *psz_on)
    {
        if ((*psz_on >= '0') && (*psz_on <= '9'))
        {
            if (num == AXII)
                num = 0;

            num = (num * 10) + (*psz_on - '0');
        }
        else
        {
            if ((*psz_on == '.') && (num < 256))
            {
                num = AXII;
                found++;
            }
            else
                b_result = false;
        }

        psz_on++;
    }

    if (  b_result                                  &&
          ((found != octets) || (num > 255)  )   )
    {
        b_result = false;
    }

    RETURN(b_result);
}
BOOL strz_is_MSISDN(PCSTR              psz_str)
{
    return !strz_mem_not_in_set(psz_str, strlen(psz_str), "+0123456789");
}

BOOL strz_is_urn(PCSTR              psz_str)
{
    BOOL        result          = false;

    ENTER(true);

    if (!strz_mem_not_in_set(psz_str, strlen(psz_str),
                            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.:-"))
    {
        result = true;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      strz_is_printable
// PURPOSE
//
// PARAMETERS
//      PCSTR psz_str --
//      INT   size    --
// RESULT
//      BOOL
//            true:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL strz_is_printable(PCSTR              psz_str,
                       INT                size)
{
    BOOL            b_result        = false;
    PU8             on;
    INT             left;

    ENTER(true);

    on          = (PU8)psz_str;
    left        = size;

    for (b_result = true; b_result && *on; on++, left--)
    {
        if (*on < ' ')
        {
            b_result = false;
        }
    }

    if (b_result && *on)
    {
        b_result = false;
    }

    RETURN(b_result);
}

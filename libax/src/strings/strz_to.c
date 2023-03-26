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

#include <arsenal.h>

#include <axstring.h>

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// ---------------------------------------------------------------------------

typedef struct __ATOLPFX
{
    long        l_mp;
    PSTR        psz_perfix;
} PREFIX;

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      _cmp
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PU8   p_src1PU8     p_src2 --
// RESULT
//      BOOL --
// ***************************************************************************
static  BOOL _cmp(PVOID     p_src1,
                  PVOID     p_src2)
{
    BOOL            b_result        = false;
    PU8             p_on1;
    PU8             p_on2;

    ENTER(true);

    for (  p_on1 = p_src1, p_on2 = p_src2, b_result = true;
           b_result && *p_on1;
           p_on1++, p_on2++)
    {
        if (*p_on2)
        {
            if ((*p_on1 | 0x20) != (*p_on2 | 0x20))
            {
                b_result = false;
            }
        }
        else
        {
            if (*p_on1 > ' ')
            {
                b_result = false;
            }
            break;
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      _strz_to_postfix
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PSTR       psz_string --
//      PREFIX *   ast_pfx    --
// RESULT
//      long long --
// ***************************************************************************
static long _strz_to_postfix(long *       pl_mp,
                             PCSTR        psz_string,
                             PREFIX *     ast_pfx)
{
    long        l_value         = 0;
    PU8         p_on;
    UINT        i_idx;

    ENTER(true);

    // Leading spaces
    for (  p_on = (PU8)psz_string;
           *p_on && (*p_on <= ' ');
           p_on++)
    {
        ;
    }

    // Numeric part
    for ( ; *p_on && ((*p_on >= '0') && (*p_on <= '9') ); p_on++)
    {
        l_value = (l_value * 10) + (*p_on - '0');
    }

    // Middle spaces
    for ( ; *p_on && (*p_on <= ' '); p_on++)
    {
        ;
    }

    // Prefix

    if (*p_on)
    {
        for (i_idx = 0; !*pl_mp && ast_pfx[i_idx].l_mp; i_idx++)
        {
            if (_cmp(p_on, ast_pfx[i_idx].psz_perfix))
            {
                *pl_mp = ast_pfx[i_idx].l_mp;
            }
        }
    }
    else
        *pl_mp = 1;

    RETURN(l_value);
}
// ***************************************************************************
// FUNCTION
//      strz_to_bytes
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_string --
// RESULT
//      long long --
// ***************************************************************************
long long strz_to_bytes(PCSTR psz_string)
{
    long long l_result          = 0;
    long l_mp                   = 0;

    PREFIX      ast_pfx         [] =
    {
        {  1024,        "k"      },
        {  1024,        "kb"     },
        {  1024,        "kib"    },

        {  1048576,     "m"      },
        {  1048576,     "mb"     },
        {  1048576,     "mib"    },

        {  1073741824,  "g"      },
        {  1073741824,  "gb"     },
        {  1073741824,  "gib"    },

        {  0,           ""    }
    };

    ENTER(psz_string);

    l_result = _strz_to_postfix(&l_mp, psz_string, ast_pfx);

    RETURN(l_result * l_mp);
}
// ***************************************************************************
// FUNCTION
//      strz_to_seconds
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_string --
// RESULT
//      long  --
// ***************************************************************************
long strz_to_seconds(PCSTR psz_string)
{
    long        l_result        = 0;
    long        l_mp            = 0;

    PREFIX      ast_pfx         [] =
    {
        {  1,       "s"     },
        {  60,      "m"     },
        {  3600,    "h"     },
        {  86400,   "d"     },

        {  0,       ""      }
    };

    ENTER(psz_string);

    l_result = _strz_to_postfix(&l_mp, psz_string, ast_pfx);

    RETURN(l_result * l_mp);
}
// ***************************************************************************
// FUNCTION
//      strz_to_metric
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_string --
// RESULT
//      long  --
// ***************************************************************************
long strz_to_metric(PCSTR psz_string)
{
    long        l_result        = 0;
    long        l_mp            = 0;

    PREFIX      ast_pfx         [] =
    {
        {  1000,        "k"      },
        {  1000000,     "m"      },
        {  1000000000,  "g"      },

        {  0,           ""    }
    };

    ENTER(psz_string);

    l_result = _strz_to_postfix(&l_mp, psz_string, ast_pfx);

    RETURN(l_result * l_mp);
}
// ***************************************************************************
// FUNCTION
//      strz_to_date
// PURPOSE
//
// PARAMETERS
//      PAXDATE pst_time   --
//      PSTR     psz_string --
//      DWORD    u_flags    -- Flags
//                  DEFTFMT_DM:     Date faormat is day/month (YMD or DMY)
//                  DEFTFMT_MD:     Date faormat is month/day (YDM or MDY)
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
BOOL strz_to_date(PAXDATE   pst_time,
                  PCSTR     psz_string,
                  UINT      u_flags)
{
    BOOL            b_result        = false;
    int             i_1;
    int             i_2;
    int             i_3;
    int             i_4;
    int             i_swap;
    PCSTR           psz_on_string   = psz_string;
    CHAR            sz_substring    [ AXLMEDIUM + 1 ];


    if (pst_time && psz_on_string)
    {
        b_result = true;                    // Assuming success

        while (  b_result                                               &&
                 ((psz_on_string = strz_substring_get(sz_substring,
                        (PSTR)psz_on_string, AXLMEDIUM, ' ')) != NULL) )
        {
            if (*sz_substring)
            {
                if (    strchr(sz_substring, '/')   ||
                        strchr(sz_substring, '.')   ||
                        strchr(sz_substring, '-')   )
                {
// --------------------------- Looks like data -------------------------------

                    i_1 = i_2 = i_3 = 0;

                    if (  (sscanf(sz_substring,
                                "%d%*[/.-]%d%*[/.-]%d", &i_1, &i_2, &i_3) > 1)  &&
                          i_1                                                   &&
                          i_2                                                   )
                    {
                        if (i_1 > i_3)
                            MAC_SWAP(i_1, i_3, i_swap);

                        if (u_flags & (DEFTFMT_MD | DEFTFMT_DM))
                        {
                            if (u_flags & DEFTFMT_MD)
                                MAC_SWAP(i_1, i_2, i_swap);
                        }
                        else
                        {
                            if (i_2 > 12)
                                MAC_SWAP(i_1, i_2, i_swap);
                        }

                        if (i_1 && (i_1 < 32) && i_2 && (i_2 < 13) && i_3)
                        {
                            pst_time->day     = i_1;
                            pst_time->month   = i_2;
                            pst_time->year    = i_3;
                        }
                        else
                            b_result = false;
                    }
                }
                else
                {
                    if (strchr(sz_substring, ':'))
                    {
// --------------------------- Looks like time -------------------------------

                        i_1 = i_2 = i_3 = i_4 = 0;

                        if (sscanf(sz_substring, "%d%*[:]%d%*[:]%d%*[.]%d",
                                                    &i_1, &i_2, &i_3, &i_4) > 1)
                        {
                            if ((i_1 < 24) && (i_2 < 60) && (i_3 < 60))
                            {
                                pst_time->hour            = i_1;
                                pst_time->minute          = i_2;
                                pst_time->second          = i_3;
                                pst_time->milliseconds    = i_4;
                            }
                            else
                                b_result = false;
                        }
                        else
                            b_result = false;
                    }
                    else
                        b_result = false;
                }
            }
        }
    }

    return b_result;
}

// ***************************************************************************
// FUNCTION
//      strz_to_date_fmt
// PURPOSE
//
// PARAMETERS
//      PAXDATE pst_time     --
//      PSTR     psz_string   --
//      PSTR     psz_fmt --
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
BOOL strz_to_date_fmt(PAXDATE       pst_time,
                      PSTR          psz_string,
                      PCSTR         psz_fmt)
{
#define MAC_ADD(a,b)            case a: pst_time->b *= 10; pst_time->b += \
                                (*(on++) - 48); break;

    BOOL            b_result        = false;
    PSTR            on;
    PSTR            ont;


    if (  pst_time                                  &&
          psz_string                                &&
          psz_fmt                                   &&
          (strlen(psz_string) >= strlen(psz_fmt))   )
    {
        on      = psz_string;
        ont    = (PSTR)psz_fmt;
        b_result        = true;

// --------------------------- Rendering time --------------------------------

        while (b_result && *on && *ont)
        {
            if ((*on >= '0') && (*on <= '9'))
            {
                switch (*ont)
                {
                    MAC_ADD('Y', year);
                    MAC_ADD('M', month);
                    MAC_ADD('D', day);
                    MAC_ADD('h', hour);
                    MAC_ADD('m', minute);
                    MAC_ADD('s', second);

                    default:
                            b_result = false;
                        break;
                }
            }
            else
            {
                if (*ont != *on)
                {
                    b_result = false;
                }
                else
                    on++;
            }

            ont++;
        }

// ------------------------- Result correctiong ------------------------------

        if (!*ont)
        {
            if (b_result)
            {
                if (  pst_time->year           &&
                      pst_time->month          &&
                      pst_time->day            &&
                      (pst_time->year < 100)   )
                {
                    pst_time->year += pst_time->year > 37 ? 1900 : 2000;
                }
            }
        }
        else
            b_result = false;
    }

    return b_result;

#undef  MAC_ADD
}
UINT strz_to_version(PCSTR              psz_str)
{
    UINT            result          = 0;
    INT             tmp;
    PSTR            on;
    UINT            octets          = 0;
    UINT            len;
    CHAR            buff            [ 16 ];

    ENTER(true);

    strz_cpy(buff, psz_str, sizeof(buff));

    len = (UINT)strlen(buff);
    on  = buff;

    while ((on = strz_substrs_get_u(on, &len, '.')) != nil)
    {
        tmp = atoi(on);

        if ((octets < 4) && (tmp >= 0) && (tmp < 256))
        {
            result = (result << 8) | (UINT) tmp;
            octets++;
        }
        else
        {
            result = 0;
            break;
        }
    }

    RETURN(result);
}
UINT strz_to_ip(PCSTR   psz_str)
{
    UINT            result          = 0;
    INT             tmp;
    PSTR            on;
    UINT            octets          = 0;
    UINT            len;
    CHAR            buff            [ 16 ];

    ENTER(true);

    strz_cpy(buff, psz_str, sizeof(buff));

    len = (UINT)strlen(buff);
    on  = buff;

    while ((on = strz_substrs_get_u(on, &len, '.')) != nil)
    {
        tmp = atoi(on);

        if ((octets < 4) && (tmp >= 0) && (tmp < 256))
        {
            result = (result << 8) | (UINT) tmp;
            octets++;
        }
        else
        {
            result = 0;
            break;
        }
    }

    result = MAC_BE_U32(result);

    RETURN(result);
}

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
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      strz_trim_s
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_string --
//      UINT   u_len      --
// RESULT
//      PSTR  --
// ***************************************************************************
PSTR strz_trim_s(PSTR   psz_string,
                 UINT   u_len)
{
    PU8      p_on;

    ENTER(psz_string && u_len);

    p_on = (PU8)(psz_string + u_len);

    while ((p_on != (PU8)psz_string) && (*(p_on - 1) <= ' '))
    {
        p_on--;
    }

    *p_on = 0;

    RETURN(psz_string);
}
// ***************************************************************************
// FUNCTION
//      strz_trim
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_string --
// RESULT
//      PSTR  --
// ***************************************************************************
PSTR strz_trim(PSTR psz_string)
{
    UINT    u_len;

    ENTER(psz_string);

    if ((u_len = strlen(psz_string)) > 0)
    {
        strz_trim_s(psz_string, u_len);
    }

    RETURN(psz_string);
}
// ***************************************************************************
// FUNCTION
//      strz_clean
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_string --
// RESULT
//      PSTR  --
// ***************************************************************************
PSTR strz_clean(PSTR psz_string)
{
    PU8          p_result        = NULL;
    PU8          p_on;
    UINT         u_len;

    ENTER(psz_string);

    p_result    = (PU8)strz_notspace(psz_string, -1, false);
    p_on        = p_result;

    if (*p_on && ((u_len = strlen((PSTR)p_on)) > 0))
    {
        p_on       += u_len;

        while (  (p_on != p_result)     &&
                 (*(p_on - 1) <= ' ')   )
        {
            p_on--;
        }

        *p_on = 0;
    }

    RETURN((PSTR)p_result);
}
// ***************************************************************************
// FUNCTION
//      strz_size
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_string --
// RESULT
//      UINT  --
// ***************************************************************************
UINT strz_size(PCSTR    psz_string,
               UINT     u_size)
{
    UINT            u_result    = 1;

    ENTER(psz_string && u_size);

    u_result += strz_len(psz_string, u_size);

    RETURN(u_result);
}
// ***************************************************************************
// FUNCTION
//      strz_size
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_string --
// RESULT
//      UINT  --
// ***************************************************************************
UINT strz_len(PCSTR     psz_string,
              UINT      u_size)
{
    UINT            u_result       = 0;
    PSTR            on;

    ENTER(psz_string && u_size);

    if ((on = (PSTR)memchr(psz_string, 0, u_size)) != NULL)
    {
        u_result = on - psz_string;
    }
    else
        u_result = u_size;

    RETURN(u_result);
}
// ***************************************************************************
// FUNCTION
//      strz_dup
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_string --
// RESULT
//      PSTR  --
// ***************************************************************************
PSTR strz_dup(PCSTR psz_string)
{
    PSTR        psz_copy    = NULL;
    UINT        u_size;

    ENTER(psz_string);

    if ((u_size = strlen(psz_string)) > 0)
    {
        u_size++;

        if ((psz_copy = MALLOC(u_size)) != NULL)
        {
            memcpy(psz_copy, psz_string, u_size);
        }
    }

    RETURN(psz_copy);
}
// ***************************************************************************
// FUNCTION
//      strz_cpy
// PURPOSE
//
// PARAMETERS
//      PSTR    psz_target --
//      PCSTR   psz_source --
//      UINT    u_size     --
// RESULT
//      PSTR  --
// ***************************************************************************
PSTR strz_cpy(PSTR      psz_target,
              PCSTR     psz_source,
              UINT      u_size)
{
    PSTR        psz_result      = NULL;
    PSTR        psz_on          = (PSTR)psz_source;
    PSTR        psz_on_tgt      = psz_target;
    UINT        u_left          = u_size;

    ENTER(psz_on_tgt && psz_on && u_left);

    while ((--u_left) && *psz_on)
    {
        *(psz_on_tgt++) = *(psz_on++);
    }

    *psz_on_tgt = 0;
    psz_result  = psz_on_tgt;

    RETURN(psz_result);
}
// ***************************************************************************
// FUNCTION
//      strz_copy
// PURPOSE
//
// PARAMETERS
//      PSTR    psz_target --
//      UINT    u_tgt_size --
//      PCSTR   psz_source --
//      UINT    u_src_size --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL strz_copy(PSTR               psz_target,
               UINT               u_tgt_size,
               PCSTR              psz_source,
               UINT               u_src_size)
{
    BOOL            b_result        = false;
    PSTR            psz_ont         = psz_target;
    UINT            u_leftt         = u_tgt_size;
    PSTR            psz_ons         = (PSTR)psz_source;
    UINT            u_lefts         = u_src_size ? u_src_size : (UINT)-1;

    ENTER(psz_ont && psz_ons && u_leftt);

    while ((--u_leftt) && (--u_lefts) && *psz_ons)
    {
        *(psz_ont++) = *(psz_ons++);
    }

    *psz_ont = 0;
    b_result = (BOOL)(0 == *psz_ons);

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      strz_cat
// PURPOSE
//
// PARAMETERS
//      PSTR  psz_target --
//      PCSTR psz_source --
//      UINT  u_size     --
// RESULT
//      PSTR  --
// ***************************************************************************
PSTR  strz_cat(PSTR               psz_target,
               PCSTR              psz_source,
               UINT               u_size)
{
    PSTR        psz_result      = NULL;
    PSTR        psz_on          = (PSTR)psz_source;
    PSTR        psz_on_tgt;
    UINT        u_left;

    ENTER(psz_target && psz_source && u_size);

    u_left      = strz_len(psz_target, u_size);
    psz_on_tgt  = (PSTR)psz_target + u_left;
    u_left      = u_size - u_left;

    while ((--u_left) && *psz_on)
    {
        *(psz_on_tgt++) = *(psz_on++);
    }

    *psz_on_tgt = 0;
    psz_result  = psz_on_tgt;

    RETURN(psz_result);
}
// ***************************************************************************
// FUNCTION
//      strz_mem
// PURPOSE
//
// PARAMETERS
//      PVOID data --
//      UINT  size --
// RESULT
//      PSTR --
// ***************************************************************************
PSTR strz_mem(PVOID     data,
              UINT      size)
{
    PSTR            result        = NULL;

    ENTER(data && size);

    if ((result = (PSTR)MALLOC(size + sizeof(CHAR))) != NULL)
    {
        memcpy(result, data, size);
        *(result + size) = 0;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      strz_check
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PVOID   p_string --
//      UINT    u_size   --
// RESULT
//      BOOL --
// ***************************************************************************
BOOL strz_check(PCSTR       psz_string,
                UINT        u_size)
{
    BOOL            b_result        = false;

    ENTER(psz_string && u_size);

    b_result = memchr(psz_string, u_size, 0) ? true : false;

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      strz_esc_expand
// PURPOSE
//      Expand string with C-like character sequences
// PARAMETERS
//      PSTR psz_string -- Pointer to target string
// RESULT
//      PSTR  -- Value of psz_string
// ***************************************************************************
PSTR strz_esc_expand(PSTR psz_string)
{
    PU8      psz_on;
    PSTR    psz_tmp;

    ENTER(psz_string);

    for (psz_on = (PU8)psz_string; *psz_on; psz_on++)
    {
        if (*psz_on == '\\')
        {
            switch (*(psz_on + 1))
            {
                case '\\':  *psz_on = '\\';    break;
                case '\"':  *psz_on = '\"';    break;
                case 'n':   *psz_on = '\n';    break;
                case 'r':   *psz_on = '\r';    break;
                case 't':   *psz_on = '\t';    break;
            }

            psz_tmp = (PSTR)psz_on + (sizeof(CHAR) << 1);
            memmove((PSTR)psz_on + sizeof(CHAR), psz_tmp, strlen(psz_tmp) + sizeof(CHAR));
        }
    }

    RETURN(psz_string);
}
// ***************************************************************************
// FUNCTION
//      strz_url_decode
// PURPOSE
//
// PARAMETERS
//      PSTR  target --
//      INT   size   --
//      PCSTR source --
// RESULT
//      INT --
// ***************************************************************************
INT strz_url_decode(PSTR               target,
                    INT                size,
                    PCSTR              source)
{
    INT     result  = -1;
    PCSTR   ons     = source;
    PSTR    ont     = target;
    UINT    len     = strlen(source);

    for (; *ons && len; ons++, len--)
    {
        switch (*ons)
        {
            case '%':
                *(ont++) = (MAC_HEX2BIN(*(ons + 1)) << 4) |
                           (MAC_HEX2BIN(*(ons + 2))     ) ;

                ons += 2;
                break;

            case '+':
                *(ont++) = ' ';
                break;

            default:
                *(ont++) = *ons;
                break;
        }
    }

    *ont    = 0;
    result  = ont - target;

    return result;
}
INT strz_url_encode(PSTR target,
                    INT  size,
                    PCSTR source)
{
    INT     result  = 0;
    PCSTR   ons     = source;
    PSTR    ont     = target;
    INT     left    = size - 1;
    UINT    len     = strlen(source);

    for (; !result && *ons && len && left; ons++, len--, left--)
    {

		if( (('a' <= *ons) && (*ons <= 'z')) ||
		    (('A' <= *ons) && (*ons <= 'Z')) ||
		    (('0' <= *ons) && (*ons <= '9')) )
        {
            *(ont++) = *ons;
		} 
        else if (left > 2)
        {
            *(ont++) = '%';
            *(ont++) = MAC_BIN2HEX((*ons) >> 4);
            *(ont++) = MAC_BIN2HEX((*ons));
            left -= 2;
		}
        else
            result = -1;
	}

    if (!result)
    {
        *ont    = 0;
        result  = ont - target;
    }

    return result;
}

// ***************************************************************************
// FUNCTION
//      strz_memdup
// PURPOSE
//
// PARAMETERS
//      PVOID data --
//      UINT  size --
// RESULT
//      PVOID --
// ***************************************************************************
PVOID strz_memdup(PCVOID    data,
                  UINT      size)
{
    PVOID           result          = nil;

    ENTER(true);

    if ((result = MALLOC(size)) != nil)
    {
        memcpy(result, data, size);
    }

    RETURN(result);
}
PSTR  strz_mem2strz(PCVOID             data,
                    UINT               size)
{
    PSTR            result          = nil;

    ENTER(true);

    if ((result = (PSTR)MALLOC(size + 1)) != nil)
    {
        memcpy(result, data, size);
        *(result + size) = 0;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      strz_xml_unescape
// PURPOSE
//
// PARAMETERS
//      PSTR str --
// RESULT
//      void --
// ***************************************************************************
void strz_xml_unescape(PSTR           str)
{
    PSTR            amp             = str;
    PSTR            wd;
    PSTR            sem;
    UINT            len;

    ENTER(true);

    while ((amp = strchr(amp, '&')) != nil)
    {
        if ((sem = strchr(amp, ';')) != nil)
        {
            // 0123
            // &lt;

            wd  = amp + 1;
            len = sem - wd;

            if ((len == 2) && !memcmp(wd, "lt", 2))
            {
                *amp = '<';
            }
            else if ((len == 2) && !memcmp(wd, "gt", 2))
            {
                *amp = '>';
            }
            else if ((len == 3) && !memcmp(wd, "amp", 3))
            {
                *amp = '&';
            }
            else if ((len == 4) && !memcmp(wd, "quot", 4))
            {
                *amp = '"';
            }
            else if ((len == 4) && !memcmp(wd, "apos", 4))
            {
                *amp = '\'';
            }
            else
                wd = 0;

            if (wd)
            {
                strz_cpy(wd, sem + 1, -1);
            }
        }

        amp++;
    }

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      strz_xml_unescape2
// PURPOSE
//
// PARAMETERS
//      PCSTR str --
// RESULT
//      PCSTR --
// ***************************************************************************
PCSTR strz_xml_unescape2(PCSTR              str)
{
    PSTR            result          = false;

    ENTER(true);

    if ((result = strz_dup(str)) != nil)
    {
        strz_xml_unescape(result);
    }

    RETURN((PCSTR)result);
}
// ***************************************************************************
// FUNCTION
//      strz_replace
// PURPOSE
//
// PARAMETERS
//      PSTR psz_string --
//      CHAR from       --
//      CHAR to         --
// RESULT
//      PSTR --
// ***************************************************************************
PSTR strz_replace(PSTR  psz_string,
                  CHAR  from,
                  CHAR  to)
{
    PU8      on;

    ENTER(psz_string);

    for (on = (PU8)psz_string; *on; on++)
    {
        if (*on == from)
        {
            *on = to;
        }
    }

    RETURN(psz_string);
}
// ***************************************************************************
// FUNCTION
//      strz_notchar
// PURPOSE
//
// PARAMETERS
//      PCSTR psz_string --
//      UINT  len        --
//      CHAR  validChar  --
// RESULT
//      PSTR --
// ***************************************************************************
PSTR strz_notchar(PCSTR     psz_string,
                  UINT      len,
                  CHAR      validChar)
{
    PSTR            result          = nil;
    PCSTR           on              = psz_string;
    UINT            left            = len;

    ENTER(true);

    while (left && *on && !result)
    {
        if (*on != validChar)
        {
            result = (PSTR)on;
        }
        else
            on++;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      strz_notspace
// PURPOSE
//
// PARAMETERS
//      PCSTR psz_string --
//      UINT  len        --
// RESULT
//      PSTR --
// ***************************************************************************
//PSTR strz_notspace(PCSTR     psz_string,
//                   UINT      len)
//{
//    PU8             result          = (PU8)psz_string;
//    UINT            left            = len;
//
//    ENTER(true);
//
//    for (; *result && (*result <= ' ') && left; result++, left--)
//    {
//        ;
//    }
//
//    RETURN((PSTR)result);
//}
// ***************************************************************************
// FUNCTION
//      strz_notspace
// PURPOSE
//
// PARAMETERS
//      PCSTR psz_string    --
//      UINT  len           --
//      BOOL  b_stop_at_eol --
// RESULT
//      PCSTR --
// ***************************************************************************
PCSTR strz_notspace(PCSTR         psz_string,
                    UINT         len,
                    BOOL         b_stop_at_eol)
{
    PCSTR           result   = psz_string;
    UINT            left            = len;

    ENTER(result);

    while (  *result                                &&
             left--                                 &&
             (*(PU8)result <= ' ')                  &&
             (!b_stop_at_eol || (*result != '\n'))  )
    {
        result++;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      strz_strz_memnchr
// PURPOSE
//
// PARAMETERS
//      PVOID data  --
//      UINT  len   --
//      U8    valid --
// RESULT
//      PVOID --
// ***************************************************************************
PVOID strz_memnchr(PVOID     data,
                   UINT      len,
                   U8        valid)
{
    PVOID           result          = nil;
    PU8             on              = data;
    UINT            left            = len;

    ENTER(true);

    while (left-- && !result)
    {
        if (*on != valid)
        {
            result = on;
        }
        else
            on++;
    }

    RETURN(result);
}
PVOID strz_mem_not_in_set(PVOID              data,
                          UINT               len,
                          PCSTR              validSet)
{
    PVOID           result          = nil;
    PU8             on              = data;
    UINT            left            = len;
    UINT            setLen          = strlen(validSet);

    ENTER(true);

    while (left-- && !result)
    {
        //printf("NOT IN SET: stage 1 '%c'\n", *on);
        if (!memchr(validSet, *on, setLen))
        {
            result = on;
        }
        else
            on++;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      strz_eol
// PURPOSE
//
// PARAMETERS
//      PCSTR psz_string --
//      UINT  size       --
// RESULT
//      PCSTR --
// ***************************************************************************
PCSTR strz_eol(PCSTR       psz_string,
               UINT       size)
{
    PCSTR           result          = (PSTR)psz_string;

    ENTER(psz_string && size);

    while ( (*result    != '\n') &&
            ((size--)  !=  0) )
    {
        result++;
    }

    result++;

    RETURN(result);
}

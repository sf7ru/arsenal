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
//
// FILE RULES
//      Strings always terminated by one zero for ASCIIZ compatibility
//      u_used contains size of string, real size equals u_used + FOOTER_SZ
// ***************************************************************************

#include <stdlib.h>
#include <string.h>

#include <arsenal.h>
#include <axstring.h>
#include <pvt_string.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      _resize
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PAXSTRING   pst_target --
//      UINT        u_used     --
// RESULT
//      BOOL --
// ***************************************************************************
static BOOL _resize(PAXSTRING       pst_target,
                    UINT            u_used)
{
    BOOL            b_result        = true;
    UINT            u_new_size;
    PVOID           p_new_data;

    ENTER(true);

    u_new_size = u_used + FOOTER_SZ;

    if (!pst_target->u_max || (pst_target->u_max < u_new_size))
    {
        if (pst_target->u_size < u_new_size)
        {
            if ((p_new_data = REALLOC(pst_target->p_data, u_new_size)) != NULL)
            {
                pst_target->u_size = u_new_size;
                pst_target->p_data = p_new_data;
            }
            else
                b_result = false;
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axstr_destroy
// PURPOSE
//
// PARAMETERS
//      HAXSTR      h_str --
// RESULT
//      HAXSTR     --
// ***************************************************************************
HAXSTR    axstr_destroy(HAXSTR    h_str)
{
    PAXSTRING   pst_str     = (PAXSTRING)h_str;

    ENTER(pst_str);

    if (pst_str->p_data)
    {
        FREE(pst_str->p_data);
    }

    FREE(pst_str);

    RETURN(NULL);
}
// ***************************************************************************
// FUNCTION
//      axstr_create
// PURPOSE
//
// PARAMETERS
//      UINT   u_initial --
// RESULT
//      HAXSTR     --
// ***************************************************************************
HAXSTR    axstr_create(UINT u_initial)
{
    PAXSTRING   pst_str     = NULL;

    ENTER(true);

    if ((pst_str = CREATE(AXSTRING)) != NULL)
    {
        pst_str->u_size = (u_initial < MINSTRING_SZ) ? u_initial : MINSTRING_SZ;

        if ((pst_str->p_data = MALLOC(pst_str->u_size)) != NULL)
        {
            *pst_str->p_data = 0;
        }
        else
            pst_str = (PAXSTRING)axstr_destroy((HAXSTR   )pst_str);
    }

    RETURN((HAXSTR   )pst_str);
}
// ***************************************************************************
// FUNCTION
//      axstr_dup
// PURPOSE
//
// PARAMETERS
//      HAXSTR      pst_source --
// RESULT
//      HAXSTR     --
// ***************************************************************************
HAXSTR     axstr_dup(HAXSTR     h_source)
{
    PAXSTRING   pst_str     = NULL;
    PAXSTRING   pst_source  = (PAXSTRING)h_source;

    ENTER(pst_source);

    if (pst_source->u_used)
    {
        if ((pst_str = CREATE(AXSTRING)) != NULL)
        {
            pst_str->u_size = pst_source->u_used + FOOTER_SZ;

            if ((pst_str->p_data = MALLOC(pst_str->u_size)) != NULL)
            {
                pst_str->u_used = pst_source->u_used;

                memcpy(pst_str->p_data, pst_source->p_data, pst_str->u_size);
            }
            else
                pst_str = (PAXSTRING)axstr_destroy((HAXSTR   )pst_str);
        }
    }

    RETURN((HAXSTR   )pst_str);
}
// ***************************************************************************
// FUNCTION
//      axstr_dup_c
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_source --
// RESULT
//      PAXSTRING  --
// ***************************************************************************
HAXSTR    axstr_dup_c(PSTR psz_source)
{
    PAXSTRING   pst_str     = NULL;
    UINT        u_used;

    ENTER(psz_source);

    if ((u_used = strlen(psz_source)) > 0)
    {
        if ((pst_str = CREATE(AXSTRING)) != NULL)
        {
            pst_str->u_size = u_used + FOOTER_SZ;

            if ((pst_str->p_data = MALLOC(pst_str->u_size)) != NULL)
            {
                pst_str->u_used = u_used;

                memcpy(pst_str->p_data, psz_source, u_used);
                memset(pst_str->p_data + u_used, 0, FOOTER_SZ);
            }
            else
                pst_str = (PAXSTRING)axstr_destroy((HAXSTR   )pst_str);
        }
    }

    RETURN((HAXSTR   )pst_str);
}
// ***************************************************************************
// FUNCTION
//      axstr_copy_c
// PURPOSE
//
// PARAMETERS
//      HAXSTR      h_target   --
//      PSTR        psz_source --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL axstr_copy_c(HAXSTR            h_target,
                  PSTR              psz_source)
{
    BOOL        b_result            = false;
    PAXSTRING   pst_target  = (PAXSTRING)h_target;
    UINT        u_used;

    ENTER(pst_target && psz_source);

    if ((u_used = strlen(psz_source)) > 0)
    {
        if (_resize(pst_target, u_used))
        {
            memcpy(pst_target->p_data, psz_source, u_used + 1);

            pst_target->u_used  = u_used;
            b_result            = true;
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axstr_add_c
// PURPOSE
//
// PARAMETERS
//      HAXSTR      h_target   --
//      PSTR        psz_source --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL axstr_add_c(HAXSTR         h_target,
                 PCSTR          psz_source)
{
    BOOL        b_result        = false;
    PAXSTRING   pst_target      = (PAXSTRING)h_target;
    UINT        u_used;
    UINT        u_new_used;

    ENTER(pst_target && psz_source);

    if ((u_used = strlen(psz_source)) > 0)
    {
        u_new_used = u_used + pst_target->u_used;

        if (_resize(pst_target, u_new_used))
        {
            memcpy(pst_target->p_data + pst_target->u_used,
                   psz_source,
                   u_used + 1);

            pst_target->u_used  = u_new_used;
            b_result            = true;
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axstr_add_char
// PURPOSE
//
// PARAMETERS
//      HAXSTR    h_target --
//      CHAR      c_char   --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL axstr_add_char(HAXSTR             h_target,
                    CHAR               c_char)
{
    BOOL        b_result        = false;
    PAXSTRING   pst_target      = (PAXSTRING)h_target;
    UINT        u_new_used;

    ENTER(pst_target && c_char);

    u_new_used = pst_target->u_used + sizeof(CHAR);

    if (_resize(pst_target, u_new_used + 1))
    {
        *(pst_target->p_data + pst_target->u_used)  = c_char;
        pst_target->u_used                          = u_new_used;
        *(pst_target->p_data + pst_target->u_used)  = 0;
        b_result                                    = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axstr_add
// PURPOSE
//
// PARAMETERS
//      HAXSTR      h_target --
//      HAXSTR      h_source --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL axstr_add(HAXSTR           h_target,
               HAXSTR           h_source)
{
    BOOL        b_result        = false;
    PAXSTRING   pst_target      = (PAXSTRING)h_target;
    PAXSTRING   pst_source      = (PAXSTRING)h_source;
    UINT        u_new_used;

    ENTER(pst_target && pst_source);

    if (pst_source->u_used)
    {
        u_new_used = pst_target->u_used + pst_source->u_used;

        if (_resize(pst_target, u_new_used))
        {
            memcpy(pst_target->p_data + pst_target->u_used,
                   pst_source->p_data,
                   pst_source->u_used + 1);

            pst_target->u_used  = u_new_used;
            b_result            = true;
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axstr_add_tabs
// PURPOSE
//
// PARAMETERS
//      HAXSTR    h_string --
//      UINT      tabs     --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL axstr_add_tabs(HAXSTR             h_string,
                    UINT               tabs)
{
    BOOL        b_result        = false;
    PAXSTRING   str             = (PAXSTRING)h_string;
    UINT        u_new_used;

    ENTER(str);

    if (tabs)
    {
        u_new_used = str->u_used + tabs;

        if (_resize(str, u_new_used))
        {
            memset(str->p_data + str->u_used,
                   '\t', tabs);

            str->u_used                     = u_new_used;
            *(str->p_data + str->u_used)    = 0;
            b_result                        = true;
        }
    }
    else
        b_result = true;

    RETURN(b_result);
}
BOOL axstr_add_tabs_w_spaces(HAXSTR             h_string,
                             UINT               tabs,
                             UINT               tabSize)
{
    BOOL        b_result        = false;
    PAXSTRING   str             = (PAXSTRING)h_string;
    UINT        u_new_used;
    UINT        size;

    ENTER(str);

    if (tabs && tabSize)
    {
        size        = tabs * tabSize;
        u_new_used = str->u_used + size;

        if (_resize(str, u_new_used))
        {
            memset(str->p_data + str->u_used,
                   ' ', size);

            str->u_used                     = u_new_used;
            *(str->p_data + str->u_used)    = 0;
            b_result                        = true;
        }
    }
    else
        b_result = true;

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axstr_add_raw
// PURPOSE
//
// PARAMETERS
//      HAXSTR    h_target --
//      PVOID     data     --
//      UINT      size     --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL axstr_add_raw(HAXSTR             h_target,
                   PVOID              data,
                   UINT               size)
{
    BOOL        b_result        = false;
    PAXSTRING   str             = (PAXSTRING)h_target;
    UINT        u_new_used;

    ENTER(str);

    u_new_used = str->u_used + size;

    if (_resize(str, u_new_used))
    {
        memcpy(str->p_data + str->u_used, data, size);

        str->u_used                     = u_new_used;
        *(str->p_data + str->u_used)    = 0;
        b_result                        = true;
    }

    RETURN(b_result);
}

// ***************************************************************************
// FUNCTION
//      axstr_pstr
// PURPOSE
//
// PARAMETERS
//      HAXSTR      h_source --
// RESULT
//      PSTR  --
// ***************************************************************************
PSTR axstr_c(HAXSTR    h_source)
{
    PSTR        psz_result      = NULL;
    PAXSTRING   pst_source      = (PAXSTRING)h_source;

    ENTER(pst_source);

    psz_result = (PSTR)pst_source->p_data;

    RETURN(psz_result);
}
// ***************************************************************************
// FUNCTION
//      axstr_len
// PURPOSE
//
// PARAMETERS
//      HAXSTR      h_source --
// RESULT
//      UINT  --
// ***************************************************************************
UINT axstr_len(HAXSTR    h_source)
{
    UINT        u_result        = 0;
    PAXSTRING   pst_source      = (PAXSTRING)h_source;

    ENTER(pst_source);

    u_result = pst_source->u_used;

    RETURN(u_result);
}
// ***************************************************************************
// FUNCTION
//      axstr_truncate
// PURPOSE
//
// PARAMETERS
//      HAXSTR h_source --
//      UINT   len      --
// RESULT
//      UINT --
// ***************************************************************************
UINT axstr_truncate(HAXSTR             h_source,
                    UINT               len)
{
    UINT        u_result        = 0;
    PAXSTRING   str      = (PAXSTRING)h_source;

    ENTER(str);

    if (len < str->u_used)
    {
        str->u_used                     = len;
        *(str->p_data + str->u_used)    = 0;
    }

    u_result = str->u_used;

    RETURN(u_result);
}
// ***************************************************************************
// FUNCTION
//      axstr_size
// PURPOSE
//
// PARAMETERS
//      HAXSTR      h_source --
// RESULT
//      UINT  --
// ***************************************************************************
UINT axstr_size(HAXSTR    h_source)
{
    UINT        u_result        = 0;
    PAXSTRING   pst_source      = (PAXSTRING)h_source;

    ENTER(pst_source);

    u_result = pst_source->u_used;

    if (u_result)
        u_result++;

    RETURN(u_result);
}
// ***************************************************************************
// FUNCTION
//      axstr_freeze_c
// PURPOSE
//
// PARAMETERS
//      HAXSTR    h_str --
// RESULT
//      PSTR --
// ***************************************************************************
PSTR axstr_freeze_c(HAXSTR             h_str)
{
    PSTR        result      = NULL;
    PAXSTRING   pst_str     = (PAXSTRING)h_str;

    ENTER(true);

    result                      = (PSTR)pst_str->p_data;
    *(result + pst_str->u_used) = 0;
    pst_str->p_data             = NULL;

    axstr_destroy(h_str);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      axstr_set_max_len
// PURPOSE
//
// PARAMETERS
//      HAXSTR    h_str     --
//      UINT      u_max_len --
// RESULT
//      void --
// ***************************************************************************
void axstr_set_max_len(HAXSTR             h_str,
                       UINT               u_max_len)
{
    PAXSTRING   pst_str     = (PAXSTRING)h_str;

    ENTER(true);

    pst_str->u_max = u_max_len;

    QUIT;
}

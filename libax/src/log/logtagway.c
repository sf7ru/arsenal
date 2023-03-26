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
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <arsenal.h>

#include <axstring.h>
#include <axfile.h>

#include <axlog.h>
#include <pvt_log.h>


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// STRUCTURE
//      PARWAY
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_TAGWAY
{
    AXLOGWAY        st_std;
    U32             flags;
    HRECSET         files;
    UINT            lifetime;
    HAXMUTEX        LCK;

    CHAR            basePath        [ AXLPATH ];
} TAGWAY, * PTAGWAY;


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _TAGWAY_msg
// PURPOSE
//
// PARAMETERS
//      PTAGWAY way      --
//      AXLOGDL v_tag    --
//      PAXDATE pst_date --
//      PSTR    psz_msg  --
// RESULT
//      void --
// ***************************************************************************
static void _TAGWAY_msg(PTAGWAY      way,
                        AXLOGDL      v_tag,
                        PAXDATE      pst_date,
                        PSTR         psz_msg)
{
    AXLOGDL             level;
    UINT                tag;
    PAXLOGWAY           fway;

    ENTER(true);

    if (v_tag < (1 << 8))
    {
        level   = v_tag;
        tag     = 0;
    }
    else
    {
        level   = v_tag & 0xFF;
        tag     = v_tag >> 8;
    }

    axmutex_lock(way->LCK, true);
    fway = (PAXLOGWAY)recset_get(way->files, tag);
    axmutex_unlock(way->LCK);

    if (!fway)
    {
        axmutex_lock(way->LCK, true);
        fway = (PAXLOGWAY)recset_get(way->files, 0);
        axmutex_unlock(way->LCK);
    }

    if (fway)
    {
        if (way->flags & AXLOGFL_CONSOLE)
        {
            printf("%s\n", psz_msg);
        }

        (*fway->pfn_msg)(fway, level, pst_date, psz_msg);
    }

    QUIT;
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _tagway_add
// PURPOSE
//
// PARAMETERS
//      PTAGWAY way       --
//      PCSTR   psz_fname --
// RESULT
//      UINT --
// ***************************************************************************
static UINT _tagway_add(PTAGWAY         way,
                        PCSTR           psz_fname)
{
    UINT            result          = AXII;
    HAXLOGWAY       fway;

    ENTER(true);

    if ((fway = axlog_file_way(way->basePath, psz_fname, way->lifetime, way->flags)) != nil)
    {
        axmutex_lock(way->LCK, true);
        result = recset_add(way->files, fway, 0);
        axmutex_unlock(way->LCK);

        if (result == AXII)
        {
            axlog_way_close(fway);
        }
    }

    RETURN(result);
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _TAGWAY_close
// PURPOSE
//
// PARAMETERS
//      PTAGWAY way --
// RESULT
//      PTAGWAY --
// ***************************************************************************
static PTAGWAY _TAGWAY_close(PTAGWAY way)
{
    ENTER(way);

    SAFEDESTROY(recset, way->files);

    SAFEDESTROY(axmutex, way->LCK);

    FREE(way);

    RETURN(NULL);
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _freeWay
// PURPOSE
//
// PARAMETERS
//      HAXLOGWAY fway --
// RESULT
//      HAXLOGWAY --
// ***************************************************************************
static HAXLOGWAY _freeWay(HAXLOGWAY     fway)
{
    if (fway)
    {
        axlog_way_close(fway);
    }

    return nil;
}
// ***************************************************************************
// FUNCTION
//      axlog_tagway
// PURPOSE
//
// PARAMETERS
//      PCSTR psz_path   --
//      UINT  u_lifetime --
//      UINT  u_flags    --
// RESULT
//      HAXLOGWAY --
// ***************************************************************************
HAXLOGWAY axlog_tagway(PCSTR              psz_path,
                       PCSTR              psz_default,
                       UINT               u_lifetime,
                       UINT               u_flags)
{
    PTAGWAY          pst_way             = NULL;

    ENTER(true);

    if ((pst_way = CREATE(TAGWAY)) != NULL)
    {
        if (  ((pst_way->LCK = axmutex_create()) != nil)                            &&
              ((pst_way->files = recset_create(5, (PFNRECFREE)_freeWay)) != nil)    )
        {
            strz_cpy(pst_way->basePath, psz_path, AXLPATH);

            pst_way->st_std.pfn_close   = (PFNLOGCLOSE)_TAGWAY_close;
            pst_way->st_std.pfn_msg     = (PFNLOGMSG)_TAGWAY_msg;
            pst_way->st_std.v_type      = AXLOGWT_tag;
            pst_way->lifetime           = u_lifetime;
            pst_way->flags              = u_flags;

            if (_tagway_add(pst_way, psz_default) == AXII)
            {
                pst_way = _TAGWAY_close(pst_way);
            }
        }
        else
            pst_way = _TAGWAY_close(pst_way);
    }

    RETURN((HAXLOGWAY)pst_way);
}
// ***************************************************************************
// FUNCTION
//      axlog_tagway_add
// PURPOSE
//
// PARAMETERS
//      HAXLOG h_log     --
//      PCSTR  psz_fname --
// RESULT
//      AXLOGDL --
// ***************************************************************************
AXLOGDL axlog_tagway_add(HAXLOG             h_log,
                         PCSTR              psz_fname)
{
    AXLOGDL         result          = 0;
    PAXLOG          log             = (PAXLOG)h_log;
    UINT            tag;
    PTAGWAY         way             = (PTAGWAY)log->pst_way;

    ENTER(psz_fname);

    if (way->st_std.v_type == AXLOGWT_tag)
    {
        if ((tag = _tagway_add(way, psz_fname)) != AXII)
        {
            result = (AXLOGDL)(tag << 8);
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      axlog_tagway_del
// PURPOSE
//
// PARAMETERS
//      HAXLOG  h_log --
//      AXLOGDL v_tag --
// RESULT
//      void --
// ***************************************************************************
void axlog_tagway_del(HAXLOG             h_log,
                      AXLOGDL            v_tag)
{
    PAXLOG          log             = (PAXLOG)h_log;
    UINT            index           = v_tag >> 8;
    PTAGWAY         way             = (PTAGWAY)log->pst_way;

    ENTER(true);

    if ((way->st_std.v_type == AXLOGWT_tag) && (index > 0))
    {
        axmutex_lock(way->LCK, true);
        recset_del(way->files, v_tag >> 8);
        axmutex_unlock(way->LCK);
    }

    QUIT;
}

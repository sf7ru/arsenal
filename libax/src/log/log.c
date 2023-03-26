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

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <arsenal.h>


#include <axstring.h>
#include <axlog.h>
#include <pvt_log.h>

// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      axlog_way_close
// PURPOSE
//
// PARAMETERS
//      HAXLOG   h_log --
// RESULT
//      HAXLOG  --
// ***************************************************************************
HAXLOGWAY axlog_way_close(HAXLOGWAY          h_way)
{
    PAXLOGWAY       pst_way = (PAXLOGWAY)h_way;

    ENTER(pst_way);

    if (pst_way->pfn_close)
    {
        (*pst_way->pfn_close)(pst_way);
    }

    RETURN(NULL);
}
// ***************************************************************************
// FUNCTION
//      axlog_close
// PURPOSE
//
// PARAMETERS
//      HAXLOG   h_log --
// RESULT
//      HAXLOG  --
// ***************************************************************************
HAXLOG axlog_close(HAXLOG h_log)
{
    PAXLOG         pst_log = (PAXLOG)h_log;

    ENTER(pst_log);

    if (pst_log->psz_msg)
        FREE(pst_log->psz_msg);

    if (pst_log->h_LCK)
        axmutex_destroy(pst_log->h_LCK);

    if (pst_log->pst_way)
        (*pst_log->pst_way->pfn_close)(pst_log->pst_way);

    FREE(pst_log);

    RETURN(NULL);
}
// ***************************************************************************
// FUNCTION
//      axlog_open
// PURPOSE
//
// PARAMETERS
//      HAXLOGWAY   h_way --
// RESULT
//      HAXLOG  --
// ***************************************************************************
HAXLOG axlog_open(HAXLOGWAY     h_way,
                  UINT          u_msgmax)
{
    PAXLOG      pst_log             = NULL;
    PAXLOGWAY   pst_way;

    ENTER(h_way && u_msgmax);

    if ((pst_way = (PAXLOGWAY)h_way) != NULL)
    {
        if ((pst_log = CREATE(AXLOG)) != NULL)
        {
            pst_log->pst_way    = pst_way;
            pst_log->u_size     = u_msgmax;

            if ( !( ((pst_log->h_LCK    =
                        axmutex_create())                != NULL)   &&
                    ((pst_log->psz_msg  =
                        MALLOC(u_msgmax + sizeof(CHAR))) != NULL)   ))
            {
                pst_log = (PAXLOG)axlog_close((HAXLOG)pst_log);
            }
        }
        else
            (*pst_way->pfn_close)(pst_way);
    }

    RETURN((HAXLOG)pst_log);
}
// ***************************************************************************
// FUNCTION
//      axlog_msg
// PURPOSE
//
// PARAMETERS
//      HAXLOG    h_log       --
//      AXLOGDL   v_level     --
//      PSTR      psz_message --
// RESULT
//      void  --
// ***************************************************************************
void axlog_msg(HAXLOG       h_log,
               AXLOGDL      v_level,
               PCSTR        psz_msg)
{
    PAXLOG          pst_log             = (PAXLOG)h_log;
    AXDATE          st_time;

    ENTER(pst_log && psz_msg);

    axdate_get_local(&st_time);

    axmutex_lock(pst_log->h_LCK, true);

    (*pst_log->pst_way->pfn_msg)(pst_log->pst_way, v_level, &st_time, psz_msg);

    axmutex_unlock(pst_log->h_LCK);

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      axlog_msg_vl
// PURPOSE
//
// PARAMETERS
//      HAXLOG    h_log      --
//      AXLOGDL   v_level    --
//      PSTR      psz_msg    --
//      va_list   st_va_list --
// RESULT
//      void  --
// ***************************************************************************
void axlog_msg_vl(HAXLOG        h_log,
                  AXLOGDL       v_level,
                  PCSTR         psz_msg,
                  va_list       st_va_list)
{
    PAXLOG          pst_log             = (PAXLOG)h_log;
    AXDATE          st_time;

    ENTER(pst_log && psz_msg);

    axdate_get_local(&st_time);

    axmutex_lock(pst_log->h_LCK, true);

    vsnprintf(pst_log->psz_msg, pst_log->u_size, psz_msg, st_va_list);

    (*pst_log->pst_way->pfn_msg)(pst_log->pst_way, v_level, &st_time, pst_log->psz_msg);

    axmutex_unlock(pst_log->h_LCK);

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      axlog_msg_v
// PURPOSE
//
// PARAMETERS
//      HAXLOG    h_log       --
//      AXLOGDL   v_level     --
//      PSTR      psz_message --
//      ...                   -- List of additional arguments
// RESULT
//      void  --
// ***************************************************************************
void axlog_msg_v(HAXLOG   h_log,
                 AXLOGDL  v_level,
                 PCSTR    psz_msg,
                 ...)
{
    va_list         st_va_list;

    ENTER(true);

    va_start(st_va_list, psz_msg);
    axlog_msg_vl(h_log, v_level, psz_msg, st_va_list);

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      axlog_get_type
// PURPOSE
//
// PARAMETERS
//      HAXLOG h_log --
// RESULT
//      AXLOGWT --
// ***************************************************************************
AXLOGWT axlog_get_type(HAXLOG             h_log)
{
    AXLOGWT         result          = AXLOGWT_none;
    PAXLOG          pst_log         = (PAXLOG)h_log;

    ENTER(true);

    result = pst_log->pst_way->v_type;

    RETURN(result);
}

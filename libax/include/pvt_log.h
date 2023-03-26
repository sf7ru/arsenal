// ***************************************************************************
// TITLE
//      Log  Maintenance Private Header
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: RClog_private.h,v 1.3 2003/05/09 09:33:01 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************


#ifndef __AXLOG_PRIVATEH__
#define __AXLOG_PRIVATEH__

#include <arsenal.h>
#include <axthreads.h>
#include <axlog.h>

// ---------------------------------------------------------------------------
// -------------------------------- TYPES ------------------------------------
// ---------------------------------------------------------------------------

typedef PVOID (*PFNLOGCLOSE)(PVOID);
typedef void (*PFNLOGMSG)(PVOID,AXLOGDL,PAXDATE,PCSTR);

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// STRUCTURE
//      AXLOGWAY
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_AXLOGWAY
{
    PFNLOGCLOSE     pfn_close;
    PFNLOGMSG       pfn_msg;
    AXLOGWT         v_type;
} AXLOGWAY, * PAXLOGWAY;

// ***************************************************************************
// STRUCTURE
//      AXLOG
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_AXLOG
{
    PAXLOGWAY       pst_way;
    HAXMUTEX        h_LCK;
    PSTR            psz_msg;
    UINT            u_size;
} AXLOG, * PAXLOG;

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif

void                constructLogNameFromTempl   (PSTR               filename,
                                                 UINT               len,
                                                 PSTR               templ,
                                                 PAXDATE            pst_date);


#ifdef __cplusplus
}
#endif

#endif                                      //  #define __AXLOG_PRIVATEH__

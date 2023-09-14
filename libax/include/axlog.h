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

#ifndef __AXLOG_H__
#define __AXLOG_H__

#include <arsenal.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define AXLOGFL_ROTATE          0x0001
#define AXLOGFL_NAMEDIR         0x0002
#define AXLOGFL_DATEDIR         0x0004
#define AXLOGFL_STAMP           0x0008
#define AXLOGFL_SHARED          0x0010
#define AXLOGFL_ICONSOLE        0x4000
#define AXLOGFL_CONSOLE         0x8000
#define AXLOGFL_DEFAULT         (AXLOGFL_STAMP)

#define AXLOGMPWAY_MAXFILES     4

#define AXLOGTAG(a,b)           ((b & ~0xFF) ? b : ((AXLOGDL)(((UINT)a)|((UINT)b))))

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct _tag_HAXLOG { UINT u_id; } * HAXLOG;
typedef struct _tag_HAXLOGWAY { UINT u_id; } * HAXLOGWAY;

typedef void (*PFNAXLOGCB)(PVOID,PCSTR,...);

typedef enum
{
    AXLOGWT_none            = 0,
    AXLOGWT_file,
    AXLOGWT_mp,
    AXLOGWT_tag
} AXLOGWT;

typedef enum
{
    AXLOGDL_emerg          = 0,
    AXLOGDL_crit,
    AXLOGDL_alert,
    AXLOGDL_error,
    AXLOGDL_warning,
    AXLOGDL_notice,
    AXLOGDL_info,
    AXLOGDL_debug,
    AXLOGDL_verbose,
    AXLOGDL_chatter,

    // This one must be the last
    AXLOGDL_bcast
} AXLOGDL;

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif

HAXLOG              axlog_close                 (HAXLOG             h_log);

HAXLOG              axlog_open                  (HAXLOGWAY          h_way,
                                                 UINT               u_msgmax);

void                axlog_msg                   (HAXLOG             h_log,
                                                 AXLOGDL            v_level,
                                                 PCSTR              psz_message);

void                axlog_msg_v                 (HAXLOG             h_log,
                                                 AXLOGDL            v_level,
                                                 PCSTR              psz_message,
                                                                    ...);

void                axlog_msg_vl                (HAXLOG             h_log,
                                                 AXLOGDL            v_level,
                                                 PCSTR              psz_message,
                                                 va_list            st_va_list);

HAXLOGWAY           axlog_way_close             (HAXLOGWAY          h_way);

HAXLOGWAY           axlog_file_way              (PCSTR              psz_path,
                                                 PCSTR              psz_name,
                                                 UINT               u_lifetime,
                                                 UINT               u_flags);

HAXLOGWAY           axlog_mobilepark_way        (UINT               u_flags);

BOOL                axlog_mobilepark_add_file   (HAXLOGWAY          h_way,
                                                 PCSTR              psz_fname,
                                                 U32                u_filter);

HAXLOGWAY           axlog_tagway                (PCSTR              psz_path,
                                                 PCSTR              psz_default,
                                                 UINT               u_lifetime,
                                                 UINT               u_flags);

AXLOGDL             axlog_tagway_add            (HAXLOG             h_log,
                                                 PCSTR              psz_fname);

void                axlog_tagway_del            (HAXLOG             h_log,
                                                 AXLOGDL            v_tag);

void                axlog                       (AXLOGDL            v_level,
                                                 PCSTR              psz_message,
                                                                    ...);

AXLOGWT             axlog_get_type              (HAXLOG             h_log);

#ifdef __cplusplus
}
#endif

#endif //  #ifndef __AXLOG_H__

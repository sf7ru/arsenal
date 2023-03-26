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

#ifndef __AXTHREADH__
#define __AXTHREADH__

#include <arsenal.h>
#include <axsystem.h>
#include <records.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// ---------------------------------------------------------------------------

#define MAC_THREADX_WAIT_QUIT(a,b,c) (axthreadX_sig_wait(h_ctrl,&a,nil,c) \
                                    && (a == AXSIG_QUIT))

#define AXTHRD_DEFAULTFLAGS    0

#if (TARGET_FAMILY == __AX_unix__)
#define AXMUTEX_SZ              24
#elif (TARGET_FAMILY == __AX_win__)
#define AXMUTEX_SZ              100
#else
#define AXMUTEX_SZ              24
#endif


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// ---------------------------------------------------------------------------

typedef struct _tag_HAXTHREAD  { UINT u_id; } * HAXTHREAD;
typedef struct _tag_HAXTHREADX { UINT u_id; } * HAXTHREADX;
typedef struct __tag_AXMUTEX   { UINT ab[AXMUTEX_SZ]; } AXMUTEX, * HAXMUTEX;
typedef struct _tag_HAXTHREADGROUP { UINT u_id; } * HAXTHREADGROUP;

typedef void (*PFNAXTHREAD)(PVOID);
typedef int (*PFNAXTHREADX)(HAXTHREADX,PVOID);



// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif                                      //  #ifdef __cplusplus


// ------------------------- process/threadsX.c ------------------------------

HAXTHREAD           axthread_destroy            (HAXTHREAD          h_thread);

BOOL                axthread_create             (HAXTHREAD *        ph_thread,
                                                 U32                u_flags,
                                                 UINT               stackSize,
                                                 UINT               priority,
                                                 PFNAXTHREAD        pf_entry,
                                                 PVOID              p_param);

void                axthread_exit               (UINT               uretcode);

BOOL                axthread_priority_set       (HAXTHREAD          h_thread,
                                                 INT                i_priority);

#define             axthreadX_exit(a)           return (a)

HAXTHREADX          axthreadX_destroy           (HAXTHREADX         h_ctrl,
                                                 UINT               u_mTO);

HAXTHREADX          axthreadX_create            (UINT               u_flags,
                                                 PFNAXTHREADX       pfn_entry,
                                                 PVOID              p_param,
                                                 UINT               u_mTO);

BOOL                axthreadX_sig_wait          (HAXTHREADX         h_ctrl,
                                                 AXSIG*             pv_sig,
                                                 ULONG*             pu_param,
                                                 UINT               u_mTO);

BOOL                axthreadX_sig_send          (HAXTHREADX         h_ctrl,
                                                 AXSIG              v_sig,
                                                 ULONG              u_param);

BOOL                axthreadX_priority_set      (HAXTHREADX         h_ctrl,
                                                 INT                i_priority);

HAXMUTEX            axmutex_destroy             (HAXMUTEX           h_lock);

HAXMUTEX            axmutex_create              (void);

BOOL                axmutex_lock                (HAXMUTEX           h_lock,
                                                 BOOL               b_wait);

BOOL                axmutex_lock_x              (HAXMUTEX           h_lock,
                                                 BOOL               b_wait,
                                                 BOOL               isr);

BOOL                axmutex_unlock              (HAXMUTEX           h_lock);

BOOL                axmutex_unlock_x            (HAXMUTEX           h_lock,
                                                 BOOL               isr);

BOOL                axmutex_init                (HAXMUTEX           h_lock);

void                axmutex_deinit              (HAXMUTEX           h_lock);


HAXTHREADGROUP      axthreadgroup_destroy       (HAXTHREADGROUP     h_group);

HAXTHREADGROUP      axthreadgroup_create        (UINT               groupSize,
                                                 PFNRECCLEAN        pfn_clean,
                                                 BOOL               reusable);

PVOID               axthreadgroup_get_uptr      (HAXTHREADGROUP     h_group,
                                                 U32                 u_id);

HAXTHREADX          axthreadgroup_get_ctl       (HAXTHREADGROUP     h_group,
                                                 U32                 u_id);

PVOID               axthreadgroup_wait_reuse    (HAXTHREADX         h_ctrl);

UINT                axthreadgroup_count         (HAXTHREADGROUP     h_group);

BOOL                axthreadgroup_reuse         (HAXTHREADGROUP     h_group,
                                                 PVOID              p_ptr);

U32                 axthreadgroup_add           (HAXTHREADGROUP     h_group,
                                                 PFNAXTHREADX       pfn_entry,
                                                 PVOID              p_ptr,
                                                 INT                i_priority,
                                                 BOOL               b_stopped,
                                                 UINT               u_TO);

BOOL                axthreadgroup_stop          (HAXTHREADGROUP     h_group,
                                                 UINT               u_TO);

BOOL                axthreadgroup_start         (HAXTHREADGROUP     h_group,
                                                 UINT               u_TO);

HAXTHREADGROUP      axthreadgroup_get_parent    (HAXTHREADX         h_ctrl,
                                                 PUINT              index,
                                                 UINT               TO);


UINT                axthread_get_id             ();

#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus

#endif                                      //  #ifndef __AXTHREADH__





// ***************************************************************************
// TITLE
//      Threads with eXtended control
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: threadX.c,v 1.2 2003/11/10 17:01:38 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************

#include <stdlib.h>
#include <string.h>

#include <arsenal.h>

#include <axthreads.h>
#include <axtime.h>
#include <pvt_threads.h>

// ---------------------------------------------------------------------------
// -------------------------- LOCAL DEFINITIONS ------------------------------
// ---------------------------------------------------------------------------

#define DEF_PARTNER         ((PAXTHREADX)pst_ctrl->p_partner)


// ---------------------------------------------------------------------------
// ----------------------------- LOCAL TYPES ---------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// STRUCTURE
//      AXTHREADXPARAMS
// PURPOSE
//      Thread Control structure
// ***************************************************************************
typedef struct __tag_AXTHREADXPARAMS
{
    PAXTHREADX              pst_ctrl;
    PFNAXTHREADX            pfn_entry;
    PVOID                   p_ptr;
} AXTHREADXPARAMS, * PAXTHREADXPARAMS;

// ***************************************************************************
// STRUCTURE
//      AXTHREADXSIG
// PURPOSE
//      ThreadX Control structure
// ***************************************************************************
typedef struct __tag_AXTHREADXSIG
{
    AXSIG                   v_sig;
    ULONG                   u_param;
} AXTHREADXSIG, * PAXTHREADXSIG;


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      _signal_exch
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PAXTHREADX   pst_ctrl   --
//      AXSIG        v_sig_send --
//      AXSIG        v_sig_wait --
//      UINT         u_mTO      --
// RESULT
//      BOOL --
// ***************************************************************************
static BOOL _signal_exch(PAXTHREADX     pst_ctrl,
                         AXSIG          v_sig_send,
                         AXSIG          v_sig_wait,
                         UINT           u_mTO)
{
    BOOL            b_result        = false;
    AXSIG           v_sig;
    ULONG           u_param;

    ENTER(pst_ctrl);

    if (  (v_sig_send == AXSIG_NONE)                                ||
          axthreadX_sig_send((HAXTHREADX)pst_ctrl, v_sig_send, 0)   )
    {
        if (  (v_sig_wait == AXSIG_NONE)                        ||
              (axthreadX_sig_wait((HAXTHREADX)pst_ctrl,
                                  &v_sig, &u_param, u_mTO)  &&
              (v_sig == v_sig_wait)                         )   )
        {
            b_result = true;
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      _axthreadX_full_control_release
// PURPOSE
//
// PARAMETERS
//      PAXTHREADX pst_ctrl --
// RESULT
//      none
// ***************************************************************************
static void _axthreadX_full_control_release(PAXTHREADX pst_ctrl)
{
    ENTER(pst_ctrl);

    if (DEF_PARTNER)
    {
        axmutex_unlock(DEF_PARTNER->pst_LCK);
    }

    axmutex_unlock(pst_ctrl->pst_LCK);

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      _axthreadX_full_control_capture
// PURPOSE
//
// PARAMETERS
//      PAXTHREADX pst_ctrl --
// RESULT
//      none
// ***************************************************************************
static void _axthreadX_full_control_capture(PAXTHREADX pst_ctrl)
{
    ENTER(pst_ctrl);

    while (true)
    {
        if (axmutex_lock(pst_ctrl->pst_LCK, false))
        {
            if (DEF_PARTNER)
            {
                if (!axmutex_lock(DEF_PARTNER->pst_LCK, false))
                {
                    axmutex_unlock(pst_ctrl->pst_LCK);
                }
                else
                    break;
            }
            else
                break;
        }

        axsleep(1);
    }

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      axthreadX_destroy
// PURPOSE
//
// PARAMETERS
//      HAXTHREADX   h_ctrl --
//      UINT         u_mTO  --
// RESULT
//      HAXTHREADX  --
// ***************************************************************************
HAXTHREADX axthreadX_destroy(HAXTHREADX h_ctrl, UINT u_mTO)
{
    PAXTHREADX         pst_ctrl        = (PAXTHREADX)h_ctrl;

    ENTER(pst_ctrl);

    if (  !u_mTO                                                ||
          !DEF_PARTNER                                          ||
          _signal_exch(pst_ctrl, AXSIG_NONE, AXSIG_QUIT, 1)     ||
          _signal_exch(pst_ctrl, AXSIG_QUIT, AXSIG_QUIT, u_mTO) )
    {
// ---------- Releasing own structure from partner's structure ---------------

        if (pst_ctrl->pst_LCK)
        {
            _axthreadX_full_control_capture(pst_ctrl);

            if (DEF_PARTNER)
                DEF_PARTNER->p_partner = NULL;

            _axthreadX_full_control_release(pst_ctrl);
        }

// ---------------------------- Freeing stuff --------------------------------

        if (pst_ctrl->h_signal)
            axevent_destroy(pst_ctrl->h_signal);

        if (pst_ctrl->h_queue)
            recqueue_destroy(pst_ctrl->h_queue);

        if (pst_ctrl->pst_LCK)
            axmutex_destroy(pst_ctrl->pst_LCK);

        if (pst_ctrl->h_thread)
            axthread_destroy(pst_ctrl->h_thread);

        FREE(pst_ctrl);
        pst_ctrl = NULL;
    }

    RETURN((HAXTHREADX)pst_ctrl);
}
// ***************************************************************************
// STATIC FUNCTION
//      _axthreadX_THREAD
// PURPOSE
//
// PARAMETERS
//      PAXTHREADXPARAMS pst_params --
// RESULT
//      none
// ***************************************************************************
static void _axthreadX_THREAD(PAXTHREADXPARAMS pst_params)
{
//    int     i_retcode;

//    i_retcode =
            (*pst_params->pfn_entry)(
                    (HAXTHREADX)pst_params->pst_ctrl, pst_params->p_ptr);

    axthreadX_destroy((HAXTHREADX)pst_params->pst_ctrl, 0);

    FREE(pst_params);

    axthread_exit(0);
}
// ***************************************************************************
// FUNCTION
//      _threadX_create
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//      PAXTHREADX --
// ***************************************************************************
static PAXTHREADX _threadX_create(void)
{
    PAXTHREADX      pst_T           = NULL;

    ENTER(true);

    if ((pst_T = CREATE(AXTHREADX)) != NULL)
    {
        if ( !( ((pst_T->pst_LCK    = axmutex_create())     != NULL)    &&
                ((pst_T->h_signal   =
                        axevent_create(false, true))      != NULL)      &&
                ((pst_T->h_queue  = recqueue_create(
                        sizeof(AXTHREADXSIG), 3, 100, NULL))!= NULL)    ))
        {
            pst_T = (PAXTHREADX)axthreadX_destroy((HAXTHREADX)pst_T, 0);
        }
    }

    RETURN(pst_T);
}
// ***************************************************************************
// FUNCTION
//      axthreadX_create
// PURPOSE
//
// PARAMETERS
//      DWORD                u_flags  --
//      PFNAXTHREADX pfn_entry --
//      PVOID                p_param  --
//      DWORD                u_mTO --
// RESULT
//      HAXTHREADX --
// ***************************************************************************
HAXTHREADX axthreadX_create(UINT                u_flags,
                            PFNAXTHREADX        pfn_entry,
                            PVOID               p_param,
                            UINT                u_mTO)
{
    PAXTHREADX         pst_parent_ctrl     = NULL;
    PAXTHREADX         pst_child_ctrl      = NULL;
    PAXTHREADXPARAMS   pst_child_params    = NULL;

    ENTER(pfn_entry);

    if (  ((pst_parent_ctrl  = _threadX_create())       != NULL)    &&
          ((pst_child_ctrl   = _threadX_create())       != NULL)    &&
          ((pst_child_params = CREATE(AXTHREADXPARAMS)) != NULL)    )
    {
        pst_parent_ctrl->p_partner  = pst_child_ctrl;
        pst_child_ctrl->p_partner   = pst_parent_ctrl;

        pst_child_params->pst_ctrl  = pst_child_ctrl;
        pst_child_params->pfn_entry = pfn_entry;
        pst_child_params->p_ptr     = p_param;

        if (axthread_create(&pst_parent_ctrl->h_thread, u_flags, 0, 0,
                (PFNAXTHREAD)_axthreadX_THREAD, pst_child_params))
        {
            if (u_mTO)
            {
                if (!_signal_exch(pst_parent_ctrl, AXSIG_NONE, AXSIG_GO, u_mTO))
                {
                    pst_parent_ctrl = (PAXTHREADX)
                        axthreadX_destroy((HAXTHREADX)pst_parent_ctrl, 0);
                }
            }
        }
        else
        {
            pst_parent_ctrl = (PAXTHREADX)
                axthreadX_destroy((HAXTHREADX)pst_parent_ctrl, 0);
            pst_child_ctrl  = (PAXTHREADX)
                axthreadX_destroy((HAXTHREADX)pst_child_ctrl, 0);

            FREE(pst_child_params);
        }
    }
    else
    {
        if (pst_parent_ctrl)
        {
            pst_parent_ctrl = (PAXTHREADX)
                axthreadX_destroy((HAXTHREADX)pst_parent_ctrl, 0);
        }

        if (pst_child_ctrl)
        {
            pst_child_ctrl  = (PAXTHREADX)
                axthreadX_destroy((HAXTHREADX)pst_child_ctrl, 0);
        }
    }

    RETURN((HAXTHREADX)pst_parent_ctrl);
}
// ***************************************************************************
// FUNCTION
//      axthreadX_sig_wait
// PURPOSE
//
// PARAMETERS
//      HAXTHREADX   h_ctrl   --
//      AXSIG*       pv_sig   --
//      PUINT        pu_param --
//      UINT         u_mTO    --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL axthreadX_sig_wait(HAXTHREADX      h_ctrl,
                        AXSIG*          pv_sig,
                        ULONG*          pu_param,
                        UINT            u_mTO)
{
    BOOL            b_received  = false;
    PAXTHREADX      pst_ctrl    = (PAXTHREADX)h_ctrl;
    AXTHREADXSIG    st_sig;

    if (pv_sig && pst_ctrl)
    {
        if (axevent_wait(pst_ctrl->h_signal, u_mTO))
        {
            axmutex_lock(pst_ctrl->pst_LCK, true);

            if (recqueue_get(pst_ctrl->h_queue, &st_sig))
            {
                if (pu_param)
                    *pu_param = st_sig.u_param;

                *pv_sig    = st_sig.v_sig;
                b_received = true;
            }

            if (!recqueue_count(pst_ctrl->h_queue))
                axevent_set(pst_ctrl->h_signal, false);

            axmutex_unlock(pst_ctrl->pst_LCK);
        }
    }

    return b_received;
}
// ***************************************************************************
// FUNCTION
//      axthreadX_sig_send
// PURPOSE
//
// PARAMETERS
//      HAXTHREADX h_ctrl  --
//      AXSIG      v_sig   --
//      DWORD       d_param --
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
BOOL axthreadX_sig_send(HAXTHREADX      h_ctrl,
                        AXSIG           v_sig,
                        ULONG           u_param)
{
    BOOL            b_sent      = false;
    PAXTHREADX      pst_ctrl    = (PAXTHREADX)h_ctrl;
    AXTHREADXSIG    st_sig;

    if (pst_ctrl)
    {
        memset(&st_sig, 0, sizeof(AXTHREADXSIG));
        st_sig.v_sig   = v_sig;
        st_sig.u_param = u_param;

        _axthreadX_full_control_capture(pst_ctrl);

        if (DEF_PARTNER)
        {
            if ((b_sent = recqueue_add(
                                DEF_PARTNER->h_queue, &st_sig)) != false)
            {
                axevent_set(DEF_PARTNER->h_signal, true);
            }
        }

        _axthreadX_full_control_release(pst_ctrl);
    }

    return b_sent;
}
// ***************************************************************************
// FUNCTION
//      axthreadX_priority_set
// PURPOSE
//
// PARAMETERS
//      HAXTHREADX h_ctrl     --
//      INT         i_priority --
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
BOOL axthreadX_priority_set(HAXTHREADX      h_ctrl,
                            INT             i_priority)
{
    BOOL            b_result    = false;
    PAXTHREADX     pst_ctrl    = (PAXTHREADX)h_ctrl;

    ENTER(pst_ctrl);

    b_result = axthread_priority_set(pst_ctrl->h_thread, i_priority);

    RETURN(b_result);
}

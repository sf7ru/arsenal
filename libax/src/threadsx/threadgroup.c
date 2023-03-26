// ***************************************************************************
// TITLE
//
// PROJECT
//
// ***************************************************************************
// FILE
//      $Id$
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arsenal.h>

#include <axtime.h>

#include <axthreads.h>
#include <pvt_threads.h>

// ---------------------------------------------------------------------------
// ----------------------------- DEFINITIONS ---------------------------------
// ---------------------------------------------------------------------------

#define DEF_TO_STOP         -1


// ---------------------------------------------------------------------------
// -------------------------------- TYPES ------------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// STRUCTURE
//      ENTRY
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_ENTRY
{
    HAXTHREADX          h_thread;

    INT                 i_priority;

    PFNRECCLEAN         pfn_clean;
    PFNAXTHREADX        pfn_entry;
    PVOID               p_uptr;
} ENTRY, * PENTRY;

// ***************************************************************************
// STRUCTURE
//      THREADGROUP
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_THREADGROUP
{
    HRECARRAY       h_threads;
    PFNRECCLEAN     pfn_clean;
    BOOL            reusable;
} THREADGROUP, * PTHREADGROUP;


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      _group_entry_clean
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PENTRY   pst_E --
// RESULT
//      BOOL --
// ***************************************************************************
static BOOL _group_entry_clean(PENTRY pst_E)
{
    BOOL            b_result        = true;

    ENTER(true);

    if (pst_E->h_thread)
    {
        if ((pst_E->h_thread = axthreadX_destroy(
                        pst_E->h_thread, DEF_TO_STOP)) != NULL)
        {
            b_result = false;
        }
    }

    if (b_result && pst_E->p_uptr && pst_E->pfn_clean)
    {
        (*pst_E->pfn_clean)(pst_E->p_uptr);

        pst_E->p_uptr = NULL;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axthreadgroup_destroy
// PURPOSE
//
// PARAMETERS
//      HAXTHREADGROUP h_group --
// RESULT
//      HAXTHREADGROUP  --
// ***************************************************************************
HAXTHREADGROUP axthreadgroup_destroy(HAXTHREADGROUP h_group)
{
    PTHREADGROUP     pst_G        = (PTHREADGROUP)h_group;

    ENTER(pst_G);

    // Threads will be stopped at recarray cleaning procedure
    if (  pst_G->h_threads                                                  &&
          ((pst_G->h_threads = recarray_destroy(pst_G->h_threads)) == NULL) )
    {
        FREE(pst_G);

        pst_G = NULL;
    }

    RETURN((HAXTHREADGROUP)pst_G);
}
// ***************************************************************************
// FUNCTION
//      axthreadgroup_create
// PURPOSE
//
// PARAMETERS
//      UINT        groupSize --
//      PFNRECCLEAN pfn_clean --
// RESULT
//      HAXTHREADGROUP --
// ***************************************************************************
HAXTHREADGROUP axthreadgroup_create(UINT          groupSize,
                                    PFNRECCLEAN   pfn_clean,
                                    BOOL          reusable)
{
    PTHREADGROUP       pst_G        = NULL;

    ENTER(true);

    if ((pst_G = CREATE(THREADGROUP)) != NULL)
    {
        pst_G->pfn_clean    = pfn_clean;

        //printf("axthreadgroup_create stage groupSize = %d\n", groupSize);

        pst_G->reusable   = reusable;

        if ((pst_G->h_threads =
                recarray_create(sizeof(ENTRY),
                                groupSize ? groupSize : 3,
                                groupSize ? 0 : 1,
                                (PFNRECCLEAN)_group_entry_clean)) == NULL)
        {
            pst_G = (PTHREADGROUP)axthreadgroup_destroy((HAXTHREADGROUP)pst_G);
        }
    }

    RETURN((HAXTHREADGROUP)pst_G);
}
// ***************************************************************************
// FUNCTION
//      axthreadgroup_add
// PURPOSE
//
// PARAMETERS
//      HAXTHREADGROUP   h_group    --
//      PFNAXTHREADX    pfn_entry --
//      PVOID           p_uptr     --
// RESULT
//      U32  --
// ***************************************************************************
U32 axthreadgroup_add(HAXTHREADGROUP        h_group,
                         PFNAXTHREADX       pfn_entry,
                         PVOID              p_uptr,
                         INT                i_priority,
                         BOOL               b_stopped,
                         UINT               u_TO)
{
    U32             u_index         = AXII;
    PTHREADGROUP    pst_G           = (PTHREADGROUP)h_group;
    ENTRY           st_E;
    PAXTHREADX      th;

    ENTER(pst_G);

    memset(&st_E, 0, sizeof(st_E));

    st_E.p_uptr     = p_uptr;
    st_E.pfn_entry  = pfn_entry;
    st_E.pfn_clean  = pst_G->pfn_clean;
    st_E.i_priority = i_priority;

//    printf("threadgroup.c:207 stage ADD !!!!\n");

    if ((u_index = recarray_add(pst_G->h_threads, &st_E)) != AXII)
    {
        if (!b_stopped)
        {
            if ((st_E.h_thread = axthreadX_create(0,
                                   pfn_entry, p_uptr, u_TO)) != NULL)
            {
                th = (PAXTHREADX)st_E.h_thread;
                th->u_index = u_index;
                th->p_group = h_group;

                if (st_E.i_priority)
                {
                    axthreadX_priority_set(st_E.h_thread, st_E.i_priority);
                }

                recarray_ovr(pst_G->h_threads, u_index, &st_E);
            }
            else
            {
                recarray_del(pst_G->h_threads, u_index);

                u_index = AXII;
            }
        }
    }

    //printf("axthreadgroup_add stage 10 = %d\n", u_index);

    RETURN(u_index);
}
// ***************************************************************************
// FUNCTION
//      axthreadgroup_get_ptr
// PURPOSE
//
// PARAMETERS
//      HAXTHREADGROUP   h_group    --
//                      UINT3u_id --
// RESULT
//      PVOID  --
// ***************************************************************************
PVOID axthreadgroup_get_uptr(HAXTHREADGROUP     h_group,
                             U32             u_id)
{
    PVOID           p_uptr          = NULL;
    PTHREADGROUP    pst_G           = (PTHREADGROUP)h_group;
    PENTRY          pst_E;

    ENTER(pst_G);

    if ((pst_E = recarray_get(pst_G->h_threads, u_id)) != NULL)
    {
        p_uptr = pst_E->p_uptr;
    }

    RETURN(p_uptr);
}
// ***************************************************************************
// FUNCTION
//      axthreadgroup_get_ctl
// PURPOSE
//
// PARAMETERS
//      HAXTHREADGROUP   h_group   --
//      U32           u_id      --
// RESULT
//      HAXTHREADX  --
// ***************************************************************************
HAXTHREADX axthreadgroup_get_ctl(HAXTHREADGROUP     h_group,
                                 U32             u_id)
{
    HAXTHREADX      h_thread        = NULL;
    PTHREADGROUP    pst_G           = (PTHREADGROUP)h_group;
    PENTRY          pst_E;

    ENTER(pst_G);

    if ((pst_E = recarray_get(pst_G->h_threads, u_id)) != NULL)
    {
        h_thread = pst_E->h_thread;
    }

    RETURN(h_thread);
}
// ***************************************************************************
// FUNCTION
//      axthreadgroup_stop
// PURPOSE
//
// PARAMETERS
//      HAXTHREADGROUP   h_group --
//      UINT             u_TO    --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL axthreadgroup_stop(HAXTHREADGROUP  h_group,
                        UINT            u_TO)
{
    BOOL            b_result        = false;
    PTHREADGROUP    pst_G           = (PTHREADGROUP)h_group;
    PENTRY          pst_E;
    UINT            u_idx;

    ENTER(pst_G);

    for (  u_idx = 0, b_result = true;
           (pst_E = recarray_get(pst_G->h_threads, u_idx)) != NULL;
           u_idx++)
    {
        if (pst_E->h_thread)
        {
            if ((pst_E->h_thread =
                    axthreadX_destroy(pst_E->h_thread, u_TO)) != NULL)
            {
                b_result = false;
            }
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axthreadgroup_start
// PURPOSE
//
// PARAMETERS
//      HAXTHREADGROUP   h_group --
//      UINT             u_TO    --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL axthreadgroup_start(HAXTHREADGROUP         h_group,
                         UINT                   u_TO)
{
    BOOL            b_result        = false;
    PTHREADGROUP    pst_G           = (PTHREADGROUP)h_group;
    PENTRY          pst_E;
    UINT            u_idx;
    PAXTHREADX      th;

    ENTER(pst_G);

    for (  u_idx = 0, b_result = true;
           (pst_E = recarray_get(pst_G->h_threads, u_idx)) != NULL;
           u_idx++)
    {
        if (!pst_E->h_thread)
        {
            if ((pst_E->h_thread = axthreadX_create(0,
                                        pst_E->pfn_entry,
                                        pst_E->p_uptr, u_TO)) != NULL)
            {
                th = (PAXTHREADX)pst_E->h_thread;
                th->u_index = u_idx;
                th->p_group = h_group;

                if (pst_E->i_priority)
                {
                    axthreadX_priority_set(pst_E->h_thread, pst_E->i_priority);
                }
            }
            else
                b_result = false;
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axthreadgroup_reuse
// PURPOSE
//
// PARAMETERS
//      HAXTHREADGROUP h_group --
//      PVOID          p_ptr   --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL axthreadgroup_reuse(HAXTHREADGROUP     h_group,
                         PVOID              p_ptr)
{
    BOOL            result          = false;
    UINT            idx             = 0;
    PENTRY          pst_E;

    PTHREADGROUP    group           = (PTHREADGROUP)h_group;

    ENTER(true);

    if (group->reusable)
    {
        while (!result && ((pst_E = recarray_get(group->h_threads, idx++)) != nil))
        {
            //printf("axthreadgroup_reuse stage 1: idx %d = %p\n", idx - 1, pst_E->p_uptr);

            if (!pst_E->p_uptr)
            {
                result  = axthreadX_sig_send(pst_E->h_thread, AXSIG_GO, (ULONG)p_ptr);
                //printf("axthreadgroup_reuse stage 2: idx %d result = %s\n", idx - 1, MAC_tf(result));
            }
        }

    }

    //printf("axthreadgroup_reuse stage result = %s (%d)\n", MAC_tf(result), idx - 1);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      axthreadgroup_wait_reuse
// PURPOSE
//
// PARAMETERS
//      HAXTHREADX h_ctrl --
// RESULT
//      PVOID --
// ***************************************************************************
PVOID axthreadgroup_wait_reuse(HAXTHREADX         h_ctrl)
{
    PVOID           result          = nil;
    AXSIG           v_sig;
    ULONG           u_param;
    PTHREADGROUP    group;
    PENTRY          e;
    BOOL            run             = true;
    UINT            index;

    ENTER(true);

    group = (PTHREADGROUP)axthreadgroup_get_parent(h_ctrl, &index, -1);

    if (group->reusable)
    {
        if ((e = (PENTRY)recarray_get(group->h_threads, index)) != nil)
        {
            e->p_uptr = nil;

            while (run && !result)
            {
                if (axthreadX_sig_wait(h_ctrl, &v_sig, &u_param, -1))
                {
                    switch (v_sig)
                    {
                        case  AXSIG_QUIT:
                            run = false;
                            break;

                        case  AXSIG_GO:
                            e->p_uptr   = (PVOID)u_param;
                            result      = (PVOID)u_param;
                            break;

                        default:
                            printf("axthreadgroup_wait_reuse %d!\n", v_sig);
                            break;
                    }
                }
            }
        }
//        else
//            printf("NO WAIT CAUSE NO ENTRY!\n");
    }
//    else
//        printf("NO WAIT CAUSE EXPANDABLE!\n");

//    printf("<< axthreadgroup_wait_reuse stage \n");

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      axthreadgroup_get_parent
// PURPOSE
//
// PARAMETERS
//      HAXTHREADX h_ctrl --
//      PUINT      index  --
// RESULT
//      HAXTHREADGROUP --
// ***************************************************************************
HAXTHREADGROUP axthreadgroup_get_parent(HAXTHREADX         h_ctrl,
                                        PUINT              index,
                                        UINT               TO)
{
    HAXTHREADGROUP  result          = false;
    UINT            cdn             = TO / 10;
    PAXTHREADX      parent;

    ENTER(true);

    do
    {
        parent = (PAXTHREADX)((PAXTHREADX)h_ctrl)->p_partner;

        if (parent && parent->p_group)
        {
            result = (HAXTHREADGROUP)parent->p_group;

            if (index != nil)
            {
                *index  = parent->u_index;
            }
        }
        else
            axsleep(10);

    } while (!result && cdn--);

    RETURN(result);
}
UINT axthreadgroup_count(HAXTHREADGROUP     h_group)
{
    UINT            result          = 0;
    PTHREADGROUP    group           = (PTHREADGROUP)h_group;

    ENTER(true);

    if (group)
    {
        result = recarray_count(group->h_threads);
    }

    RETURN(result);
}
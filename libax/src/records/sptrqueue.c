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
#include <string.h>

#include <arsenal.h>

#include <records.h>

#include <pvt_records.h>


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct __tag_SPTRQUEUE
{
    HPTRQUEUE               queue;
    PVOID                   buff;
    UINT                    buffSize;
    UINT                    buffCount;
} SPTRQUEUE, * PSPTRQUEUE;

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      sptrqueue_destroy
// PURPOSE
//
// PARAMETERS
//      HSPTRQUEUE h_queue --
// RESULT
//      HSPTRQUEUE --
// ***************************************************************************
HSPTRQUEUE sptrqueue_destroy(HSPTRQUEUE h_queue)
{
    PSPTRQUEUE       Q   = (PSPTRQUEUE)h_queue;

    ENTER(Q);
    
    SAFEDESTROY(ptrqueue, Q->queue);

    RETURN(NULL);
}
// ***************************************************************************
// FUNCTION
//      sptrqueue_create
// PURPOSE
//
// PARAMETERS
//      UINT u_size --
// RESULT
//      HSPTRQUEUE --
// ***************************************************************************
HSPTRQUEUE sptrqueue_create(PVOID       buff,
                            PVOID       buffSize,
                            UINT        recSize)
{
    PSPTRQUEUE       Q       = NULL;

    ENTER(u_size);

    if ((Q = CREATE(SPTRQUEUE)) != NULL)
    {
        Q->queue
     }

    RETURN((HSPTRQUEUE)Q);
}
// ***************************************************************************
// FUNCTION
//      sptrqueue_peek
// PURPOSE
//
// PARAMETERS
//      HSPTRQUEUE h_queue --
// RESULT
//      PVOID --
// ***************************************************************************
PVOID sptrqueue_peek(HSPTRQUEUE h_queue)
{
    PVOID           result      = NULL;
    volatile PVOID* top;
    PSPTRQUEUE       Q       = (PSPTRQUEUE)h_queue;

    if (Q->top != Q->bot)
    {
        top =   (Q->top == Q->end)  ?
                 Q->begin           :
                 Q->top             ;

        result = *top;
    }

//    if (result != NULL)
//        printf("%p peek %d (%d,%d/%d,%d)\n", Q, *(PUINT)result,
//               Q->top - Q->begin,
//               Q->end - Q->top,
//               Q->bot - Q->begin,
//               Q->end - Q->bot);

    return result;
}
// ***************************************************************************
// FUNCTION
//      sptrqueue_get
// PURPOSE
//
// PARAMETERS
//      HSPTRQUEUE h_queue --
// RESULT
//      PVOID --
// ***************************************************************************
PVOID sptrqueue_get(HSPTRQUEUE     h_queue)
{
    PVOID           result      = NULL;
    PSPTRQUEUE       Q       = (PSPTRQUEUE)h_queue;

    LOCK;

    if (Q->top != Q->bot)
    {
        if (Q->top == Q->end)
        {
            Q->top = Q->begin;
        }

        result = *Q->top;
        Q->top++;

//        printf("%p get %d (%d,%d/%d,%d)\n", Q, *(PUINT)result,
//               Q->top - Q->begin,
//               Q->end - Q->top,
//               Q->bot - Q->begin,
//               Q->end - Q->bot);
    }

    UNLOCK;

    return result;
}
// ***************************************************************************
// FUNCTION
//      sptrqueue_put
// PURPOSE
//
// PARAMETERS
//      HSPTRQUEUE h_queue --
//      PVOID     ptr     --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL sptrqueue_put(HSPTRQUEUE     h_queue,
                  PVOID         ptr)
{
    BOOL            result    = false;
    PSPTRQUEUE       Q       = (PSPTRQUEUE)h_queue;

    ENTER(Q);

    LOCK;

    if (Q->bot == Q->end)
    {
        if ((Q->top - Q->begin) > 1)
        {
            Q->bot  = Q->begin;
            result  = true;
        }
        else
        {
            // head must not catch tail
        }
    }
    else
    {
        if (Q->bot >= Q->top)
        {
            result = true;
        }
        else
        {
            if ((Q->top - Q->bot) > 1)
            {
                result = true;
            }
            else
            {
                // head must not catch tail
            }
        }
    }

    if (result)
    {
        *(Q->bot++) = ptr;
    }

//    printf("%p put %d (%d,%d/%d,%d)\n", Q, *(PUINT)ptr,
//           Q->top - Q->begin,
//           Q->end - Q->top,
//           Q->bot - Q->begin,
//           Q->end - Q->bot);

    UNLOCK;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      sptrqueue_size
// PURPOSE
//
// PARAMETERS
//      HSPTRQUEUE h_queue --
// RESULT
//      UINT --
// ***************************************************************************
UINT sptrqueue_size(HSPTRQUEUE          h_queue)
{
    UINT            result      = 0;
    PSPTRQUEUE       Q           = (PSPTRQUEUE)h_queue;

    ENTER(Q);

    result = Q->end - Q->begin;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      sptrqueue_count
// PURPOSE
//
// PARAMETERS
//      HSPTRQUEUE h_queue --
// RESULT
//      UINT --
// ***************************************************************************
UINT sptrqueue_count(HSPTRQUEUE          h_queue)
{
    UINT            result          = 0;
    PSPTRQUEUE       Q           = (PSPTRQUEUE)h_queue;

    ENTER(true);

    if (Q->top != Q->bot)
    {
        if (Q->top < Q->bot)
        {
            result = Q->bot - Q->top;
        }
        else
        {
            result = (Q->end - Q->top) + (Q->bot - Q->begin);
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      sptrqueue_clean
// PURPOSE
//
// PARAMETERS
//      PFNRECFREE pfn_free --
// RESULT
//      void --
// ***************************************************************************
void sptrqueue_clean(HSPTRQUEUE          h_queue,
                    PFNRECFREE         pfn_free)
{
    PVOID       ptr;

    ENTER(true);

    if (pfn_free)
    {
        while ((ptr = sptrqueue_get(h_queue)) != nil)
        {
            (*pfn_free)(ptr);
        }
    }

    QUIT;
}

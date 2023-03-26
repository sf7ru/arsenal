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

//#define LOCK    a7mutex_lock(Q->LCK, true)
//#define UNLOCK  a7mutex_unlock(Q->LCK)
#define LOCK
#define UNLOCK


// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      ptrqueue_destroy
// PURPOSE
//
// PARAMETERS
//      HPTRQUEUE h_queue --
// RESULT
//      HPTRQUEUE --
// ***************************************************************************
HPTRQUEUE ptrqueue_destroy(HPTRQUEUE h_queue)
{
    PPTRQUEUE       Q   = (PPTRQUEUE)h_queue;

    ENTER(Q);

    if (Q->begin)
        FREE(Q->begin);

//    if (Q->LCK)
//        a7mutex_destroy(Q->LCK);

    FREE(Q);

    RETURN(NULL);
}
// ***************************************************************************
// FUNCTION
//      ptrqueue_create
// PURPOSE
//
// PARAMETERS
//      UINT u_size --
// RESULT
//      HPTRQUEUE --
// ***************************************************************************
HPTRQUEUE ptrqueue_create(UINT          u_size)
{
    PPTRQUEUE       Q       = NULL;

    ENTER(u_size);

    if ((Q = CREATE(PTRQUEUE)) != NULL)
    {
        if ((Q->begin = MALLOC(u_size * sizeof(PVOID))) != NULL)
        {
            Q->top  = Q->begin;
            Q->bot  = Q->begin;
            Q->end  = Q->begin + u_size;

//            Q->LCK  = a7mutex_create();
        }
        else
            Q = (PPTRQUEUE)ptrqueue_destroy((HPTRQUEUE)Q);
     }

    RETURN((HPTRQUEUE)Q);
}
// ***************************************************************************
// FUNCTION
//      ptrqueue_peek
// PURPOSE
//
// PARAMETERS
//      HPTRQUEUE h_queue --
// RESULT
//      PVOID --
// ***************************************************************************
PVOID ptrqueue_peek(HPTRQUEUE h_queue)
{
    PVOID           result      = NULL;
    volatile PVOID* top;
    PPTRQUEUE       Q       = (PPTRQUEUE)h_queue;

    LOCK;

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

    UNLOCK;

    return result;
}
// ***************************************************************************
// FUNCTION
//      ptrqueue_get
// PURPOSE
//
// PARAMETERS
//      HPTRQUEUE h_queue --
// RESULT
//      PVOID --
// ***************************************************************************
PVOID ptrqueue_get(HPTRQUEUE     h_queue)
{
    PVOID           result      = NULL;
    PPTRQUEUE       Q       = (PPTRQUEUE)h_queue;

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
//      ptrqueue_put
// PURPOSE
//
// PARAMETERS
//      HPTRQUEUE h_queue --
//      PVOID     ptr     --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL ptrqueue_put(HPTRQUEUE     h_queue,
                  PVOID         ptr)
{
    BOOL            result    = false;
    PPTRQUEUE       Q       = (PPTRQUEUE)h_queue;

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
        *Q->bot     = ptr;

        Q->bot++;
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
//      ptrqueue_size
// PURPOSE
//
// PARAMETERS
//      HPTRQUEUE h_queue --
// RESULT
//      UINT --
// ***************************************************************************
UINT ptrqueue_size(HPTRQUEUE          h_queue)
{
    UINT            result      = 0;
    PPTRQUEUE       Q           = (PPTRQUEUE)h_queue;

    ENTER(Q);

    result = Q->end - Q->begin;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      ptrqueue_count
// PURPOSE
//
// PARAMETERS
//      HPTRQUEUE h_queue --
// RESULT
//      UINT --
// ***************************************************************************
UINT ptrqueue_count(HPTRQUEUE          h_queue)
{
    UINT            result          = 0;
    PPTRQUEUE       Q           = (PPTRQUEUE)h_queue;

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
//      ptrqueue_clean
// PURPOSE
//
// PARAMETERS
//      PFNRECFREE pfn_free --
// RESULT
//      void --
// ***************************************************************************
void ptrqueue_clean(HPTRQUEUE          h_queue,
                    PFNRECFREE         pfn_free)
{
    PVOID       ptr;

    ENTER(true);

    if (pfn_free)
    {
        while ((ptr = ptrqueue_get(h_queue)) != nil)
        {
            (*pfn_free)(ptr);
        }
    }

    QUIT;
}

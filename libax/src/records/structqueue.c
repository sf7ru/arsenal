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
// ------------------------------- DEFINITIONS -------------------------------
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      structqueue_destroy
// PURPOSE
//
// PARAMETERS
//      HSTRUCTQUEUE h_queue --
// RESULT
//      HSTRUCTQUEUE --
// ***************************************************************************
HSTRUCTQUEUE structqueue_destroy(HSTRUCTQUEUE h_queue)
{
    PSTRUCTQUEUE       Q   = (PSTRUCTQUEUE)h_queue;

    ENTER(Q);

    if (Q->begin)
        FREE(Q->begin);

    FREE(Q);

    RETURN(NULL);
}
// ***************************************************************************
// FUNCTION
//      structqueue_create
// PURPOSE
//
// PARAMETERS
//      UINT          structSize --
//      UINT          queueSize --
// RESULT
//      HSTRUCTQUEUE --
// ***************************************************************************
HSTRUCTQUEUE structqueue_create(UINT          structSize,
                                UINT          queueSize)
{
    PSTRUCTQUEUE       Q       = NULL;
    UINT               size;

    ENTER(u_size);

    if ((Q = CREATE(STRUCTQUEUE)) != NULL)
    {
        Q->recSize  = MAC_ALIGN_U32(structSize);
        size            = Q->recSize * queueSize;

        if ((Q->begin = MALLOC(size)) != NULL)
        {
            Q->top      = Q->begin;
            Q->bot      = Q->begin;
            Q->end      = Q->begin + size;
        }
        else
            Q = (PSTRUCTQUEUE)structqueue_destroy((HSTRUCTQUEUE)Q);
     }

    RETURN((HSTRUCTQUEUE)Q);
}
// ***************************************************************************
// FUNCTION
//      structqueue_peek
// PURPOSE
//
// PARAMETERS
//      HSTRUCTQUEUE h_queue --
// RESULT
//      UINT --
// ***************************************************************************
PVOID structqueue_peek(HSTRUCTQUEUE h_queue)
{
    PVOID               result      = nil;
    PSTRUCTQUEUE        Q       = (PSTRUCTQUEUE)h_queue;

    if (Q->top != Q->bot)
    {
        result = (Q->top == Q->end)  ?
                  Q->begin               :
                  Q->top                 ;
    }

    return result;
}
// ***************************************************************************
// FUNCTION
//      structqueue_get
// PURPOSE
//
// PARAMETERS
//      HSTRUCTQUEUE   h_queue --
//      PVOID       p_data  -- Pointer to target data buffer. In case it equals
//                             zero, no data will be copied, but record
//                             will be pushed out
// RESULT
//      BOOL  --
// ***************************************************************************
PVOID structqueue_get(HSTRUCTQUEUE     h_queue)
{
    PVOID               result      = nil;
    PSTRUCTQUEUE        Q           = (PSTRUCTQUEUE)h_queue;

    if (Q->top != Q->bot)
    {
        if (Q->top == Q->end)
        {
            Q->top = Q->begin;
        }

        result      = Q->top;
        Q->top     += Q->recSize;
    }

    return result;
}
// ***************************************************************************
// FUNCTION
//      la6_structqueue_rec_add
// PURPOSE
//      Add Record to Queue
// PARAMETERS
//      HSTRUCTQUEUE Q -- Pointer to Record Queue control structure
//      PVOID        p_data    -- Pointer to Recodr data to add
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
PVOID structqueue_put_begin(HSTRUCTQUEUE   h_queue)
{
    PVOID              result    = nil;
    PSTRUCTQUEUE       Q       = (PSTRUCTQUEUE)h_queue;

    ENTER(Q);

    if (Q->bot == Q->end)
    {
        if ((Q->top - Q->begin) > 1)
        {
            Q->bot  = Q->begin;
            result  = Q->bot;
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
            result  = Q->bot;
        }
        else
        {
            if ((Q->top - Q->bot) > 1)
            {
                result = Q->bot;
            }
            else
            {
                // head must not catch tail
            }
        }
    }

    RETURN(result);
}
void structqueue_put_commit(HSTRUCTQUEUE   h_queue)
{
    PSTRUCTQUEUE       Q       = (PSTRUCTQUEUE)h_queue;

    Q->bot += Q->recSize;
}

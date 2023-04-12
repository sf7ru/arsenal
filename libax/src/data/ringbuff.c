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

//#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arsenal.h>
#include <axdata.h>
#include <axsystem.h>
#include <axstring.h>


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

PAXRINGBUFF axringbuff_create(UINT               size)
{
    PAXRINGBUFF  result          = nil;

    ENTER(true);

    result = CREATE_X(AXRINGBUFF, size);

    RETURN(result);
}


void axringbuff_shake(PAXRINGBUFF        buff)
{
    if (buff->tail == buff->head)
    {
        buff->tail = buff->head = 0;
    }
}

INT axringbuff_write(PAXRINGBUFF        buff,
                     PVOID              data,
                     INT                size)
{
    INT         result          = -1;
    INT         tail;
    INT         left;

    ENTER(true);

    tail = buff->tail;
    left = buff->size - tail;

    if (left >= size)
    {
        buff->tail = tail + size;

        memcpy(buff->data + buff->tail, data, size);

        result = size;
    }

    RETURN(result);
}

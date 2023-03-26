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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arsenal.h>
#include <ringbuff.h>


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------


void ringbuff1_clean              (PRINGBUFF cbuf)
{
    if(cbuf)
    {
        cbuf->head = 0;
        cbuf->tail = 0;
    }
}
BOOL ringbuff1_is_empty(PRINGBUFF cbuf)
{
    return (cbuf->head == cbuf->tail);
}
//BOOL ringbuff1_is_full(PRINGBUFF cbuf)
//{
//    return ((cbuf->head + 1) % cbuf->size) == cbuf->tail;
//}
int ringbuff1_put(PRINGBUFF cbuf,
                  U8        data)
{
    int         result          = 0;

    ENTER(cbuf);

//    cbuf->data[cbuf->head] = data;
//    cbuf->head = (cbuf->head + 1) % cbuf->size;
//
//    if(cbuf->head == cbuf->tail)
//    {
//        cbuf->tail = (cbuf->tail + 1) % cbuf->size;
//    }

    cbuf->data[cbuf->head++] = data;

    if (cbuf->head >= cbuf->size)
        cbuf->head = 0;

    if(cbuf->head == cbuf->tail)
    {
        cbuf->tail++;

        if (cbuf->tail >= cbuf->size)
            cbuf->tail = 0;
    }

    result = 1;

    RETURN(result);
}
 int ringbuff1_get(PRINGBUFF         cbuf,
                   PU8               data)
{
    int         result = 0;

    ENTER(cbuf && data);

    if(!ringbuff1_is_empty(cbuf))
    {
//        *data = cbuf->data[cbuf->tail];
//        cbuf->tail = (cbuf->tail + 1) % cbuf->size;

        *data = cbuf->data[cbuf->tail++];
        if (cbuf->tail >= cbuf->size)
            cbuf->tail = 0;

        result = 1;
    }

    RETURN(result);
}
PRINGBUFF ringbuff1_create(UINT               size)
{
    PRINGBUFF  result          = nil;

    ENTER(size);

    if ((result = CREATE_X(RINGBUFF, size + 1)) != nil)
    {
        result->size = size;
    }

    RETURN(result);
}
PRINGBUFF ringbuff1_destroy(PRINGBUFF          b)
{
    SAFEFREE(b);

    return nil;
}
INT ringbuff1_write(PRINGBUFF          buff,
                    PVOID              data,
                    INT                size)
{
    INT         result          = 0;
    PU8         on              = (PU8)data;

    ENTER(true);

    while (result < size)
    {
        if (ringbuff1_put(buff, *(on++)) > 0)
        {
            result++;
        }
        else
            break;
    }

    RETURN(result);
}
INT ringbuff1_read(PRINGBUFF          buff,
                   PVOID              data,
                   INT                size)
{
    INT         result          = 0;
    PU8         on              = (PU8)data;

    ENTER(true);

    while (result < size)
    {
        if (ringbuff1_get(buff, (on++)) > 0)
        {
            result++;
        }
        else
            break;
    }

    RETURN(result);
}


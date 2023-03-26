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

#ifndef __RINGBUFF_H__
#define __RINGBUFF_H__

#include <arsenal.h>

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------


// ***************************************************************************
// STRUCTURE
//      RINGBUFF
// PURPOSE
//      Ring buffer with property
// ***************************************************************************
typedef struct __tag_RINGBUFF
{
    UINT            head;
    UINT            tail;
    UINT            size;
    U8              data	    [ 1 ];
} RINGBUFF, * PRINGBUFF;




// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif                                      //  #ifdef __cplusplus

// ringbuff.c

PRINGBUFF           ringbuff_destroy            (PRINGBUFF          b);

PRINGBUFF           ringbuff_create             (UINT               size);

INT                 ringbuff_write              (PRINGBUFF          pst_buff,
                                                 PVOID              p_data,
                                                 INT                i_size);

INT                 ringbuff_read               (PRINGBUFF          pst_buff,
                                                 PVOID              p_data,
                                                 INT                i_size);

void                ringbuff_clean              (PRINGBUFF          b);

BOOL                ringbuff_is_empty           (PRINGBUFF          cbuf);

// ringbuff1.c

PRINGBUFF           ringbuff1_destroy           (PRINGBUFF          b);

PRINGBUFF           ringbuff1_create            (UINT               size);

INT                 ringbuff1_put               (PRINGBUFF          buff,
                                                 U8                 data);

INT                 ringbuff1_get               (PRINGBUFF          buff,
                                                 PU8                data);

INT                 ringbuff1_write             (PRINGBUFF          buff,
                                                 PVOID              data,
                                                 INT                size);

INT                 ringbuff1_read              (PRINGBUFF          buff,
                                                 PVOID              data,
                                                 INT                size);


void                ringbuff1_clean             (PRINGBUFF          b);

BOOL                ringbuff1_is_empty          (PRINGBUFF          cbuf);


#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus

#endif                                      //  #ifndef __RINGBUFF_H__


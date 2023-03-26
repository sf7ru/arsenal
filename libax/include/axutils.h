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


#ifndef __AXUTILSH__
#define __AXUTILSH__

#include <arsenal.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define AXCON_FL_FLUSH      0x01

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus

#define axutils_delete(a)   { delete a; a = NULL; }

extern "C" {
#endif                                      //  #ifdef __cplusplus

PVOID               axutils_destroy             (PVOID              p_ptr);

UINT                read_int                    (PCVOID             p_int,
                                                 UINT               size,
                                                 BOOL               invert);

void                write_int                   (PCVOID             p_int,
                                                 UINT               value,
                                                 UINT               size,
                                                 BOOL               invert);

U64                 read_int64                  (PCVOID             p_int,
                                                 UINT               size,
                                                 BOOL               invert);

PVOID               axutils_realloc_lesser      (PVOID              mem,
                                                 UINT               size);


INT                 axcon_gets                  (PSTR               psz_string,
                                                 UINT               u_size,
                                                 UINT               flags,
                                                 UINT               TO);

INT                 axcon_getc                  (UINT               to);

#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus



#endif                                      //  #ifndef __AXUTILSH__


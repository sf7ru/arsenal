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


#ifndef __AXMATHH__
#define __AXMATHH__

#include <arsenal.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define AXCON_FL_FLUSH      0x01

#define PI                  (3.141592654)

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus

#define axutils_delete(a)   { delete a; a = NULL; }

extern "C" {
#endif                                      //  #ifdef __cplusplus

UINT                ax_parts                    (UINT               u_dividend,
                                                 UINT               u_divisor);

ULONG               ax_parts_l                  (ULONG              u_dividend,
                                                 UINT               u_divisor);

UINT                ax_round_up                 (UINT               u_dividend,
                                                 UINT               u_divisor);


INT                 ax_twoscomplement_to_int    (INT                smallInt,
                                                 INT                bits);

#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus



#endif                                      //  #ifndef __AXMATHH__


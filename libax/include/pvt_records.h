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

#ifndef __PVTRECORDSH__
#define __PVTRECORDSH__

#include <arsenal.h>

//#include <records.h>


// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// ---------------------------------------------------------------------------

#define GET_RECORD(a,b)         (((b) < (a)->u_used) ? (a)->p_data +    \
                                 ((a)->u_recsize * (b)) : NULL)


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// STRUCTURE
//      RECARRAY
// PURPOSE
//
// ***************************************************************************
typedef AXPACKED(struct) __tag_RECARRAY
{
    PFNRECCLEAN     pfn_clean;
    PVOID           p_ptr;

    PU8             p_data;

    UINT            u_total;
    UINT            u_used;
    UINT            u_recsize;
    UINT            u_amount;
} RECARRAY, * PRECARRAY;

// ***************************************************************************
// STRUCTURE
//      RECSET
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_RECSET
{
    PFNRECFREE      pfn_free;
    HRECARRAY       h_array;
} RECSET, * PRECSET;

// ***************************************************************************
// STRUCTURE
//      RECQUEUE
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_RECQUEUE
{
    UINT            u_recsize;
    UINT            u_maxrecs;
    UINT            u_init;

    PU8             p_top;
    PU8             p_bottom;
    PU8             p_end;
    PU8             p_data;

    PFNRECCLEAN     pfn_clean;
} RECQUEUE, * PRECQUEUE;

// ***************************************************************************
// STRUCTURE
//      STRUCTQUEUE
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_STRUCTQUEUE
{
    PU8             top; // top
    PU8             bot; // bottom
    PU8             end;
    PU8             begin;
    UINT            recSize;

} STRUCTQUEUE, * PSTRUCTQUEUE;

// ***************************************************************************
// STRUCTURE
//      PTRQUEUE
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_PTRQUEUE
{
    PVOID*          top;
    PVOID*          bot;
    PVOID*          end;
    PVOID*          begin;

//    HAXMUTEX        LCK;
} PTRQUEUE, * PPTRQUEUE;

// ***************************************************************************
// STRUCTURE
//      RECMAP
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_RECMAP
{
    UINT            u_divider;
    UINT            u_id_size;
    PVOID *         p_offsets;
    PFNRECFREE      pfn_free;
} RECMAP, * PRECMAP;

// ***************************************************************************
// STRUCTURE
//      IDMAP
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_IDMAP
{
    PIDREC *    map;
    UINT        used;
    UINT        size;
    UINT        idSize;
    UINT        allocAmount;
    PFNRECFREE  pfn_free;
} IDMAP, * PIDMAP;

// ***************************************************************************
// STRUCTURE
//      RECMAP
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_RECMAP2
{
    UINT            mask;
    UINT            idSize;
    UINT            amount;
    HIDMAP *        maps;
    PFNRECFREE      pfn_free;
} RECMAP2, * PRECMAP2;


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif                                      //  #ifdef __cplusplus



#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus

#endif                                      //  #ifndef __PVTRECORDSH__


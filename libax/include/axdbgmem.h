// ***************************************************************************
// TITLE
//      Memory debugging macros and functions
// PROJECT
//      Arsenal
// ***************************************************************************
//
// FILE
//      $Id:$
// HISTORY
//      $Log:$
// ***************************************************************************

#ifndef __AXDBGMEMH___
#define __AXDBGMEMH___

#include <arsenal.h>
#include <stdlib.h>

// ===========================================================================
// ================================= COMMON ==================================
// ===========================================================================

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// STRUCTURE
//      DBGMEMINFO
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_DBGMEMINFO
{
    UINT        d_curr_usage;
    UINT        d_max_usage;
    UINT        d_max_block;
} DBGMEMINFO, * PDBGMEMINFO;


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif                                      //  #ifdef __cplusplus

void                dbgmem_stop                 (void);

HAXHANDLE           dbgmem_start                (void);

void                dbgmem_set                  (HAXHANDLE          handle);

#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus


#define SAFEFREE(obj)         { if ((obj)) { FREE(obj); obj = NULL; } }
#define SAFECLOSE(a,b)      { if ((b)) { b = a##_close(b); } }
#define SAFECLOSE2(a,b,c)   { if ((b)) { b = (typeof(b))a##_close((c)b); } }
#define SAFEDESTROY(kind,obj)    { if ((obj)) { obj = kind##_destroy(obj); } }
#define SAFEDESTROY0(kind,obj) { if ((obj)) { kind##_destroy(obj); } }
#define SAFEDESTROY2(kind,obj,TO) { if ((obj)) { obj = kind##_destroy(obj,TO); } }
#define SAFEDELETE(obj)       { if (obj) { delete obj; obj = NULL;} }


#if (defined(DBGSOURCE) || ((!defined(DBGFULL) && !defined(DBGMEM))))

// ===========================================================================
// ============================= NON-DEBUG MODE ==============================
// ===========================================================================

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// ---------------------------------------------------------------------------

#define MALLOC(a)               LIBMALLOC(a)
#define MALLOC_D(a,b)           LIBMALLOC(a)
#define CALLOC(a,b)             LIBCALLOC(a,b)
#define CALLOC_D(a,b,c)         LIBCALLOC(a,b)
#define REALLOC(a,b)            LIBREALLOC(a, b)
#define REALLOC_D(a,b,c)        LIBREALLOC(a, b)
#define FREE(a)                 LIBFREE((PVOID)(a))

#define CREATE(a)               ((a*)LIBCALLOC(1, sizeof(a)))
#define CREATE_D(a,b)           CREATE(a)
#define CREATE_X(a,b)           ((a*)LIBCALLOC(1, sizeof(a) + b))
#define CREATE_XD(a,b,c)        CREATE_X(a,b)

#else                                       //  #ifdef DBGMEM

// ===========================================================================
// =============================== DEBUG MODE ================================
// ===========================================================================

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// ---------------------------------------------------------------------------

#ifndef DBGSOURCE
#define malloc              ERROR_XX_DO_NOT_USE_malloc
#define calloc(a,b)         ERROR_XX_DO_NOT_USE_calloc
#define realloc(a,b)        ERROR_XX_DO_NOT_USE_realloc
#define free(a)             ERROR_XX_DO_NOT_USE_free
#endif

#define MALLOC(a)           dbgmem_alloc(a, __AXPLACE__)
#define MALLOC_D(a,b)       dbgmem_alloc(a, (b == NULL ? __AXPLACE__ : b))
#define CALLOC(a,b)         dbgmem_calloc(a, b, __AXPLACE__)
#define CALLOC_D(a,b,c)     dbgmem_calloc(a, b, c)
#define REALLOC(a,b)        dbgmem_realloc(a,b, __AXPLACE__)
#define REALLOC_D(a,b,c)    dbgmem_realloc(a, b, c)
#define FREE(a)             dbgmem_free(a, __AXPLACE__)

#define CREATE(a)           (a*)dbgmem_create(sizeof(a), #a ,     __AXPLACE__)
#define CREATE_D(a,b)       (a*)dbgmem_create(sizeof(a), #a ,     b)
#define CREATE_X(a,b)       (a*)dbgmem_create(sizeof(a) + b, #a , __AXPLACE__)
#define CREATE_XD(a,b,c)    (a*)dbgmem_create(sizeof(a) + b, #a , c)


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif                                      //  #ifdef __cplusplus

void                dbgmem_free                 (PVOID              p_block,
                                                 PSTR               psz_desc);

PVOID               dbgmem_alloc                (UINT               u_size,
                                                 PSTR               psz_desc);

PVOID               dbgmem_realloc              (PVOID              p_block,
                                                 UINT               u_size,
                                                 PSTR               psz_desc);

PVOID               dbgmem_calloc               (UINT               u_num,
                                                 UINT               u_size,
                                                 PSTR               psz_desc);

PVOID               dbgmem_create               (UINT               u_size,
                                                 PSTR               psz_object,
                                                 PSTR               psz_decs);


#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus

#endif                                      //  #ifndef DBGMEM

#endif                                      //  #ifndef __AXDBGMEMH___



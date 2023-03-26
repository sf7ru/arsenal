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

#ifndef __PVTRESOURCESH__
#define __PVTRESOURCESH__

#include <arsenal.h>
#include <records.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// ---------------------------------------------------------------------------

#define id_KREZ             0x5A45524B      // "KREZ" - Main
#define id_KRSM             0x4D53524B      // "KRSM" - Simple
#define id_KRLC             0x434C524B      // "KRLC" - Locale
#define id_KRTY             0x5954524B      // "KRTY" - Type
#define id_KROW             0x574F524B      // "KROW" - Owner

#define DEF_JUST_ADDED      0x80000000      //


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// STRUCTURE
//      HEADER
// ***************************************************************************
#pragma pack(1)
typedef AXPACKED(struct) __tag_HEADER
{
    U32             u_magic;
    U32             u_main_offset;
    U32             u_enum_offset;
} HEADER, * PHEADER;
#pragma pack()

// ***************************************************************************
// STRUCTURE
//      ENUM
// ***************************************************************************
#pragma pack(1)
typedef AXPACKED(struct) __tag_ENUM
{
    U32             u_id;
    U32             u_number;
} ENUM, * PENUM;
#pragma pack()

// ***************************************************************************
// STRUCTURE
//      OFFS
// ***************************************************************************
#pragma pack(1)
typedef AXPACKED(struct) __tag_OFFS
{
    U32             u_id;
    U32             u_type;
    U32             u_offset;
    U32             u_size;
} OFFS, * POFFS;
#pragma pack()

// ***************************************************************************
// STRUCTURE
//      LOC
// ***************************************************************************
typedef struct __tag_LOC
{
    OFFS            st_offs;
    U32             u_attrib;
    PVOID           p_at_mem;
} LOC, * PLOC;


// ***************************************************************************
// STRUCTURE
//      CROESUS
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_CROESUS
{
    FILE *          h_source;
    HRECMAP         h_resmap;
    U32             u_number;
    U32             u_main_offset;
    U32             u_enum_offset;
} CROESUS, * PCROESUS;


// ***************************************************************************
// STRUCTURE
//      CROESUSMM
// PURPOSE
//      Matering mode
// ***************************************************************************
typedef struct __tag_CROESUSMM
{
    PCROESUS        pst_R;

    BOOL            b_flip_bytes;
    U32             u_added;
    UINT            u_flags;
    FILE *          h_target;
} CROESUSMM, * PCROESUSMM;

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif                                      //  #ifdef __cplusplus


PCROESUS            _croesus_open_base          (PCSTR              psz_filename);

#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus

#endif                                      //  #ifndef __PVTRESOURCESH__





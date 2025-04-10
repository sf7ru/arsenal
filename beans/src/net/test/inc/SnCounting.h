// ***************************************************************************
// TITLE
//
// PROJECT
//
// ***************************************************************************
//
// FILE
//
// HISTORY
//
// ***************************************************************************


#ifndef __SNCOUNTING_H__
#define __SNCOUNTING_H__

#include <arsenal.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------



// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef U64                 SNCOUNTINT;

typedef struct _tag_COUNTS
{
    ULONG       packets;
    SNCOUNTINT  size;
    AXTIME      last;
} COUNTS, * PCOUNTS;

typedef struct __tag_WAYCOUNTS
{
    UINT            lanId;
    COUNTS          in;
    COUNTS          out;
    COUNTS          drop;
} WAYCOUNTS, * PWAYCOUNTS;

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------


#endif // __SNCOUNTING_H__

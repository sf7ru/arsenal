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

#ifndef __EXTMEMORY_H__
#define __EXTMEMORY_H__

#include <arsenal.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

typedef struct __tag_EXTMEMORYPART
{
        UINT                device;
        UINT                offset;
        UINT                size;
} EXTMEMORYPART, * PEXTMEMORYPART;

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class ExtMemory
{
protected:
public:
virtual INT             read                    (PVOID          buff,
                                                 UINT           offset,
                                                 INT            size)   = 0;

virtual INT             write                   (PVOID          buff,
                                                 UINT           offset,
                                                 INT            size)   = 0;

virtual INT             getSize                 ()                      = 0;
       
};

#endif //  #ifndef __EXTMEMORY_H__

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

#ifndef __FLASHSTORAGEFILE_H__
#define __FLASHSTORAGEFILE_H__

#include <Storage.h>
#include <Flash.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define FLASHSTOR_PAGE_SIZE 256

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct __tag_FLASHBCR
{
    BOOL                    erased;
    UINT                    writeOffset;
} FLASHBCR, * PFLASHBCR;

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------


class FlashStorageFile: public StorageFile
{
        PFLASHBCR       bcrs;

        BOOL            flagNoErase;

        void            applyOffsets            (UINT           relativeOffset,
                                                 UINT           size,
                                                 UINT           begin,
                                                 UINT           end);

        UINT            calculateSectors         (PVOID          target,
                                                 UINT           size,
                                                 UINT &         begin,
                                                 UINT &         end,
                                                 UINT &         eraseBegin,
                                                 UINT &         eraseEnd);

protected:
        FLASHFILEPROPS  props;

        INT             cacheBase;
        PU8             cacheData;
        BOOL            cacheHit;

        BOOL            eraseFileRegion         (UINT           begin,
                                                 UINT           end);


        PCVOID          calculateAddress                    (INT            offset,
                                                 INT &          size);

virtual BOOL            eraseSectors            (UINT           begin,
                                                 UINT           end)
        { return false; }

virtual INT             programSectors          (UINT           begin,
                                                 UINT           end,
                                                 PVOID          target,
                                                 PCVOID         source,
                                                 INT            size)
        { return -1; }

virtual UINT            getSectorSizeByNum      (UINT           sector)
        = 0;

        INT             writeInt                (INT            offset,
                                                 PCVOID         data,
                                                 INT            size);

public:

                        FlashStorageFile        ();

virtual                 ~FlashStorageFile       ();

        BOOL            init                    (BOOL           writable);

virtual PCVOID          map                     (INT            offset,
                                                 INT            size);

        INT             write                   (INT            offset,
                                                 PCVOID         data,
                                                 INT            size);

virtual INT             read                    (INT            offset,
                                                 PVOID          data,
                                                 INT            size);

        INT             sync                    ();

        INT             getMaxSize              ()
        { return props.size; }

        INT             truncate                ();

        INT             ctl                     (UINT           option,
                                                 UINT           value);
};


#endif // #ifndef __FLASHSTORAGEFILE_H__

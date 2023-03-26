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

#ifndef __IAPSTORAGEFILE_H__
#define __IAPSTORAGEFILE_H__

#include <FlashStorageFile.h>

class IapStorageFile: public FlashStorageFile
{
protected:

        INT             programSectors          (UINT           begin,
                                                 UINT           end,
                                                 PVOID          target,
                                                 PCVOID         source,
                                                 INT            size);

        BOOL            eraseSectors            (UINT           begin,
                                                 UINT           end);

        UINT            getSectorSizeByNum      (UINT           block);
public:

        BOOL            open                    (StorageFileId  id,
                                                 BOOL           writable);

        INT             getSectorSize           ();

//        INT             alignWriteSize          (INT            size);

//        INT             truncate                (); // Inherited from FlashStorageFile
};

#endif // #define __IAPSTORAGEFILE_H__

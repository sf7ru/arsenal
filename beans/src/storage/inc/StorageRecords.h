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

#ifndef __STORAGEVARRECORDS_H__
#define __STORAGEVARRECORDS_H__

#include <Storage.h>

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct __tag_FINDRECORD
{
    UINT                    index;

    UINT                    keyAddr;
    UINT                    recAddr;
} FINDRECORD, * PFINDRECORD;


// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class StorageRecords
{
        StorageFile *   file;

        UINT            sectorSize;
        UINT            pageSize;
        UINT            keysPerPage;
        UINT            pagesPerSector;
        UINT            recsPerPage;
        PU8             indexPage;


        UINT            keySize;
        UINT            recordSize;
        UINT            recordAlignedSize;
        UINT            indexSizeInSectors;
        UINT            recordsNum;

        BOOL            keyContainsOnlyVal      (PVOID          key,
                                                 U8             val);


//        void            searchRecord            (PUINT          foundAddr,
//                                                 PUINT          deletedAddr,
//                                                 PUINT          nextAddr,
//                                                 PVOID          key);
//
        void            findRecord              (PFINDRECORD    found,
                                                 PFINDRECORD    next,
                                                 PVOID          key);


        UINT            recAddrByKeyAddr        (UINT           keyAddr);

        BOOL            calcLocations           (PFINDRECORD    locations);

        BOOL            deleteKey               (UINT           keyAddr);
public:

                        StorageRecords          ();

        INT             open                    (StorageFileId  fileId,
                                                 BOOL           writable,
                                                 UINT           keySize,
                                                 UINT           recordSize,
                                                 UINT           maxRecords);

        void            close                   (void);

        UINT            getSize                 ()
        { return recordsNum; }

        INT             set                     (PVOID          key,
                                                 PVOID          data);

        INT             get                     (PVOID          key,
                                                 PVOID          data);

        INT             del                     (PVOID          key);

        INT             clean                   ();

        StorageFile *   getFile                 ()
        { return file; }

};

#endif // #ifndef __STORAGEVARRECORDS_H__

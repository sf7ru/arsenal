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

#ifndef __STORAGEBLOCKS_H__
#define __STORAGEBLOCKS_H__

#include <Storage.h>

#define NUM_OF_BACKUP_SECTS     2

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct __tag_FINDRECORD
{
    UINT                    index;
    BOOL                    backup;

    UINT                    keyAddr;
    UINT                    recAddr;
} FINDRECORD, * PFINDRECORD;

typedef struct __tag_STORAGEBLKSEC
{
    UINT                    status;
    UINT                    offset;
} STORAGEBLKSEC, * PSTORAGEBLKSEC;

typedef struct _tag_RECBUFF
{
    UINT                    secOffset;
    UINT                    pageOffset;
    UINT                    offset;
    UINT                    num;
    PU8                     buff;
} RECBUFF, * PRECBUFF;


// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class StorageBlocks
{
        StorageFile *   file;

        UINT            totalSize;
        UINT            sectorSize;
        UINT            pageSize;

        UINT            pagesPerSector;
        UINT            recsPerPage;
        UINT            totalSectors;
        UINT            recordsNum;

        UINT            keySize;
        UINT            dataSize;
        UINT            recordSize;
        UINT            alignedSize;
        UINT            tailSize;

        PSTORAGEBLKSEC  sectors;

        INT             addToRecBuff            (PRECBUFF       recs,
                                                 PU8            rec);

        BOOL            findRecordOnPacked       (PU8            sector,
                                                 UINT           index,
                                                 PFINDRECORD    found,
                                                 PVOID          key);

        BOOL            findRecordOnWrite       (PU8            sector,
                                                 UINT           index,
                                                 PFINDRECORD    found,
                                                 PVOID          key);

        BOOL            findRecord              (PFINDRECORD    found,
                                                 PVOID          key);

        void            countSector             (PU8            sector,
                                                 PSTORAGEBLKSEC info);

        void            countWriteSector        (PU8            sector,
                                                 PSTORAGEBLKSEC info);

        void            countPackedSector       (PU8            sector,
                                                 PSTORAGEBLKSEC info);

        BOOL            checkNumberOfPlaces     (UINT           num,
                                                 BOOL           backup);

        BOOL            findEmptySector         (PFINDRECORD    found);

        BOOL            findPlaceOnPacked       (PFINDRECORD    found);

        BOOL            findPlaceOnWrite        (PFINDRECORD    found);

        INT             checkKey                (PCVOID         key,
                                                 BOOL           checkCs = false);

        UINT            recAddrByKeyAddr        (UINT           keyAddr);

        BOOL            copySector              (UINT           sector,
                                                 PVOID          key);

        BOOL            packSector              (UINT           sector,
                                                 BOOL           backup);

        BOOL            initStructure           ();

public:

                        StorageBlocks           ();
                        ~StorageBlocks          ();

        INT             open                    (StorageFileId  fileId,
                                                 UINT           keySize,
                                                 UINT           dataSize);

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

        void            dump                    ();
};

#endif // #ifndef __STORAGEBLOCKS_H__

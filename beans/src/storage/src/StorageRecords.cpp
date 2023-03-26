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

#include <arsenal.h>
#include <math.h>
#include <string.h>
#include <axstring.h>
#include <stdlib.h>

#include <customboard.h>

#include SUBSYS_INCLUDE(STORAGE)
#include <StorageRecords.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      StorageRecords::StorageRecords
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
StorageRecords::StorageRecords()
{
    file                = nil;
    keySize             = sizeof(UINT);
    recordSize          = 0;
    recordAlignedSize   = 0;
    indexSizeInSectors  = 0;
    recordsNum          = 0;
    sectorSize          = 4 KB;
    keysPerPage         = 0;
    pagesPerSector      = 0;
    recsPerPage         = 0;
    pageSize            = STORAGE_PAGE_SIZE;
    indexPage           = (PU8)MALLOC(pageSize);
}
// ***************************************************************************
// FUNCTION
//      StorageRecords::open
// PURPOSE
//
// PARAMETERS
//      StorageFileId fileId     --
//      BOOL          writable   --
//      UINT          keySize    --
//      UINT          recordSize --
//      UINT          maxRecords --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
INT StorageRecords::open(StorageFileId  fileId,
                         BOOL           writable,
                         UINT           keySize,
                         UINT           recordSize,
                         UINT           maxRecords)
{
    SUBSYS_STORAGE &    stor            = SUBSYS_STORAGE::getInstance();
    UINT                maxSize;

    ENTER(true);

    if ((file = stor.open(StorageFileId_var, writable)) != nil)
    {
        file->ctl(STORAGE_CTL_NOERASE, true);
        // Примерная задача:
        //
        // размер ключа 8 байт, размер записи 40 байт. размер страницы 256 байт. размер сектора 4096
        // 6 записей на страницу, 16 страниц (один сектор) может описать 512 записей, это 85 страниц, соотношение: 16÷(16+85)=0,158415842
        // всего страниц 28672, умножаем на соотношение = 4542,099021824 с округлением 4543 страниц на описатели = 283,9375 секторов
        // с округлением 284 секторов на описатели
        // 284 сектора могут описать 24140 страниц, (284×16)+(284×85) = 28684 страниц. последний сектор не полный.
        //
        // ответ задачи: 284 секторов на описатели.

                this->keySize           = keySize;
                this->recordSize        = recordSize;
                this->recordAlignedSize = MAC_ALIGN_U32(recordSize);

        //        pageSize            = stor.getPageSize();
        //        sectorSize          = stor.getSectorSize();

                            maxSize             = file->getMaxSize();
                            recsPerPage         = pageSize / recordAlignedSize;
                            keysPerPage         = pageSize / keySize;
                            pagesPerSector      = sectorSize / pageSize;
                UINT        pagesPerIdxBlock    = (keysPerPage * pagesPerSector) / recsPerPage;
                UINT        totalPages          = maxSize / pageSize;

                UINT        indexPages          = ceil(((double)pagesPerSector / (pagesPerSector + pagesPerIdxBlock)) * totalPages);

                            indexSizeInSectors  = ceil((double)indexPages / pagesPerSector);

                //printf("page size = %u, sector size = %u maxSize = %u\n", pageSize, sectorSize, maxSize);

                recordsNum          = (totalPages - indexPages) * recsPerPage;

                //printf("index size in sectors = %d, total records = %d\n", indexSizeInSectors, recordsNum);
    }

    RETURN(recordsNum);
}
// ***************************************************************************
// FUNCTION
//      StorageRecords::close
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      void --
// ***************************************************************************
void StorageRecords::close(void)
{
    if (file)
    {
        SUBSYS_STORAGE::getInstance().close(file);
    }
}
// ***************************************************************************
// FUNCTION
//      StorageRecords::deleteKey
// PURPOSE
//
// PARAMETERS
//      UINT keyAddr --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL StorageRecords::deleteKey(UINT           keyAddr)
{
    BOOL            result          = false;
    PVOID           zero;

    ENTER(true);

    //printf("StorageRecords::deleteKey stage 1: %X\n", keyAddr);
    if ((zero = MALLOC(keySize)) != nil)
    {
        //printf("StorageRecords::deleteKey stage 2\n");
        memset(zero, 0, keySize);

        result  = file->write(keyAddr, zero, keySize) == (INT)keySize;

        //printf("StorageRecords::deleteKey stage 3 = %d\n", result);

        FREE(zero);
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      StorageRecords::set
// PURPOSE
//
// PARAMETERS
//      PVOID key  --
//      PVOID data --
// RESULT
//      INT --
// ***************************************************************************
INT StorageRecords::set(PVOID          key,
                        PVOID          data)
{
    INT             result          = -1;
    FINDRECORD      found;
    FINDRECORD      next;

    ENTER(true);

//    printf("StorageRecords::set stage 1\n");
    findRecord(&found, &next, key);
//    printf("StorageRecords::set stage 2\n");

    if (next.index != AXII)
    {
        if ((found.index == AXII) || deleteKey(found.keyAddr))
        {
            if (calcLocations(&next))
            {
//                printf("saving key to idx %d, addr %X\n", next.index, next.keyAddr);

                if (file->write(next.keyAddr, key, keySize) == (INT)keySize)
                {
                    //file->sync();

//                    printf("saving data to addr %X\n", next.recAddr);

                    if (file->write(next.recAddr, data, recordSize) == (INT)recordSize)
                    {
                        file->sync();
                        result = 1;
                    }
                }
            }
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      StorageRecords::get
// PURPOSE
//
// PARAMETERS
//      PVOID key  --
//      PVOID data --
// RESULT
//      INT --
// ***************************************************************************
INT StorageRecords::get(PVOID          key,
                        PVOID          data)
{
    INT             result          = -1;
    FINDRECORD      found;

    ENTER(true);

    findRecord(&found, nil, key);

    if (found.index != AXII)
    {
        //printf("GET: record found at idx %d\n", found.index);

        if (calcLocations(&found))
        {
            //printf("StorageRecords::get key addr %X rec addr %X\n", found.keyAddr, found.recAddr);

            if (file->read(found.recAddr, data, recordSize) == (INT)recordSize)
            {
                result = recordSize;
            }
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      StorageRecords::del
// PURPOSE
//
// PARAMETERS
//      PVOID key --
// RESULT
//      INT --
// ***************************************************************************
INT StorageRecords::del(PVOID          key)
{
    INT             result          = -1;
//    UINT            kaddr;
//    UINT            daddr;

    ENTER(true);

//    switch (findRecord(&kaddr, &daddr, key))
//    {
//        case 0:
//            result = 1;
//            break;
//
//        case -1:
//            break;
//
//        default:
//            //result = unsetId(addr);
//            break;
//    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      StorageRecords::findRecord
// PURPOSE
//
// PARAMETERS
//      PFINDRECORD found --
//      PFINDRECORD next  --
//      PVOID       key   --
// RESULT
//      void --
// ***************************************************************************
void StorageRecords::findRecord(PFINDRECORD    found,
                                PFINDRECORD    next,
                                PVOID          key)
{
#define CURRENT_ADDR        (sector * sectorSize) + (page * pageSize) + (on - indexPage)

    BOOL            run             = true;
    PU8             on;
    UINT            keyIdx          = 0;
    UINT            pagesInSector   = sectorSize / pageSize;

    ENTER(true);

    found->index    = AXII;

    if (next)
    {
        next->index = AXII;
    }

    for (UINT sector = 0; run && (sector < indexSizeInSectors); ++sector)
    {
        for (UINT page  = 0; run  && (page < pagesInSector); ++page)
        {
            //printf("StorageRecords::searchRecord stage sector = %d page = %d\n", sector, page);

            on = indexPage;

            if (file->read(CURRENT_ADDR, indexPage, pageSize) > 0)
            {
                //strz_sdump_print(0, 0, 0, indexPage, pageSize);

                for (UINT idx = 0; run && (idx < keysPerPage); ++idx, on += keySize, ++keyIdx)
                {
                    //printf("StorageRecords::searchRecord stage 3 key = %d\n", keyIdx);

                    if (!keyContainsOnlyVal(on, 0xFF))
                    {
                        if (!keyContainsOnlyVal(on, 0x00) && !memcmp(key, on, keySize))
                        {
                            //printf("StorageRecords::searchRecord stage 4 found key = %d\n", keyIdx);

                            found->index        = keyIdx;
                            found->keyAddr      = CURRENT_ADDR;

                            if (!next)
                            {
                                run  = false;
                            }
                        }
                    }
                    else
                    {
                        //printf("StorageRecords::searchRecord stage untouched area reached!\n");

                        if (next && (next->index == AXII))
                        {
                            next->index             = keyIdx;
                            next->keyAddr           = CURRENT_ADDR;
                        }

                        run = false;
                    }
                }
            }
            else
            {
                AXTRACE("READ ERROR: %u %u!!!!!!!\n", CURRENT_ADDR, pageSize);
            }
        }

        //printf("StorageRecords::searchRecord stage 100\n");
    }

    QUIT;

#undef  CURRENT_ADDR
}
/*
void StorageRecords::searchRecord(PUINT          foundAddr,
                                  PUINT          deletedAddr,
                                  PUINT          nextAddr,
                                  PVOID          key)
{
#define CURRENT_ADDR        (sector * sectorSize) + (page * pageSize) + (on - indexPage)

    BOOL            run             = true;
    PU8             on;
    UINT            idx;
    UINT            pagesInSector   = sectorSize / pageSize;

    ENTER(true);

    for (UINT sector = 0; run && (sector < indexSizeInSectors); ++sector)
    {
        for (UINT page  = 0; run  && (page < pagesInSector); ++page)
        {
            printf("StorageRecords::searchRecord stage sector = %d page = %d\n", sector, page);

            on = indexPage;

            if (file->read(CURRENT_ADDR, indexPage, pageSize) > 0)
            {
                printf("StorageRecords::searchRecord stage 2\n");

                for (idx = 0; run && (idx < keysPerPage); ++idx, on += keySize)
                {
                    printf("StorageRecords::searchRecord stage 3 key = %d\n", idx);

                    if (!keyContainsOnlyVal(on, 0xFF))
                    {
                        if (!keyContainsOnlyVal(on, 0x00))
                        {
                            if (!memcmp(key, on, keySize))
                            {
                                if (foundAddr)
                                    *foundAddr = CURRENT_ADDR;

                                run = false;
                            }
                        }
                        else
                        {
                            if (deletedAddr && (*deletedAddr == AXII))
                            {
                                *deletedAddr = CURRENT_ADDR;
                            }
                        }
                    }
                    else
                    {
                        printf("StorageRecords::searchRecord stage untouched area reached!\n");
                        if (nextAddr)
                            *nextAddr = CURRENT_ADDR;

                        run = false;
                    }
                }
            }
        }

        printf("StorageRecords::searchRecord stage 100\n");
    }

    QUIT;

#undef  CURRENT_ADDR
}
*/
// ***************************************************************************
// FUNCTION
//      StorageRecords::keyContainsOnlyVal
// PURPOSE
//
// PARAMETERS
//      PVOID key --
//      U8    val --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL StorageRecords::keyContainsOnlyVal(PVOID          key,
                                        U8             val)
{
    BOOL            result          = true;
    PU8             on              = (PU8)key;

    ENTER(true);

    for (UINT cnt = 0; result && (cnt < keySize); ++cnt, ++on)
    {
        if (*on != val)
            result = false;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      StorageRecords::calcLocations
// PURPOSE
//
// PARAMETERS
//      PFINDRECORD locations --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL StorageRecords::calcLocations(PFINDRECORD    locations)
{
    BOOL            result          = false;


    ENTER(true);

    div_t       d1          = div((int)locations->index, (int)recsPerPage);
    div_t       d2          = div((int)d1.quot, (int)pagesPerSector);
    UINT        recSector   = d2.quot;
    UINT        recPage     = d2.rem;
    UINT        recNum      = d1.rem;

//    printf("StorageRecords::calcLocations  index = %u sector = %u page = %u num = %u\n",
//           locations->index,
//           recSector,
//           recPage,
//           recNum
//           );

    locations->recAddr  = (indexSizeInSectors * sectorSize) + (recSector * sectorSize) + (recPage * pageSize) + (recNum * recordAlignedSize);
    result              = true;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      StorageRecords::clean
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      INT  --
// ***************************************************************************
INT  StorageRecords::clean()
{
    return file->truncate();
}

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

/*
    == Начальные условия
    Запись возможна по страницам (256B), стирание возможно посекторно (32K).
    Перезаписывать страницы нельзя - портяться. Но можно намеренно портить страницы, допустим,
    с целью удаления записи.
    Реализованный алгоритм требует наличия всегда хотябы одного сектора EMPTY, который выполняет
    роль промежуточного буфера.
    Не могуть быть использованы ключи со значениями 0 и -1.

    == Хранение записей
    Сектора для хранения разделяются на EMPTY, WRITE, PACKED. Сохранение записей происходи в
    сектор WRITE постранично (одна страница - одна запись). После завершения сектора происходит
    его упаковка - копирование записей в сектор PACKED или EMPTY, упаковывая на границу
    выравнивания и начиная запись с границы страницы.

    == Удаление записей
    Удаление записи в секторе WRITE происходит порчей записи нулями, после этого необходимо
    проверить, что ключ имеет значение 0, если нет - повторить порчу.
    Удаление записи из сектора PACKED происходит копированием содержимого сектора в EMPTY без
    удаляемой записи.

    == Признаки
    Для различения секторов WRITE и PACKED в конце страниц в секторах WRITE добавляется
    признак !0xFF (0xFE), если страница не имеет такого признака, то если первая записть имеет ключ -1,
    а последний байт страницы равен 0xFF, это сектор EMPTY, иначе PACKED.
    Ключ со значением 0 служит признаком удаленной записи, также последний байт в записи равен !0xFF (0x00)

    == Поиск
    Поиск в секторах WRITE проводится до первого ключа со значением -1, т.к. этот сектор не должен
    содержать пропуски между записями.
    Cектоар PACKED может иметь неполные страницы, т.к. кол-во страниц, требуемых на упаковку
    сектора WRITE, зависит от размера записи. Поиск в секторе проводится постранично до конца,
    поиск записи на каждой странице проводится до первого ключа -1.

    == Данные секторов
    При старте происходит обход секторов и определения их статуса и смещения для записи.
    Определение статуса происходит по признакам (см Признаки), определение смещения
    происходит на граничу страницы.

 */
#include <arsenal.h>
#include <math.h>
#include <string.h>
#include <axstring.h>

#include <axhashing.h>

#include <customboard.h>

#include SUBSYS_INCLUDE(STORAGE)
#include <StorageBlocks.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define STATUS_empty        0
#define STATUS_packed       1
#define STATUS_write        2

#define CRC_CALC(a,b)       crc16_update(0x5A, a, b)

#define CHECKSUM_SIZE       sizeof(U16)

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct __tag_HEAD
{
    U16                     flags;
    U16                     sign;
} HEAD, * PHEAD;


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      StorageBlocks::StorageBlocks
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
StorageBlocks::StorageBlocks()
{
    file                = nil;
    keySize             = sizeof(UINT);
    dataSize            = 0;
    recordSize          = 0;
    alignedSize         = 0;
    tailSize            = 0;
    totalSize           = 0;
    pageSize            = 0;
    sectorSize          = 0;
    totalSectors        = 0;
    recordsNum          = 0;
    pagesPerSector      = 0;
    recsPerPage         = 0;
    sectors             = nil;
}
// ***************************************************************************
// TYPE
//      Destructor
// FUNCTION
//      StorageBlocks::~StorageBlocks
// PURPOSE
//      Class destruction
// PARAMETERS
//          --
// ***************************************************************************
StorageBlocks::~StorageBlocks()
{
    if (sectors)
        delete[] sectors;
}
// ***************************************************************************
// FUNCTION
//      StorageBlocks::close
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      void --
// ***************************************************************************
void StorageBlocks::close(void)
{
    if (file)
    {
        SUBSYS_STORAGE::getInstance().close(file);
    }
}
// ***************************************************************************
// FUNCTION
//      StorageBlocks::counWriteSector
// PURPOSE
//
// PARAMETERS
//      PU8            sector --
//      PSTORAGEBLKSEC info   --
// RESULT
//      void --
// ***************************************************************************
void StorageBlocks::countWriteSector(PU8            sector,
                                     PSTORAGEBLKSEC info)
{
    PU8             on      = sector;
    UINT            empty   = 0;
    UINT            used    = 0;
    PU8             offset  = nil;
    UINT            page;

    ENTER(true);

    for (page = 0, on = sector; page < pagesPerSector; page++, on += pageSize)
    {
        if (checkKey(on) == -1)
        {
            if (!offset)
                offset = on;

            empty++;
        }
        else
            used++;
    }

    info->status    = STATUS_write;
    info->offset    = MAC_ALIGN(offset - sector, pageSize);

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      StorageBlocks::countSector
// PURPOSE
//
// PARAMETERS
//      PU8            sector --
//      PSTORAGEBLKSEC info   --
// RESULT
//      void --
// ***************************************************************************
void StorageBlocks::countSector(PU8            sector,
                                PSTORAGEBLKSEC info)
{
    PU8             on      = sector;
    UINT            empty   = 0;
    PU8             offset  = nil;
    UINT            page;
    BOOL            feSign;

    ENTER(true);

    feSign = (0xFF != *(on + pageSize - 1));

    for (page = 0, on = sector; page < pagesPerSector; page++, on += pageSize)
    {
        if (checkKey(on) == -1)
        {
            if (!offset)
                offset = on;

            empty++;
        }
    }

    info->status    = feSign ? STATUS_write : (empty == pagesPerSector) ? STATUS_empty : STATUS_packed;
    info->offset    = offset ? offset - sector : 0;

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      StorageBlocks::initStructure
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL StorageBlocks::initStructure()
{
    BOOL                result          = false;
    PCVOID              begin;
    PU8                 on;

    ENTER(true);

    if ((sectors = new STORAGEBLKSEC[totalSectors]) != nil)
    {
        if ((begin = file->map(0, totalSize)) != nil)
        {
            on  = (PU8)begin;

            for (UINT sector = 0; sector < totalSectors; sector++, on += sectorSize)
            {
                countSector(on, &sectors[sector]);
                //printf("sector %u, status = %u, offset = %u\n", sector, sectors[sector].status, sectors[sector].offset);
            }

            result = true;

            file->unmap(begin);
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      StorageBlocks::open
// PURPOSE
//
// PARAMETERS
//      StorageFileId fileId   --
//      UINT          keySize  --
//      UINT          dataSize --
// RESULT
//      INT --
// ***************************************************************************
INT StorageBlocks::open(StorageFileId  fileId,
                        UINT           keySize,
                        UINT           dataSize)
{
    SUBSYS_STORAGE &    stor            = SUBSYS_STORAGE::getInstance();

    ENTER(true);

    if (keySize >= sizeof(U32))
    {
        if ((file = stor.open(StorageFileId_var, true)) != nil)
        {
            if (file->isMapable)
            {
                pageSize            = file->getPageSize();
                UINT maxRecSize     = MAC_ALIGN(dataSize + keySize + CHECKSUM_SIZE + 1, sizeof(U32));

                if (pageSize >= maxRecSize)
                {
                    file->ctl(STORAGE_CTL_NOERASE, true);

                    sectorSize          = file->getSectorSize();
                    totalSize           = file->getMaxSize();
                    this->keySize       = keySize;
                    this->dataSize      = dataSize;

                    recordSize          = keySize + dataSize;
                    alignedSize         = MAC_ALIGN(recordSize, sizeof(U32));
                    tailSize            = alignedSize - recordSize;

                    recsPerPage         = pageSize   / alignedSize;

                    // tail of page must always be non-zero, for non-0xFF sign
                    if ((recsPerPage * alignedSize) == pageSize)
                    {
                        recsPerPage--;
                    }

                    pagesPerSector      = sectorSize / pageSize;
                    UINT    totalPages  = totalSize  / pageSize;
                    totalSectors        = totalSize  / sectorSize;
                    recordsNum          = (totalPages - (pagesPerSector * NUM_OF_BACKUP_SECTS)) * recsPerPage; // N sectors must always be free

                    AXTRACE("page size = %u, sector size = %u totalSize = %u, recordsNum = %u, recsPerPage = %u, pagesPerSector = %u, recordAlignedSize = %u, keySize  = %u, dataSize = %u\n",
                           pageSize,
                           sectorSize,
                           totalSize,
                           recordsNum,
                           recsPerPage,
                           pagesPerSector,
                           alignedSize,
                           keySize, dataSize);

                    if (!initStructure())
                    {
                        close();
                    }
                }
                else
                {
                    AXTRACE("StorageBlocks::open data size exceeding page size\n");
                    close();
                }
            }
            else
            {
                AXTRACE("StorageBlocks::open file not memory mapable\n");
                close();
            }
        }
    }

    RETURN(recordsNum);
}
// ***************************************************************************
// FUNCTION
//      StorageBlocks::set
// PURPOSE
//
// PARAMETERS
//      PVOID key  --
//      PVOID data --
// RESULT
//      INT --
// ***************************************************************************
INT StorageBlocks::set(PVOID          key,
                       PVOID          data)
{
    INT             result          = -1;
    FINDRECORD      found;
    PU8             on;
    INT             rd;
    U8              buff            [ pageSize ];

    ENTER(true);

    //printf("StorageBlocks::set stage 1\n");
    if ((checkKey(key) > 0) && ((rd = del(key)) >= 0))
    {

        found.backup = rd > 0;

        if (  findPlaceOnWrite(&found)      ||
              findEmptySector(&found)       )
        {
            //printf("StorageBlocks::set: found.index = %u, found.keyAddr = %u\n", found.index, found.keyAddr);

            on = buff;
            memcpy(buff, key, keySize);

            on += keySize;
            memcpy(on, data, dataSize);

            on += dataSize;
            *((PU16)on) = CRC_CALC(key, keySize);

            on += CHECKSUM_SIZE;
            memset(on, 0xFF, pageSize - recordSize - CHECKSUM_SIZE - 1);

            *(buff + pageSize - 1) = 0xFE;

            if (file->write(found.keyAddr, buff, pageSize) == (INT)pageSize)
            {
                file->sync();

                sectors[found.index].status     = STATUS_write;
                sectors[found.index].offset    += pageSize;
                result                          = 1;
            }
        }
        else
        {
//            printf("place not found!\n");
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      StorageBlocks::get
// PURPOSE
//
// PARAMETERS
//      PVOID key  --
//      PVOID data --
// RESULT
//      INT --
// ***************************************************************************
INT StorageBlocks::get(PVOID          key,
                       PVOID          data)
{
    INT             result          = -1;
    FINDRECORD      found;

    ENTER(true);

    if (findRecord(&found, key))
    {
        //AXTRACE("GET: record found at idx %d\n", found.index);

        //AXTRACE("StorageBlocks::get key addr %X rec addr %X\n", found.keyAddr, found.recAddr);

        if (data)
        {
            result = file->read(found.recAddr, data, dataSize);
        }
        else
            result = dataSize;
    }
    else
        result = 0;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      StorageBlocks::del
// PURPOSE
//
// PARAMETERS
//      PVOID key --
// RESULT
//      INT --
// ***************************************************************************
INT StorageBlocks::del(PVOID          key)
{
    INT             result          = -1;
    FINDRECORD      found;

    ENTER(true);

    if (findRecord(&found, key))
    {
        if (STATUS_packed == sectors[found.index].status)
        {
            result = copySector(found.index, key);
        }
        else
        {
            U8 zeroPage [ pageSize ];

            memset(&zeroPage, 0, pageSize);

            result = file->write(found.keyAddr, zeroPage, pageSize);
        }
    }
    else
    {
        result = 0;
    }

//    printf("StorageBlocks::del stage 10 = %d\n", result);
//
//    {
//        for (UINT sector = 0; sector < totalSectors; sector++)
//        {
//            printf("sector %u, status = %u, offset = %u\n", sector, sectors[sector].status, sectors[sector].offset);
//        }
//    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      StorageBlocks::addToRecBuff
// PURPOSE
//
// PARAMETERS
//      PRECBUFF recs --
//      PU8      rec  --
// RESULT
//      INT --
// ***************************************************************************
INT StorageBlocks::addToRecBuff(PRECBUFF   recs,
                                PU8        rec)
{
    INT             result          = -1;

    ENTER(true);

    if (rec)
    {
        memcpy(recs->buff + recs->offset, rec, recordSize);

        recs->offset += alignedSize;
        recs->num++;
    }

    if  (!rec || (recs->num == recsPerPage))
    {
        if (file->write(recs->secOffset + recs->pageOffset, recs->buff, pageSize) > 0)
        {
            recs->pageOffset   += pageSize;
            recs->offset        = 0;
            result              = recs->num;
            recs->num           = 0;

            memset(recs->buff, 0xFF, pageSize);
        }
    }
    else
        result = 0;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      StorageBlocks::copySector
// PURPOSE
//
// PARAMETERS
//      UINT  sector --
//      PVOID key    --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL StorageBlocks::copySector(UINT           sector,
                               PVOID          key)
{
    BOOL                result          = false;
    FINDRECORD          empty;
    PU8                 onOrigin;
    PU8                 onPage;
    BOOL                copied;
    PU8                 begin;
    RECBUFF             recs;
    U8                  buff            [ pageSize ];

    ENTER(true);

    empty.backup = true;

    if (findEmptySector(&empty))
    {
        if ((begin = (PU8)file->map(0, totalSize)) != nil)
        {
            recs.buff       = buff;
            recs.num        = 0;
            recs.offset     = 0;
            recs.secOffset  = empty.index * sectorSize;
            recs.pageOffset = 0;

            memset(recs.buff, 0xFF, pageSize);

            onPage      = begin + (sector * sectorSize);

            for (UINT page = 0; page < pagesPerSector; page++, onPage += pageSize)
            {
                onOrigin = onPage;

                for (UINT rec = 0; rec < recsPerPage; rec++, onOrigin += alignedSize)
                {
                    if (checkKey((PCVOID)onOrigin) > 0)
                    {
                        if (!key || memcmp(key, onOrigin, keySize))
                        {
                            if (addToRecBuff(&recs, onOrigin) > 0)
                            {
                                copied = true;
                            }
                        }
                    }
                }
            }

            // Pack last
            if ((addToRecBuff(&recs, nil) > 0) || copied)
            {
                sectors[empty.index].status = STATUS_packed;
                sectors[empty.index].offset = recs.pageOffset;
            }

            file->ctl(STORAGE_CTL_ERASESEC, sector);
            sectors[sector].status = STATUS_empty;
            sectors[sector].offset = 0;

            result = true;

            file->unmap((PCVOID)begin);
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      StorageBlocks::checkNumberOfPlaces
// PURPOSE
//
// PARAMETERS
//      UINT num --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL StorageBlocks::checkNumberOfPlaces(UINT num,
                                        BOOL backup)
{
    BOOL                result          = false;
    PCVOID              begin;
    PU8                 on;
    UINT                found           = 0;
    UINT                cnt;
    UINT                count           = 0;

    ENTER(true);

    if ((begin = file->map(0, totalSize)) != nil)
    {
        on  = (PU8)begin;

        for (UINT sector = 0; (found < num) && (sector < totalSectors); sector++, on += sectorSize)
        {
            switch (sectors[sector].status)
            {
                case STATUS_empty:
                    if (backup || ((++count) > NUM_OF_BACKUP_SECTS))
                    {
                        cnt     = (pagesPerSector * recsPerPage);
                        found  += cnt;
                    }
                        //printf("StorageBlocks::checkNumberOfPlaces STATUS_empty: sector = %u cnt = %u\n", sector, cnt);
                    break;

                case STATUS_packed:
                    cnt     = ((sectorSize - sectors[sector].offset) / pageSize) * recsPerPage;
                    found  += cnt;
                    //printf("StorageBlocks::checkNumberOfPlaces STATUS_packed: sector = %u offset = %u cnt = %u\n", sector, sectors[sector].offset, cnt);
                    break;
            }
        }

        result = (found >= num);

        file->unmap(begin);
    }

    //printf("StorageBlocks::checkNumberOfPlaces stage result = %d\n", result);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      StorageBlocks::findPlaceOnPacked
// PURPOSE
//
// PARAMETERS
//      PFINDRECORD found --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL StorageBlocks::findPlaceOnPacked(PFINDRECORD    found)
{
    BOOL            result          = false;

    ENTER(true);

    for (UINT i = 0; !result && (i < totalSectors); i++)
    {
        if (  (sectors[i].status == STATUS_packed)  &&
              (sectors[i].offset < sectorSize)      )
        {
            found->index    = i;
            found->keyAddr  = (i * sectorSize) + sectors[i].offset;
            result          = true;
        }
    }

    //printf("StorageBlocks::findPlaceOnPacked stage resutl = %d\n", result);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      StorageBlocks::packSector
// PURPOSE
//
// PARAMETERS
//      UINT sector --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL StorageBlocks::packSector(UINT sector,
                               BOOL backup)
{
    BOOL                result          = false;
    FINDRECORD          empty;
    PU8                 onUnpacked;
    PU8                 onBuff;
    PU8                 begin;
    UINT                packed;
    UINT                unpacked        = pagesPerSector;
    U8                  buff            [ pageSize ];

    ENTER(true);

    //printf("StorageBlocks::packSector stage 1 sector = %u\n", sector);

    if (checkNumberOfPlaces(pagesPerSector, backup))
    {
        //printf("StorageBlocks::packSector stage 2\n");

        if ((begin = (PU8)file->map(0, totalSize)) != nil)
        {
            //printf("StorageBlocks::packSector stage 3\n");
            onUnpacked  = begin + (sector * sectorSize);
            result      = true;

            while (unpacked && result)
            {
                //printf("StorageBlocks::packSector stage unpacked = %u\n", unpacked);
                onBuff      = buff;
                packed      = 0;
                memset(buff, 0xFF, pageSize);

                while (unpacked && (packed < recsPerPage))
                {
                    if (checkKey((PCVOID)onUnpacked, true) > 0)
                    {
                        //printf("StorageBlocks::packSector stage onBuff = %p, onUnpacked = %p\n", onBuff, onUnpacked);
                        // If actual record is same
                        memcpy(onBuff, onUnpacked, recordSize);
                        onBuff += alignedSize;
                        packed++;
                    }

                    onUnpacked += pageSize;
                    unpacked--;
                }

                //printf("StorageBlocks::packSector stage packed = %u\n", packed);

                if (packed)
                {
                    empty.backup = true;

                    if (  findPlaceOnPacked(&empty)     ||
                          findEmptySector(&empty)       )
                    {
                        //printf("StorageBlocks::packSector stage 10 = %u + %u\n", empty.keyAddr, sectors[empty.index].offset);
//                        strz_sdump_print(0,   40, AXDUMP_SEP_DEFAULT, buff, pageSize);

                        file->write(empty.keyAddr, buff, pageSize);

                        sectors[empty.index].status  = STATUS_packed;
                        sectors[empty.index].offset += pageSize;
                    }
                    else
                        result = false;

                    //printf("StorageBlocks::packSector stage 11\n");
                }
            }

            //printf("StorageBlocks::packSector stage 12\n");

            //printf("StorageBlocks::packSector stage 13: sector = %u\n", sector);

            file->ctl(STORAGE_CTL_ERASESEC, sector);
            //printf("StorageBlocks::packSector stage 14\n");
            sectors[sector].status = STATUS_empty;
            sectors[sector].offset = 0;

            //printf("StorageBlocks::packSector stage 14\n");

            file->unmap((PCVOID)begin);
        }
    }

    //printf("StorageBlocks::packSector stage result = %d\n", result);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      StorageBlocks::findPlaceOnWrite
// PURPOSE
//
// PARAMETERS
//      PFINDRECORD found --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL StorageBlocks::findPlaceOnWrite(PFINDRECORD    found)
{
    BOOL            result          = false;

    ENTER(true);

    for (UINT i = 0; !result && (i < totalSectors); i++)
    {
        if (sectors[i].status == STATUS_write)
        {
            found->index    = i;
            found->keyAddr  = (i * sectorSize);

            if (sectors[i].offset < sectorSize)
            {
                found->keyAddr += sectors[i].offset;
                result          = true;
            }
            else if (packSector(i, found->backup))
            {
                result          = true;
            }
            else
            {
                AXTRACE("FATAL ERROR: cannot copy sector %u !!\n", i);
                break;
            }
        }
    }

    //printf("StorageBlocks::findPlaceOnWrite stage result = %d\n", result);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      StorageBlocks::findEmptySector
// PURPOSE
//
// PARAMETERS
//      PFINDRECORD found --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL StorageBlocks::findEmptySector(PFINDRECORD    found)
{
    BOOL                result          = false;
    UINT                count           = 0;

    ENTER(true);

    found->index = AXII;

    for (UINT i = 0; !result && (i < totalSectors); i++)
    {
        if (sectors[i].status == STATUS_empty)
        {
            if (found->index == AXII)
            {
                found->index    = i;
                found->keyAddr  = (i * sectorSize);
            }

            if (found->backup || ((++count) > NUM_OF_BACKUP_SECTS))
            {
                result = true;  // return found sector
            }
        }
    }

    //printf("StorageBlocks::findEmptySector stage result = %d\n", result);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      StorageBlocks::findOnStoreSector
// PURPOSE
//
// PARAMETERS
//      PU8         sector --
//      UINT        index  --
//      PFINDRECORD found  --
//      PVOID       key    --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL StorageBlocks::findRecordOnPacked(PU8            sector,
                                      UINT           index,
                                      PFINDRECORD    found,
                                      PVOID          key)
{
    BOOL                result          = false;
    PU8                 onPage;
    PU8                 on;

    ENTER(true);

    onPage = sector;

    for (UINT page = 0; !result && (page < pagesPerSector); page++, onPage += pageSize)
    {
        on = onPage;
        for (UINT rec = 0; !result && (rec < recsPerPage); rec++, on += alignedSize)
        {
            if ((checkKey(on) > 0) && !memcmp(key, on, keySize))
            {
                found->index    = index;
                found->keyAddr  = (index * sectorSize) + (on - sector);
                found->recAddr  = found->keyAddr + keySize;
                result          = true;
            }
        }
    }


    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      StorageBlocks::findOnWriteSector
// PURPOSE
//
// PARAMETERS
//      PU8         sector --
//      UINT        index  --
//      PFINDRECORD found  --
//      PVOID       key    --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL StorageBlocks::findRecordOnWrite(PU8            sector,
                                      UINT           index,
                                      PFINDRECORD    found,
                                      PVOID          key)
{
    BOOL                result          = false;
    PU8                 on;

    ENTER(true);

    on = sector;

    for (UINT page = 0; !result && (page < pagesPerSector); page++, on += pageSize)
    {
        if ((*(on + pageSize - 1) != 0xFF) && (checkKey(on, true) > 0))
        {
            if (!memcmp(key, on, keySize))
            {
                found->index    = index;
                found->keyAddr  = (index * sectorSize) + (on - sector);
                found->recAddr  = found->keyAddr + keySize;
                result          = true;
            }
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      StorageBlocks::findRecord
// PURPOSE
//
// PARAMETERS
//      PFINDRECORD found --
//      PVOID       key   --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL StorageBlocks::findRecord(PFINDRECORD    found,
                               PVOID          key)
{
#define CURRENT_ADDR        (sector * sectorSize) + (page * pageSize) + (onUnpacked - indexPage)

    BOOL                result             = false;
    PU8                 onSector;
    PU8                 begin;

    ENTER(true);

    if ((begin = (PU8)file->map(0, totalSize)) != nil)
    {
        onSector = begin;

        for (UINT i = 0; !result && (i < totalSectors); i++, onSector += sectorSize)
        {
            switch (sectors[i].status)
            {
                case STATUS_packed:
                    result = findRecordOnPacked(onSector, i, found, key);
                    break;

                case STATUS_write:
                    result = findRecordOnWrite(onSector, i, found, key);
                    break;

                default:
                    break;
            }
        }

        file->unmap((PCVOID)begin);
    }

    RETURN(result);

#undef  CURRENT_ADDR
}
// ***************************************************************************
// FUNCTION
//      StorageBlocks::checkKey
// PURPOSE
//
// PARAMETERS
//      PCVOID key --
// RESULT
//      INT --
// ***************************************************************************
INT StorageBlocks::checkKey(PCVOID key,
                            BOOL   checkCs)
{
    INT             result          = -1;
    PU32            on      = (PU32)key;

    ENTER(true);

    if (keySize > sizeof(U32))
    {
        if (*on == (U32)-1)
        {
            if (strz_memnchr(on + 1, keySize - sizeof(U32), 0xFF))
            {
                result = 1;
            }
        }
        else if (*on == 0)
        {
            if (strz_memnchr(on + 1, keySize - sizeof(U32), 0x0))
            {
                result = 1;
            }
            else
                result = 0;
        }
        else
            result = 1;
    }
    else
        result = *on;

    if ((result > 0) && checkCs)
    {
        if (CRC_CALC((PVOID)key, keySize) != *(PU16)(((PU8)key) + (keySize + dataSize)))
        {
            result = 0;
        }
    }

    RETURN(result);
}
void StorageBlocks::dump()
{
    PU8                 on;
    PU8                 begin;
    PU8                 onPage;
    PU8                 onSector;
    PU8                 end;
    UINT                count;

    ENTER(true);

    if ((begin = (PU8)file->map(0, totalSize)) != nil)
    {
        for (UINT sector = 0; sector < totalSectors; sector++)
        {
            onSector = begin + (sector * sectorSize);

            count   = 0;

            //printf("[ SECTOR %3d - ", sector);

            switch (sectors[sector].status)
            {
                case STATUS_empty:
                    //printf(" EMPTY ]\n");
                    break;

                case STATUS_packed:
                    //printf(" PACKED (size %u)]\n", sectors[sector].offset);

                    onPage  = onSector;
                    end     = onPage + sectors[sector].offset;

                    for (UINT page = 0; (onPage < end) && (page < pagesPerSector); page++, onPage += pageSize)
                    {
                        on = onPage;

                        for (UINT rec = 0; (on < end) && (rec < recsPerPage); rec++, on += alignedSize)
                        {
                            strz_dump("\nKEY: ", on, keySize);
                            strz_dump("REC: ", on + keySize, dataSize);

                            count++;
                        }
                    }
                    break;

                case STATUS_write:
                    //printf(" WRITE (size %u)]\n", sectors[sector].offset);

                    on = onSector;
                    end     = on + sectors[sector].offset;

                    for (UINT page = 0; (on < end) && (page < pagesPerSector); page++, on += pageSize)
                    {
                        strz_dump("\nKEY: ", on, keySize);

                        strz_dump("REC: ", on + keySize, dataSize);

                        strz_dump("CRC: ", on + keySize + dataSize, CHECKSUM_SIZE);

                        count++;
                    }

                    break;
            }

            //printf("[ ^^^  KEYS: %u]\n\n", count);
        }
    }

    QUIT;
}

// ***************************************************************************
// FUNCTION
//      StorageBlocks::clean
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      INT  --
// ***************************************************************************
INT  StorageBlocks::clean()
{
    INT             result          = -1;

    ENTER(true);

    if (file->truncate() > 0)
    {
        initStructure();
        result = 1;
    }

    RETURN(result);
}

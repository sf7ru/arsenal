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

#include <app.h>
#include <stdio.h>
#include <FlashStorageFile.h>
#include <string.h>
#include <axstring.h>

#include <axdbg.h>

//extern void myprintf(PCSTR, ...);
//
//#undef AXTRACE
//#define AXTRACE printf

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      FlashStorageFile::FlashStorageFile
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
FlashStorageFile::FlashStorageFile()
{
    bcrs        = nil;
    isMapable   = true;
    cacheBase   = -1;
    cacheData   = nil;
    cacheHit    = false;
    flagNoErase = false;
}
// ***************************************************************************
// TYPE
//      Destructor
// FUNCTION
//      FlashStorageFile::~FlashStorageFile
// PURPOSE
//      Class destruction
// PARAMETERS
//          --
// ***************************************************************************
FlashStorageFile::~FlashStorageFile()
{
    SAFEFREE(bcrs);
    SAFEFREE(cacheData);
}
// ***************************************************************************
// FUNCTION
//      FlashStorageFile::init
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL FlashStorageFile::init(BOOL           writable)
{
    BOOL            result          = false;
    UINT            size;

    ENTER(true);

    size = props.sectors * sizeof(FLASHBCR);

    if (writable)
    {
        if ((bcrs = (PFLASHBCR)MALLOC(size)) != nil)
        {
            memset(bcrs, 0, size);

            if ((cacheData = (PU8)MALLOC(FLASHSTOR_PAGE_SIZE)) != nil)
            {
                memset(cacheData, 0xFF, FLASHSTOR_PAGE_SIZE);

                cacheBase   = -1;
                cacheHit    = false;
                result      = true;
            }
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      FlashStorageFile::map
// PURPOSE
//
// PARAMETERS
//      INT offset --
//      INT size   --
// RESULT
//      PCVOID --
// ***************************************************************************
PCVOID FlashStorageFile::map(INT            offset,
                             INT            size)
{
    PCVOID          result          = AXIP;
    PU8             on;

    ENTER(true);

//    AXTRACE("FlashStorageFile::map stage offset + size = %u props.size = %u props.begin = %u props.ptr=%p\n",
//           offset + size, props.size,
//           props.begin,
//           props.ptr);

    if ((offset + size) <= props.size)
    {
        on      = (PU8)props.ptr;
        on     += offset;
        result  = on;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      FlashStorageFile::calcMapping
// PURPOSE
//
// PARAMETERS
//      INT   offset --
//      INT & size   --
// RESULT
//      PCVOID --
// ***************************************************************************
PCVOID FlashStorageFile::calculateAddress(INT            offset,
                                     INT &          size)
{
    PCVOID          result          = AXIP;
    PU8             on;

    ENTER(true);

    if (offset <= props.size)
    {
        on      = (PU8)props.ptr;
        on     += offset;
        size    = MAC_MIN(size, (props.size - offset));
        result  = on;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      FlashStorageFile::eraseRegion
// PURPOSE
//
// PARAMETERS
//      UINT begin --
//      UINT end   --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL FlashStorageFile::eraseFileRegion(UINT begin,
                                       UINT end)
{
    BOOL            result          = false;
    UINT            idx;
    UINT            sec;

    ENTER(true);

    if (eraseSectors(begin, end))
    {
        for (idx = (begin - props.begin), sec = begin; sec <= end; sec++, idx++)
        {
            bcrs[idx].erased        = true;
            bcrs[idx].writeOffset   = 0;
        }

        result = true;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      FlashStorageFile::applyOffsets
// PURPOSE
//
// PARAMETERS
//      UINT relativeOffset --
//      UINT size           --
//      UINT begin          --
//      UINT end            --
// RESULT
//      void --
// ***************************************************************************
void FlashStorageFile::applyOffsets(UINT relativeOffset,
                                    UINT size,
                                    UINT begin,
                                    UINT end)
{
    UINT            offset;
    UINT            sec;
    UINT            idx;
    UINT            secSize;

    //AXTRACE("FlashStorageFile::write stage relativeOffset = %u", relativeOffset);

    // current offset
    offset = size;

    for (idx = (begin - props.begin), sec = begin; offset && (sec <= end); sec++, idx++)
    {
        secSize                 = getSectorSizeByNum(sec);
        bcrs[idx].writeOffset   = MAC_MIN((offset + bcrs[idx].writeOffset), secSize);
        offset                 -= bcrs[idx].writeOffset;
    }



    //AXTRACE("FlashStorageFile::applyOffsets OFFSET: block idx = %u blk = %u offset = %u\n", idx, blk, bcrs[idx].writeOffset);

}
// ***************************************************************************
// FUNCTION
//      FlashStorageFile::calculateSectors
// PURPOSE
//
// PARAMETERS
//      PVOID  target     --
//      UINT   size       --
//      UINT & begin      --
//      UINT & end        --
//      UINT & eraseBegin --
//      UINT & eraseEnd   --
// RESULT
//      UINT --
// ***************************************************************************
UINT FlashStorageFile::calculateSectors(PVOID          target,
                                        UINT           size,
                                        UINT &         begin,
                                        UINT &         end,
                                        UINT &         eraseBegin,
                                        UINT &         eraseEnd)
{
    UINT            result      = AXII;
    PU8             wndBegin;
    PU8             wndEnd;
    PU8             secBegin;
    PU8             secEnd;
    UINT            sec;
    UINT            idx;
    UINT            secSize;
    UINT            secOffset;

    ENTER(true);

    wndBegin    = (PU8)target;
    wndEnd      = wndBegin + size;
    secBegin    = (PU8)props.ptr;
    sec         = props.begin;


//                       bB           bE
//                [ 0 ]  [ 1 ] [ 2 ] [ 3 ] [ 4 ]
//                         [ data      ]
//                         wB          wE

    //AXTRACE("FlashStorageFile::calculateSectors stage 1: props.pages = %d\n", props.sectors);

    for (idx = 0; idx < props.sectors; idx++, sec++)
    {
        secSize = getSectorSizeByNum(sec);
        secEnd  = secBegin + secSize;

//        AXTRACE("FlashStorageFile::calculateSectors stage 2: %d %d\n", sec, secSize);

        // 1 2 3

        //printf("FlashStorageFile::calculateSectors stage 2.1: %p %p %p %p\n", wndBegin, blkEnd, wndEnd, blkBegin);
        if (!((wndBegin >= secEnd) || (wndEnd <= secBegin)))
        {
            if (begin == AXII)
            {
                begin = sec;
            }

            if (result == AXII)
            {
                result = wndBegin - secBegin;
            }

            secOffset = (UINT)(wndBegin - secBegin);
            //AXTRACE("FlashStorageFile::calculateSectors: bcrs[idx].erased=%u, bcrs[idx].writeOffset=%u, blkOffset=%u\n", bcrs[idx].erased, bcrs[idx].writeOffset, secOffset);

            if ((!bcrs[idx].erased) || (bcrs[idx].writeOffset > secOffset))
            {
                //AXTRACE("FlashStorageFile::write EEEEEEEEEEEEEEEEERRRRRRRRAAAAAAAASEEEEEEEE %u\n", sec);

                if (eraseBegin == AXII)
                {
                    eraseBegin  = sec;
                }

                eraseEnd = sec;
            }

            end         = sec;
            wndBegin    = secEnd;
        }

        secBegin = secEnd;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      FlashStorageFile::write
// PURPOSE
//
// PARAMETERS
//      INT    offset --
//      PCVOID data   --
//      INT    size   --
// RESULT
//      INT --
// ***************************************************************************
INT FlashStorageFile::writeInt(INT            offset,
                               PCVOID         data,
                               INT            size)
{
    INT             result          = -1;
    UINT            begin           = AXII;
    UINT            end             = AXII;
    UINT            relativeOffset  = AXII;
    UINT            eraseBegin      = AXII;
    UINT            eraseEnd        = AXII;
    PVOID           target;

    ENTER(true);

    //AXTRACE("FlashStorageFile::writeInt stage 1: offset = %u\n", offset);
    //strz_sdump_print(offset, 0, 0, (PVOID)data, size);

    if ((target = (PVOID)calculateAddress(offset, size)) != AXIP)
    {
        //AXTRACE("FlashStorageFile::writeInt stage 2: target = %u\n", (UINT)((PU8)target - (PU8)props.ptr));

        if (size)
        {
            //AXTRACE("FlashStorageFile::writeInt stage 3: %p %d %d %d %d %d\n", target, size, begin, end, eraseBegin, eraseEnd);

            relativeOffset  = calculateSectors(target, size, begin, end, eraseBegin, eraseEnd);

            //AXTRACE("FlashStorageFile::writeInt stage 4: %p %d %d %d %d %d relativeOffset = %d\n", target, size, begin, end, eraseBegin, eraseEnd, relativeOffset);

            if (begin != AXII)
            {
//                AXTRACE("FlashStorageFile::writeInt stage 5: %d %d\n", eraseBegin, eraseEnd);

                if (flagNoErase || ((eraseBegin == AXII) || eraseFileRegion(eraseBegin, eraseEnd)))
                {
//                    axsleep(100);

                    //AXTRACE("FlashStorageFile::writeInt stage 6: %d %d\n", begin, end);

                    if ((result = programSectors(begin, end, target, data, size)) > 0)
                    {
                        //AXTRACE("FlashStorageFile::writeInt stage 7\n");
                        applyOffsets(relativeOffset, result, begin, end);
                    }
                }
            }
        }
        else
            result = 0;
    }

    //AXTRACE("FlashStorageFile::writeInt stage 10: result = %u\n", result);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      FlashStorageFile::write
// PURPOSE
//
// PARAMETERS
//      INT    offset --
//      PCVOID data   --
//      INT    size   --
// RESULT
//      INT --
// ***************************************************************************
INT FlashStorageFile::write(INT            offset,
                            PCVOID         data,
                            INT            size)
{
    INT             result          = -1;
    INT             len;
    PU8             onData;
    INT             currOffset;
    INT             base;
    INT             left;
    INT             ofs;

    ENTER(true);

    //AXTRACE(">>>>>>>>>>>>>>>>>>>>>>>>>>>> FlashStorageFile::write stage 1 offset = %d, size = %d\n", offset, size);

    if (cacheData)
    {
        onData      = (PU8)data;
        currOffset  = offset;
        left        = size;
        result      = 0;

        while (left && (result >= 0))
        {
            base    = currOffset & (~(FLASHSTOR_PAGE_SIZE - 1));
            ofs     = currOffset & ( (FLASHSTOR_PAGE_SIZE - 1));
            len     = FLASHSTOR_PAGE_SIZE - ofs;

            if (len > left)
                len = left;

            //AXTRACE("FlashStorageFile::write stage offset = %u size = %u, base = %u, ofs = %u\n", offset, size, base, ofs );

            if (base != cacheBase)
            {
                if (cacheBase != -1)
                {
                    sync();
                }

               // AXTRACE("FlashStorageFile::write stage 10: %u %u\n", base, FLASHSTOR_PAGE_SIZE);

                if (len != FLASHSTOR_PAGE_SIZE)
                {
                    if (read(base, cacheData, FLASHSTOR_PAGE_SIZE) > 0)
                    {
                        //AXTRACE("FlashStorageFile::write READED CACHE %u %u\n", base, FLASHSTOR_PAGE_SIZE);

                        //strz_sdump_print(base, 0, 0, cacheData, FLASHSTOR_PAGE_SIZE);
                        cacheBase = base;
                    }
                    else
                        result = -1;
                }
            }

            if (result != -1)
            {
                if (len != FLASHSTOR_PAGE_SIZE)
                {
                    memcpy(cacheData + ofs, onData, len);
                    cacheHit     = true;
                }
                else
                {
                    len = writeInt(base, onData, FLASHSTOR_PAGE_SIZE);
                }

                if (len > 0)
                {
                    result      += len;
                    onData      += len;
                    currOffset  += len;
                    left        -= len;

                    //AXTRACE("FlashStorageFile::write stage len = %d left = %d\n", len, left);
                }
                else
                    result = -1;
            }
        }
    }

    //AXTRACE("<<<<<<<<<<<<<<<<<<<<<<<<<<<< FlashStorageFile::write result = %d\n", result);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      FlashStorageFile::sync
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      INT --
// ***************************************************************************
INT FlashStorageFile::sync()
{
    INT             result          = -1;

    ENTER(true);

    //AXTRACE("FlashStorageFile::sync stage 1\n");

    if (cacheData)
    {
        //AXTRACE("FlashStorageFile::sync stage 2\n");
        if (cacheHit)
        {
            //AXTRACE("FlashStorageFile::sync page base offset 0x%X !!!!!\n", cacheBase);

            if ((result = writeInt(cacheBase, cacheData, FLASHSTOR_PAGE_SIZE)) > 0)
            {
                cacheHit = false;
            }
        }
        else
            result = 0;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      FlashStorageFile::read
// PURPOSE
//
// PARAMETERS
//      PCVOID data   --
//      INT    offset --
//      INT    size   --
// RESULT
//      INT --
// ***************************************************************************
INT FlashStorageFile::read(INT            offset,
                           PVOID          data,
                           INT            size)
{
    INT             result          = -1;
    PVOID           on;

    ENTER(true);

    if ((on = (PVOID)calculateAddress(offset, size)) != AXIP)
    {
        if (size)
        {
            memcpy(data, on, size);

            result = size;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      FlashStorageFile::truncate
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      INT --
// ***************************************************************************
INT FlashStorageFile::truncate()
{
    INT             result          = -1;

    ENTER(true);

    result = eraseFileRegion(props.begin, props.begin + props.sectors - 1) ? 1 : 0;
    //result = eraseRegion(props.begin, props.begin + 3) ? 1 : 0;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      FlashStorageFile::ctl
// PURPOSE
//
// PARAMETERS
//      UINT option --
//      UINT value  --
// RESULT
//      INT --
// ***************************************************************************
INT FlashStorageFile::ctl(UINT           option,
                          UINT           value)
{
    INT             result          = false;

    ENTER(true);

    switch (option)
    {
        case STORAGE_CTL_NOERASE:
            flagNoErase = value;
            break;

        case STORAGE_CTL_ERASESEC:
            result = eraseFileRegion(props.begin + value, props.begin + value) ? 1 : -1;
            break;

        case STORAGE_CTL_TEST:
            //flagNoErase = value;

            if (read(value, cacheData, FLASHSTOR_PAGE_SIZE) > 0)
            {
                //strz_sdump_print(value, 0, 0, cacheData, FLASHSTOR_PAGE_SIZE);
                result = 1;
            }
            break;

    }


    RETURN(result);
}

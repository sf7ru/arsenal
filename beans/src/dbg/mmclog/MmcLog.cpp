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

#include <MmcLog.h>
#include <MmcSpi.h>
#include <string.h>
#include <axhashing.h>
#include <axtime.h>
#include <app.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define DEFAULT_READ_TO     500
#define DEFAULT_WRITE_TO    1000

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

void MmcLog::taskMmcLog(MmcLog *   log)
{
    UINT prevIndex;

    while (1)
    {
        if (log->mBuffSwitched)
        {
            prevIndex  = log->mBuffIndex ? 0 : 1;

            axmutex_lock(log->LCK, true);
            log->mmc.writeBlock(log->mBlockNumber, log->mBuff[prevIndex], 2000);
            log->mBlockNumber++;
            axmutex_unlock(log->LCK);

            log->mBuffSwitched = false;
        }

        axsleep(10);
    }
}

MmcLog::MmcLog()
{
    mBlocksNum      = 0;

    mBlockNumber    = 1;

    mBuffIndex      = 0;
    mBuffOffset     = 0;
    mBuffSwitched   = false;

    useHeader       = false;
}
BOOL MmcLog::loadHeader()
{
    BOOL                result = false;
    PDBGLOGHEADER    h;
    
    ENTER(true);
    
    if (mmc.readBlock(0, mBuff[0], DEFAULT_WRITE_TO) > 0)
    {
        h = (PDBGLOGHEADER)mBuff[0];

        if (DEBUGLOG_MAGIC == h->magic)
        {
            mBlockNumber    = h->mBlockNumber;
            mBuffOffset     = h->mBuffOffset;

            result          = true;
        }
    }
    
    RETURN(result);
}
BOOL MmcLog::saveHeader(UINT       freeIndex)
{
    BOOL                result = false;
    PDBGLOGHEADER    h;
    
    ENTER(true);
    
    h               = (PDBGLOGHEADER)mBuff[freeIndex];
    h->magic        = DEBUGLOG_MAGIC;
    h->mBlockNumber = mBlockNumber;
    h->mBuffOffset  = mBuffOffset;

    if (mmc.writeBlock(0, h, DEFAULT_WRITE_TO) > 0)
    {
        result          = true;
    }
    
    RETURN(result);
}
// static void dump(const char * name, PVOID data, int size)
// {
//     PU8 on = (PU8)data;

//     printf("%s: ", name);

//     for (int i = 0; i < size; i++, on++)
//     {
//         printf("%.2x ", *on);
//     }

//     printf("\n");
// }
BOOL MmcLog::open(UINT           recSize, 
                    BOOL           append,
                    int            ifaceNo,
                    PORTPIN        ppCs,
                    PORTPIN        ppReset)
{
    BOOL            result = false;
    
    ENTER(true);

    if (recSize > 0)
    {
        mmc.mmcInit(ifaceNo, ppCs, ppReset);

        if ((mBlocksNum = mmc.getNumOfBlocks()) > 0)
        {
            mRecSize = recSize;

            if (append)
            {
                useHeader = loadHeader();
            }

            if ((LCK = axmutex_create()) != nil)
            {
                axthread_create(&thread1,
                                0,
                                DEFAULT_TASK_STACK_SIZE,
                                DEFAULT_PRIORITY, (PFNAXTHREAD)MmcLog::taskMmcLog, (PVOID)this);

                printf("Debug MMC capacity: %d blocks, %u Mbytes, next rec at block %d offset %d\n", mBlocksNum, mBlocksNum / 2000, mBlockNumber, mBuffOffset);

                result = true;
            }
        }
    }

    RETURN(result);
}
void MmcLog::close()
{
    if (mBuffOffset)
    {
        whriteCurrBlock();
    }
}
BOOL MmcLog::whriteCurrBlock()
{
    BOOL            result = false;
    
    ENTER(true);
    
    axmutex_lock(LCK, true);
    if (mmc.writeBlock(mBlockNumber, mBuff[mBuffIndex], 2000) > 0)
    {
        result = true;
    }
    axmutex_unlock(LCK);
    
    RETURN(result);
}
BOOL MmcLog::log(PDBGLOGENTRY   rec)
{
    BOOL            result = false;
    PDBGLOGENTRY    tgt;
    
    ENTER(true);
    
    if ((mBuffOffset + mRecSize) > MMC_BLOCK_SIZE)
    {
        mBuffIndex      = mBuffIndex ? 0 : 1;
        mBuffOffset     = 0;
        mBuffSwitched   = true;

        memset(mBuff[mBuffIndex], 0, MMC_BLOCK_SIZE);
    }

    tgt = (PDBGLOGENTRY)&mBuff[mBuffIndex][mBuffOffset];

    memcpy(tgt, rec, mRecSize);
    
    tgt->crc = adler32_calc((&tgt->crc) + 1, mRecSize - sizeof(DBGLOGENTRY));

    //dump("saved: ", tgt, mRecSize);

    mBuffOffset += mRecSize;
    result = true;
    
    RETURN(result);
}
void MmcLog::sync()
{
    whriteCurrBlock();
}
PDBGLOGENTRY MmcLog::logRead(PDBGLOGREAD    ctl)
{
    PDBGLOGENTRY        result      = nil;
    BOOL                flagReady   = true;

    ENTER(true);

    if (!ctl->mBLockNumber)
        ctl->mBuffOffset = MMC_BLOCK_SIZE;

    if ((ctl->mBuffOffset + mRecSize) > MMC_BLOCK_SIZE)
    {
        ctl->mBLockNumber++;
        
        if (ctl->mBLockNumber < mBlockNumber)
        {
            ctl->mBuffOffset = 0;

            axmutex_lock(LCK, true);
            if (mmc.readBlock(ctl->mBLockNumber, ctl->mBuff, DEFAULT_READ_TO) > 0)
            {
                ctl->mBuffOffsetMax = MMC_BLOCK_SIZE;
            }
            else
                flagReady = false;
            axmutex_unlock(LCK);

            // if (flagReady)
            //     printf("blk #%d/%d\n", ctl->mBLockNumber, mBlockNumber);
            // else
            //     printf("error reading sector %d\n", ctl->mBLockNumber);
        }
        else if (ctl->mBLockNumber == mBlockNumber)
        {
//            printf("block in mem\n");

            ctl->mBuffOffset = 0;

            memcpy(ctl->mBuff, mBuff[mBuffIndex], MMC_BLOCK_SIZE);

            ctl->mBuffOffsetMax = mBuffOffset;
        }
        else
            flagReady = false;
    }

    if (flagReady && (ctl->mBuffOffset < ctl->mBuffOffsetMax))
    {
        result = (PDBGLOGENTRY)&ctl->mBuff[ctl->mBuffOffset];

        U16 crc = adler32_calc((&result->crc) + 1, mRecSize - sizeof(DBGLOGENTRY));

        if (result->crc == crc)
        {
            ctl->mBuffOffset += mRecSize;
        }
        else
            result = nil;
    }
    
    RETURN(result);
}

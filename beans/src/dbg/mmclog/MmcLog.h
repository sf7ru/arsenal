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

#ifndef __MmcLog_H__
#define __MmcLog_H__

#include <MmcSpi.h>
#include <axthreads.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define DEBUGLOG_MAGIC      0x0DEB0DEB


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct __tag_DBGLOGHEADER
{
    U32                     magic;    
    U32                     mBlockNumber;
    U32                     mBuffOffset;
} DBGLOGHEADER, * PDBGLOGHEADER;

typedef struct __tag_DBGLOGENTRY
{
    U16                     crc;
} DBGLOGENTRY, * PDBGLOGENTRY;

typedef struct __tag_DBGLOGREAD
{
    U32                     mBLockNumber;
    U32                     mBuffOffset;
    U32                     mBuffOffsetMax;
    U8                      mBuff [ MMC_BLOCK_SIZE ];
} DBGLOGREAD, * PDBGLOGREAD;

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class MmcLog
{
protected:
        MmcSpi          mmc;
        HAXMUTEX        LCK;

        HAXTHREAD       thread1;

        BOOL            useHeader;

        UINT            mRecSize;

        int             mBlocksNum;

        UINT            mBlockNumber;

        UINT            mBuffIndex;
        UINT            mBuffOffset;
        BOOL            mBuffSwitched;

        U8              mBuff                   [2] [ MMC_BLOCK_SIZE];

        BOOL            whriteCurrBlock         ();

        BOOL            loadHeader              ();

        BOOL            saveHeader              (UINT       freeIndex);

static  void            taskMmcLog              (MmcLog *   log);

public: 

                        MmcLog                ();


        BOOL            open                    (UINT           recSize, 
                                                 BOOL           append,
                                                 int            ifaceNo,
                                                 PORTPIN        ppCs,
                                                 PORTPIN        ppReset);

        void            close                   ();                                                 


        BOOL            log                     (PDBGLOGENTRY   rec);

        PDBGLOGENTRY    logRead                 (PDBGLOGREAD    ctl);

        void            sync                    ();
};

#endif // #define __MmcLog_H__

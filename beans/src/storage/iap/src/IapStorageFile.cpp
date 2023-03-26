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

#include <IapStorage.h>
#include <Iap.h>

//extern void myprintf(PCSTR, ...);
//
//#undef AXTRACE
//#define AXTRACE printf

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      IapStorageFile::open
// PURPOSE
//
// PARAMETERS
//      StorageFileId id --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL IapStorageFile::open(StorageFileId id,
                          BOOL          writable)
{
    BOOL            result                      = false;
    Iap &           iap                         = Iap::getInstance();

    ENTER(true);

    if (iap.getFileProps(&props, id) && props.size)
    {
        init(writable);

        isMapable   = true;
        result      = true;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      IapStorageFile::programSectors
// PURPOSE
//
// PARAMETERS
//      PCVOID data   --
//      INT    offset --
//      INT    size   --
// RESULT
//      INT --
// ***************************************************************************
INT IapStorageFile::programSectors(UINT           begin,
                                 UINT           end,
                                 PVOID          target,
                                 PCVOID         source,
                                 INT            size)
{
    INT             result          = -1;
    Iap &           iap                         = Iap::getInstance();

    ENTER(true);

    AXTRACE("IapStorageFile::programSectors begin=%u end=%u target=%p size=%u\radius\n",
           begin, end, target, size);

    AXTRACE("IapStorageFile::programSectors stage 1 %d %d\n", begin, end);

//    for (int i = 0; i < 100; i++)
//    {
//        myprintf("%x ", *((PU8)target + i));
//    }
//
//    myprintf("\n");


    if (iap.prepare(begin, end))
    {
        AXTRACE("IapStorageFile::programSectors stage 2 target = %p, source = %p, size = %d \n", target, source, size);

        result = iap.write(target, (void*)source, size);

        iap.unprepare(begin, end);

        AXTRACE("IapStorageFile::programSectors stage 3 %d \n", result);
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      IapStorageFile::eraseSectors
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
BOOL IapStorageFile::eraseSectors(UINT           begin,
                                UINT           end)
{
    BOOL            result          = false;
    Iap &           iap             = Iap::getInstance();

    ENTER(true);


    //AXTRACE("@@@@@@@@@@@@@@@@@@@@2 IapStorageFile::eraseSectors begin=%u end=%u\n", begin, end);

    if (iap.prepare(begin, end))
    {
        result = iap.erase(begin, end);

        iap.unprepare(begin, end);
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      IapStorageFile::getSectorSize
// PURPOSE
//
// PARAMETERS
//      UINT block --
// RESULT
//      UINT --
// ***************************************************************************
UINT IapStorageFile::getSectorSizeByNum(UINT           block)
{
    UINT            result          = false;
    Iap &           iap                         = Iap::getInstance();

    ENTER(true);

    result = iap.getSectorSizeByNum(block);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      IapStorageFile::getSectorSize
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      INT --
// ***************************************************************************
INT IapStorageFile::getSectorSize()
{
    INT             result          = -1;

    ENTER(true);

    result = getSectorSizeByNum(props.begin + props.sectors - 1);

    RETURN(result);
}

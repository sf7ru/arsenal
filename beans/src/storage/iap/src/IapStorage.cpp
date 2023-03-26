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
#include <Storage.h>
#include <FlashStorageFile.h>
#include <Hal.h>
#include <Iap.h>

#include <update.h>

#include <axdbg.h>

#include <IapStorage.h>

//#define     USER_FLASH_AREA_START   FLASH_SECTOR_29
//#define     USER_FLASH_AREA_SIZE    (FLASH_SECTOR_SIZE_16_TO_29 * 1)
//
//
//
//unsigned char user_area[ USER_FLASH_AREA_SIZE ] __attribute__((section( ".ARM.__at_" USER_FLASH_AREA_START_STR( USER_FLASH_AREA_START ) )));


/*
void memdump( char *base, int n )
{
    unsigned int    *p;

    AXTRACE( "  memdump from 0x%08X for %d bytes", (unsigned int)base, n );

    p   = (unsigned int *)((unsigned int)base & ~(unsigned int)0x3);

    for ( int i = 0; i < (n >> 2); i++, p++ ) {
        if ( !(i % 4) )
            AXTRACE( "\radius\n  0x%08X :", (unsigned int)p );

        AXTRACE( " 0x%08X", *p );
    }

    AXTRACE( "\radius\n" );
}

int testmain()
{
    char    mem[ MEM_SIZE ];    //  memory, it should be aligned to word boundary
    int     radius;
    Iap &   iap = Iap::getInstance();

    AXTRACE( "\radius\n\radius\n=== IAP: Flash memory writing test ===\radius\n" );
    AXTRACE( "  device-ID = 0x%08X, serial# = 0x%08X\radius\n", iap.readId(), iap.readSerial());
    AXTRACE( "  user reserved flash area: start_address=0x%08X, size=%d bytes\radius\n", (int)iap.reservedAreaStart(), iap.reservedAreaSize() );
    AXTRACE( "  read_BootVer=0x%08X\radius\radius\n", iap.readBootVer() );

    for ( int i = 0; i < MEM_SIZE; i++ )
        mem[ i ]    = i & 0xFF;

    //  blank check: The mbed will erase all flash contents after downloading new executable

    radius   = iap.blankCheck( TARGET_SECTOR, TARGET_SECTOR );
    AXTRACE( "blank check result = 0x%08X\radius\n", radius );

    //  erase sector, if required

//    if ( radius == SECTOR_NOT_BLANK )
    {
        iap.prepare( TARGET_SECTOR, TARGET_SECTOR );
        radius   = iap.erase( TARGET_SECTOR, TARGET_SECTOR );
        AXTRACE( "erase result       = 0x%08X\radius\n", radius );
    }

    // copy RAM to Flash

    __disable_irq();
    iap.prepare( TARGET_SECTOR, TARGET_SECTOR );
    radius   = iap.write((char*)FLASH_SECTOR_29,  mem, MEM_SIZE);
    __enable_irq();

    AXTRACE( "copied: SRAM(0x%08X)->Flash(0x%08X) for %d bytes. (result=0x%08X)\radius\n", (int)mem, (int)FLASH_SECTOR_29, MEM_SIZE, radius );

    // compare

    radius   = iap.compare((char*)FLASH_SECTOR_29,  mem, MEM_SIZE );
    AXTRACE( "compare result     = \"%s\"\radius\n", radius ? "FAILED" : "OK" );

    AXTRACE( "showing the flash contents...\radius\n" );
    memdump((char*)FLASH_SECTOR_29, MEM_SIZE * 3 );


    return 0;
}


 */

//static struct __tag_file
//{
//    UINT        id          = FLASHSE
//};

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

//typedef struct __tag_IAPFILEHDR
//{
//    U32                     cheksum;
//    U32                     size;
//    U32                     fileId;
//    U32                     reserved    [ 5 ];
//} IAPFILEHDR, * PIAPFILEHDR;


// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define PAGE_SIZE           256

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

/*
// ***************************************************************************
// FUNCTION
//      IapStorageFile::alignWriteSize
// PURPOSE
//
// PARAMETERS
//      INT size --
// RESULT
//      INT --
// ***************************************************************************
INT IapStorageFile::alignWriteSize(INT            size)
{
    INT             result          = 0;

    ENTER(true);

    if (size % PAGE_SIZE)
    {
        size = ((size / PAGE_SIZE) + 1) * PAGE_SIZE;
    }

    RETURN(result);
}
*/
// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      IapStorage::IapStorage
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
IapStorage::IapStorage()
{

}
 // ***************************************************************************
 // TYPE
 //      Destructor
 // FUNCTION
 //      IapStorage::~IapStorage
 // PURPOSE
 //      Class destruction
 // PARAMETERS
 //          --
 // ***************************************************************************
IapStorage::~IapStorage()
{

}
// ***************************************************************************
// FUNCTION
//      IapStorage::init
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL IapStorage::init()
{
    BOOL            result          = true;

    ENTER(true);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      IapStorage::open
// PURPOSE
//
// PARAMETERS
//      INT id --
// RESULT
//      StorageFile * --
// ***************************************************************************
StorageFile * IapStorage::open(StorageFileId id,
                            BOOL          writable)
{
    IapStorageFile *    result          = nil;

    ENTER(true);

    if ((result = new IapStorageFile()) != nil)
    {
        if (!result->open(id, writable))
        {
            SAFEDELETE(result);
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      IapStorage::close
// PURPOSE
//
// PARAMETERS
//      StorageFile * file --
// RESULT
//      StorageFile * --
// ***************************************************************************
StorageFile * IapStorage::close(StorageFile *  file)
{
    ENTER(true);

    if (file)
    {
        file->sync();

        delete file;
    }

    RETURN(nil);
}
// ***************************************************************************
// FUNCTION
//      IapStorage::getId
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      U32 --
// ***************************************************************************
INT IapStorage::getId(PVOID        buff,
                   INT          size)
{
    INT             result          = 0;
    Iap &           iap             = Iap::getInstance();

    ENTER(true);

    result = iap.readSerial(buff, size);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      IapStorage::performUpdate
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      UINT --
// ***************************************************************************
UINT IapStorage::performUpdate(StorageFile *  updateFile)
{
    U32                 result          = ULURESULT_failed;
    StorageFile *       programFile;
    PULUFILE            head;
    PU8                 update;
    PU8                 program;
    Hal &               hal             = Hal::getInstance();


    ENTER(true);

    if ((result = checkUpdate(updateFile)) == ULURESULT_done)
    {
        if ((head = (PULUFILE)updateFile->map(0, updateFile->getMaxSize())) != AXIP)
        {
            if ((programFile = open(StorageFileId_program)) != nil)
            {
                // No check for nil, because nil for program is ok
                program = (PU8)programFile->map(0, programFile->getMaxSize());

                update = (PU8)(head + 1);

                if (hal.sys.prepareForUpdate())
                {
                    hal.sys.writeProgram(program, update, head->size);
                    //programFile->write(0, update, head->size);

                    result = ULURESULT_done;

                    hal.sys.reboot();
                }
            }
        }
    }

    RETURN(result);
}

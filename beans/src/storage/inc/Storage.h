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

#ifndef __STORAGE_H__
#define __STORAGE_H__

#include <arsenal.h>

// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define STORAGE_PAGE_SIZE   256

#define STORAGE_CTL_NOERASE  1
#define STORAGE_CTL_NOCACHE  2
#define STORAGE_CTL_ERASESEC 254
#define STORAGE_CTL_TEST     255

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef enum
{
    StorageFileId_bootld     = 0,
    StorageFileId_program,
    StorageFileId_update,
    StorageFileId_log,
    StorageFileId_var,
    StorageFileId_config,
    StorageFileId_EXT
} StorageFileId;


// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class Storage;

class StorageFile
{
public:
        Storage *       storage;

        BOOL            isMapable;

                        StorageFile             ()
        {  isMapable = false; }

virtual                 ~StorageFile            ()
        {  }

virtual INT             write                   (INT            offset,
                                                 PCVOID         data,
                                                 INT            size)
        { return -1; }

virtual INT             read                    (INT            offset,
                                                 PVOID          data,
                                                 INT            size)
        { return -1; }

virtual PCVOID          map                     (INT            offset,
                                                 INT            size)
        { return nil; }

virtual PCVOID          unmap                    (PCVOID        mem)
        { return nil; }

virtual INT             getMaxSize              ()
        { return -1; }

virtual INT             getPageSize             ()
        { return 256; }

virtual INT             getSectorSize           ()
        { return 4 KB; }

virtual INT             sync                    ()
        { return 0; }

virtual INT             truncate                ()
        { return 0; }

virtual INT             ctl                     (UINT           option,
                                                 UINT           value)
        { return 0; }
};

class Storage
{
protected:

public:
virtual BOOL            init                    () = 0;

                        Storage                 ()
                        {}

virtual                 ~Storage                ()
                        {}

virtual StorageFile *   open                    (StorageFileId  id,
                                                 BOOL           writable = false) = 0;

virtual StorageFile *   close                   (StorageFile *  file) = 0;

virtual UINT            checkUpdate             (StorageFile *  file);

virtual UINT            performUpdate           (StorageFile *  file) = 0;

virtual INT             getId                   (PVOID          buff,
                                                 INT            size) = 0;
};

#endif // #define __STORAGE_H__

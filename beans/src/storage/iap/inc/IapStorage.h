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

#ifndef __IAPSTORAGE_H__
#define __IAPSTORAGE_H__

#include <Storage.h>
#include <FlashStorageFile.h>
#include <IapStorageFile.h>

#include <singleton_template.hpp>

// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------


// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class IapStorage: public Storage, public SingletonTemplate<IapStorage>
{
protected:

public:
        BOOL            init                    ();

                        IapStorage              ();

                        ~IapStorage             ();

        StorageFile *   open                    (StorageFileId  id,
                                                 BOOL           writable = false);

        StorageFile *   close                   (StorageFile *  file);

        UINT            performUpdate           (StorageFile *  file);

        INT             getId                   (PVOID          buff,
                                                 INT            size);
};

#endif // #define __IAPSTORAGE_H__

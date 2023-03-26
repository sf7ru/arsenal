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

#include <stdio.h>

#include <app.h>
#include <Storage.h>
#include <update.h>
#include <axhashing.h>
#include <axstring.h>


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      Storage::checkUpdate
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      UINT --
// ***************************************************************************
UINT Storage::checkUpdate(StorageFile *   updateFile)
{
    UINT            result      = ULURESULT_not_ready;
    PULUFILE        head;
    PU8             on;
    U32             cs;

    ENTER(true);

        if ((head = (PULUFILE)updateFile->map(0, updateFile->getMaxSize())) != AXIP)
        {
            on      = (PU8)(head + 1);
            result  = ULURESULT_mismatch;

            if (  (head->product  == ULUPRODUCT_program)    &&
                  (head->platform == APP_HW_PLATFORM)       &&
                  (head->model    == APP_HW_MODEL)          )
            {
                cs = fletcher32_calc(on, head->size);

                if (cs == head->checksum)
                {
                    result = ULURESULT_done;
                }
            }
        }


    RETURN(result);
}

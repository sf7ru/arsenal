// ***************************************************************************
// TITLE
//      Mutual Execution Object handing functions for Win32 Platform
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: mutex.c,v 1.1 2003/04/24 10:07:28 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************


#include <stdlib.h>

#include <windows.h>

#include <arsenal.h>

#include <axthreads.h>


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------


// ***************************************************************************
// FUNCTION
//      axmutex_destroy
// PURPOSE
//      Destroy Dtalock control structure
// PARAMETERS
//      HAXMUTEX pst_lock -- Pointer to Mutex control structure
// RESULT
//      HAXMUTEX -- Always NULL
// ***************************************************************************
HAXMUTEX axmutex_destroy(HAXMUTEX h_lock)
{
    CRITICAL_SECTION *  pst_lock    = (CRITICAL_SECTION*)h_lock;

    ENTER(pst_lock);

    DeleteCriticalSection(pst_lock);
    FREE(pst_lock);

    RETURN(NULL);
}
// ***************************************************************************
// FUNCTION
//      axmutex_create
// PURPOSE
//      Create Mutex control structure
// PARAMETERS
//      none
// RESULT
//      HAXMUTEX -- Pointer to the just created Mutex control structure
//                      if all is ok, otherwise NULL
// ***************************************************************************
HAXMUTEX axmutex_create(void)
{
    CRITICAL_SECTION *      pst_lock    = NULL;

    ENTER(true);

    if ((pst_lock = CREATE(CRITICAL_SECTION)) != NULL)
    {
        InitializeCriticalSection(pst_lock);
    }

    RETURN((HAXMUTEX)pst_lock);
}
// ***************************************************************************
// FUNCTION
//      axmutex_lock
// PURPOSE
//      Lock data
// PARAMETERS
//      HAXMUTEX    pst_lock   -- Pointer to Mutex control structure
//      BOOL         b_wait -- Wait for data releasing or not
// RESULT
//      BOOL -- true if all is ok or false if error has occured
// NOTE
//      Function will not return until data released and potentially
//      may block execution in 'wait for data releasing' mode
// ***************************************************************************
BOOL axmutex_lock(HAXMUTEX  h_lock,
                  BOOL      b_wait)
{
    BOOL                b_result    = false;
    CRITICAL_SECTION *  pst_lock    = (CRITICAL_SECTION*)h_lock;

    ENTER(pst_lock);

    if (b_wait)
    {
        EnterCriticalSection(pst_lock);
        b_result = true;
    }
    else
        b_result = TryEnterCriticalSection(pst_lock);

    RETURN(b_result);
}

// ***************************************************************************
// FUNCTION
//      axmutex_unlock
// PURPOSE
//      Unlock data
// PARAMETERS
//      HAXMUTEX pst_lock -- Pointer to Mutex control structure
// RESULT
//      BOOL -- true if all is ok or false if error has occured
// ***************************************************************************
BOOL axmutex_unlock(HAXMUTEX h_lock)
{
    BOOL                b_result    = false;
    CRITICAL_SECTION *  pst_lock    = (CRITICAL_SECTION*)h_lock;

    ENTER(pst_lock);

    LeaveCriticalSection(pst_lock);

    b_result = true;

    RETURN(b_result);
}


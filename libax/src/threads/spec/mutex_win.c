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

#include <a7threads.h>


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------


// ***************************************************************************
// FUNCTION
//      a7mutex_destroy
// PURPOSE
//      Destroy Dtalock control structure
// PARAMETERS
//      HA7MUTEX pst_lock -- Pointer to Mutex control structure
// RESULT
//      HA7MUTEX -- Always NULL
// ***************************************************************************
HA7MUTEX a7mutex_destroy(HA7MUTEX h_lock)
{
    CRITICAL_SECTION *  pst_lock    = (CRITICAL_SECTION*)h_lock;

    ENTER(pst_lock);

    DeleteCriticalSection(pst_lock);
    FREE(pst_lock);

    RETURN(NULL);
}
// ***************************************************************************
// FUNCTION
//      a7mutex_create
// PURPOSE
//      Create Mutex control structure
// PARAMETERS
//      none
// RESULT
//      HA7MUTEX -- Pointer to the just created Mutex control structure
//                      if all is ok, otherwise NULL
// ***************************************************************************
HA7MUTEX a7mutex_create(void)
{
    CRITICAL_SECTION *      pst_lock    = NULL;

    ENTER(true);

    if ((pst_lock = CREATE(CRITICAL_SECTION)) != NULL)
    {
        InitializeCriticalSection(pst_lock);
    }

    RETURN((HA7MUTEX)pst_lock);
}
// ***************************************************************************
// FUNCTION
//      a7mutex_lock
// PURPOSE
//      Lock data
// PARAMETERS
//      HA7MUTEX    pst_lock   -- Pointer to Mutex control structure
//      BOOL         b_wait -- Wait for data releasing or not
// RESULT
//      BOOL -- true if all is ok or false if error has occured
// NOTE
//      Function will not return until data released and potentially
//      may block execution in 'wait for data releasing' mode
// ***************************************************************************
BOOL a7mutex_lock(HA7MUTEX  h_lock,
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
//      a7mutex_unlock
// PURPOSE
//      Unlock data
// PARAMETERS
//      HA7MUTEX pst_lock -- Pointer to Mutex control structure
// RESULT
//      BOOL -- true if all is ok or false if error has occured
// ***************************************************************************
BOOL a7mutex_unlock(HA7MUTEX h_lock)
{
    BOOL                b_result    = false;
    CRITICAL_SECTION *  pst_lock    = (CRITICAL_SECTION*)h_lock;

    ENTER(pst_lock);

    LeaveCriticalSection(pst_lock);

    b_result = true;

    RETURN(b_result);
}


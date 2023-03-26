// ***************************************************************************
// TITLE
//      Mutual Execution Object handing functions for UNIX platform
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: mutex.c,v 1.2 2003/06/04 13:16:48 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************


#include <string.h>
#include <stdlib.h>

#include <axthreads.h>
#include <FreeRTOS.h>
#include <semphr.h>


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      axmutex_init
// PURPOSE
//
// PARAMETERS
//      HAXMUTEX mutex --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
//BOOL axmutex_init(HAXMUTEX      mutex)
//{
//    BOOL                result          = false;
//    xSemaphoreHandle    mx              = (xSemaphoreHandle)mutex;
//
//    ENTER(mx);
//
//
//    RETURN(result);
//}
// ***************************************************************************
// FUNCTION
//      axmutex_deinit
// PURPOSE
//
// PARAMETERS
//      HAXMUTEX h_lock --
// RESULT
//      void --
// ***************************************************************************
//void axmutex_deinit(HAXMUTEX h_lock)
//{
//    ENTER(pst_lock);
//
//    QUIT;
//}
// ***************************************************************************
// FUNCTION
//      axmutex_destroy
// PURPOSE
//
// PARAMETERS
//      HAXMUTEX lck --
// RESULT
//      HAXMUTEX --
// ***************************************************************************
HAXMUTEX axmutex_destroy(HAXMUTEX lck)
{
    ENTER(lck);

    vSemaphoreDelete((xSemaphoreHandle)lck);

    RETURN(nil);
}
// ***************************************************************************
// FUNCTION
//      axmutex_create
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      HAXMUTEX --
// ***************************************************************************
HAXMUTEX axmutex_create(void)
{
    xSemaphoreHandle    result      = nil;

    ENTER(true);

    vSemaphoreCreateBinary(result);

    RETURN((HAXMUTEX)result);
}
// ***************************************************************************
// FUNCTION
//      axmutex_lock
// PURPOSE
//
// PARAMETERS
//      HAXMUTEX   h_lock   --
//      BOOL       b_wait     -- Wait for releasing or not
// RESULT
//      BOOL  --
// NOTE
//      Function will not returns until mutex released and potentially
//      may block thread execution
// ***************************************************************************
BOOL axmutex_lock(HAXMUTEX      h_lock,
                  BOOL          b_wait)
{
    BOOL                b_result    = false;
    xSemaphoreHandle    mx              = (xSemaphoreHandle)h_lock;

    ENTER(mx);

    b_result = (xSemaphoreTake(mx, (b_wait ? portMAX_DELAY : 0)) != pdFALSE);

    RETURN(b_result);
}

BOOL axmutex_lock_x(HAXMUTEX      h_lock,
                    BOOL          b_wait,
                    BOOL          isr)
{
    BOOL                b_result    = false;
    xSemaphoreHandle    mx              = (xSemaphoreHandle)h_lock;

    ENTER(mx);

    if (isr)
        b_result = (xSemaphoreTakeFromISR(mx, nil) != pdFALSE);
    else
        b_result = (xSemaphoreTake(mx, (b_wait ? portMAX_DELAY : 0)) != pdFALSE);

    RETURN(b_result);
}

// ***************************************************************************
// FUNCTION
//      axmutex_unlock
// PURPOSE
//
// PARAMETERS
//      HAXMUTEX   h_lock --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL axmutex_unlock(HAXMUTEX h_lock)
{
    BOOL                b_result        = false;
    xSemaphoreHandle    mx              = (xSemaphoreHandle)h_lock;

    ENTER(mx);

    xSemaphoreGive(mx);
    b_result    = true;

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axmutex_unlock_x
// PURPOSE
//
// PARAMETERS
//      HAXMUTEX h_lock --
//      BOOL     isr    --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL axmutex_unlock_x(HAXMUTEX  h_lock,
                      BOOL      isr)
{
    BOOL                b_result        = false;
    xSemaphoreHandle    mx              = (xSemaphoreHandle)h_lock;

    ENTER(mx);

    if (isr)
        xSemaphoreGiveFromISR(mx, nil);
    else
        xSemaphoreGive(mx);

    b_result    = true;

    RETURN(b_result);
}

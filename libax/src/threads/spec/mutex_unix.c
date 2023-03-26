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

#include <arsenal.h>
#include <axthreads.h>
#include <pvt_threads.h>

#include <pthread.h>


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
BOOL axmutex_init(HAXMUTEX      mutex)
{
    BOOL                result          = false;
    pthread_mutex_t *   mx;
    pthread_mutex_t     st_m        = PTHREAD_MUTEX_INITIALIZER;

    ENTER(true);

    mx = (pthread_mutex_t *)mutex;

    memcpy(mx, &st_m, sizeof(st_m));

    if (pthread_mutex_init(mx, NULL) != -1)
    {
        result = true;
    }

    RETURN(result);
}
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
void axmutex_deinit(HAXMUTEX h_lock)
{
    pthread_mutex_t*    pst_lock    = (pthread_mutex_t*)h_lock;

    ENTER(pst_lock);

    pthread_mutex_destroy(pst_lock);

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      axmutex_destroy
// PURPOSE
//      Destroy mutex
// PARAMETERS
//      HAXMUTEX   h_lock -- Handle of mutex
// RESULT
//      HAXMUTEX  -- Always NULL
// ***************************************************************************
HAXMUTEX axmutex_destroy(HAXMUTEX h_lock)
{
    ENTER(h_lock);

    axmutex_deinit(h_lock);

    FREE(h_lock);

    RETURN(NULL);
}
// ***************************************************************************
// FUNCTION
//      axmutex_create
// PURPOSE
//
// PARAMETERS
//         none
// RESULT
//      HAXMUTEX  -- Handler of created mutex control, or NULL
// ***************************************************************************
HAXMUTEX axmutex_create(void)
{
    pthread_mutex_t*    pst_lock    = NULL;

    ENTER(true);

    if ((pst_lock = MALLOC(sizeof(pthread_mutex_t))) != NULL)
    {
        if (!axmutex_init((HAXMUTEX)pst_lock))
        {
            pst_lock = (pthread_mutex_t*)axmutex_destroy((HAXMUTEX)pst_lock);
        }
    }

    RETURN((HAXMUTEX)pst_lock);
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
    pthread_mutex_t*    pst_lock    = (pthread_mutex_t*)h_lock;

    ENTER(pst_lock);

    b_result = b_wait                               ?
                !pthread_mutex_lock(pst_lock)       :
                !pthread_mutex_trylock(pst_lock)    ;

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
    BOOL                b_result    = false;
    pthread_mutex_t*    pst_lock    = (pthread_mutex_t*)h_lock;

    ENTER(pst_lock);

    pthread_mutex_unlock(pst_lock);
    b_result = true;

    RETURN(b_result);
}


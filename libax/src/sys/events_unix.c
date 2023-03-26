// ***************************************************************************
// TITLE
//      Events on UNIX Platform
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: events.c,v 1.4 2003/11/10 17:01:38 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************


#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <limits.h>


#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <arsenal.h>
#include <axsystem.h>




// ---------------------------------------------------------------------------
// -------------------------- LOCAL DEFINITIONS ------------------------------
// ---------------------------------------------------------------------------

#define MAC_AVAIL(a)        ((ioctl((a), FIONREAD, &i_avail) != -1) && i_avail)



// ---------------------------------------------------------------------------
// ----------------------------- LOCAL TYPES ---------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// STRUCTURE
//      AXEVENT
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_AXEVENT
{
    BOOL                b_manual;
    int                 ai_fd       [2];
} AXEVENT, * PAXEVENT;

// ***************************************************************************
// STRUCTURE
//      A7EVENT
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_A7EVENT
{
    sem_t               sem;
    int                 pshared;
    BOOL                inited;
} A7EVENT, * PA7EVENT;


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------


// ***************************************************************************
// FUNCTION
//      axevent_destroy
// PURPOSE
//      Destroy Event control structure
// PARAMETERS
//      HAXEVENT h_event   -- Handler of Event
// RESULT
//      HAXEVENT -- Always NULL
// ***************************************************************************
HAXEVENT axevent_destroy(HAXEVENT h_event)
{
    PAXEVENT           pst_event   = (PAXEVENT)h_event;

    ENTER(pst_event);

    if (pst_event->ai_fd[0] != -1)
        close(pst_event->ai_fd[0]);

    if (pst_event->ai_fd[1] != -1)
        close(pst_event->ai_fd[1]);

    FREE(pst_event);

    RETURN(NULL);
}

// ***************************************************************************
// FUNCTION
//      axevent_create
// PURPOSE
//      Create Event control structure
// PARAMETERS
//      BOOL      b_initial      -- Initial state of event
//      BOOL      b_manual_reset -- Make ability for manual event reset
// RESULT
//      HAXEVENT  -- Handler of a new createtd Event control structure
//                    or NULL
// ***************************************************************************
HAXEVENT axevent_create(BOOL b_initial, BOOL b_manual_reset)
{
    PAXEVENT           pst_event   = NULL;

    ENTER(true);

    if ((pst_event = CREATE(AXEVENT)) != NULL)
    {
        pst_event->ai_fd[0] = -1;
        pst_event->ai_fd[1] = -1;
        pst_event->b_manual = true;

        if (  (pipe(pst_event->ai_fd) == 0)                     &&
              axevent_set((HAXEVENT)pst_event, b_initial)    )
        {
            pst_event->b_manual = b_manual_reset;
        }
        else
            pst_event = (PAXEVENT)axevent_destroy((HAXEVENT)pst_event);
    }

    RETURN((HAXEVENT)pst_event);
}

// ***************************************************************************
// FUNCTION
//      axevent_wait
// PURPOSE
//      Wait for event
// PARAMETERS
//      HAXEVENT h_event   -- Handler of Event
//      UINT     d_timeout -- Timeout in msecs
// RESULT
//      BOOL  -- true if event happen or false if not
// ***************************************************************************
BOOL axevent_wait(HAXEVENT   h_event,
                  UINT       d_timeout)
{
    BOOL                b_result        = false;
    int                 i_result;
    BOOL                b_infin         = false;
    PAXEVENT           pst_event       = (PAXEVENT)h_event;
    int                 i_avail         = 0;
    struct  timeval     st_tv;
    fd_set              fds;
    div_t               secs;

    ENTER(pst_event);

// A7TRACE("  >> axevent_wait\n");

    if (!MAC_AVAIL(pst_event->ai_fd[0]))
    {
// ---------------------------- Waiting event --------------------------------

        if (d_timeout)
        {
            switch (d_timeout)
            {
                case -1:
                    b_infin = true;
                    break;
                default:
                    secs = div(d_timeout, 1000);
    //                A7TRACE("  secs.quot = %d %d\n", secs.quot, secs.rem * 1000);
                    st_tv.tv_sec  = secs.quot;
                    st_tv.tv_usec = secs.rem * 1000;
                    break;
            }

            FD_ZERO(&fds);
            FD_SET(pst_event->ai_fd[0], &fds);

            while ( ((i_result = select(pst_event->ai_fd[0] + 1,
                                    &fds, NULL, NULL, b_infin ? NULL :
                                    &st_tv)) == -1)                     &&
                       ((i_result == -1) && (errno == EINTR))           )
            {
                ;
            }

    //        sigemptyset(&st_set);
    //        sigaddset(&st_set, SIGINT);
    //        sigaddset(&st_set, SIGALRM);


    //        switch (pselect(pst_event->ai_fd[0] + 1, &fds, NULL, NULL, b_infin ? NULL : &st_tv, NULL))
    //        switch (pselect(pst_event->ai_fd[0] + 1, &fds, NULL, NULL, b_infin ? NULL : &st_tv, &st_set))
    //
            switch (i_result)
            {
                case -1:
                    // EINTR
                    perror("event ");
    //                A7TRACE("  ERROR -1\n");
                    break;

                case 0:
    //                A7TRACE("  ERROR 0\n");
                    break;

                default:
                    b_result    = true;
                    break;
            }


    /*
    //        switch (pselect(pst_event->ai_fd[0] + 1, &fds, NULL, NULL, b_infin ? NULL : &st_tv, NULL))
            switch (pselect(pst_event->ai_fd[0] + 1, &fds, NULL, NULL, b_infin ? NULL : &st_tv, &st_set))
            {
                case -1:
                    // EINTR
                    perror("event ");
    //                A7TRACE("  ERROR -1\n");
                    break;

                case 0:
    //                A7TRACE("  ERROR 0\n");
                    break;

                default:
                    b_result    = true;
                    break;
            }
    */
        }
    }
    else
        b_result = true;

// A7TRACE("  << axevent_wait = %s\n", MAC_TF(b_result));

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axevent_set
// PURPOSE
//      Set state of event
// PARAMETERS
//      HAXEVENT h_event    -- Handler of Event
//      BOOL      b_signaled -- State of event
// RESULT
//      BOOL  -- true if all is ok or false if error has occured
// ***************************************************************************
BOOL axevent_set(HAXEVENT h_event, BOOL b_signaled)
{
    BOOL                b_result        = false;
    int                 i_avail         = 0;
    PAXEVENT            pst_event       = (PAXEVENT)h_event;
    U8                  ab_data         [ PIPE_BUF ];

    ENTER(pst_event != NULL);

    if (pst_event->b_manual)
    {
        if (b_signaled)
        {
            if (!MAC_AVAIL(pst_event->ai_fd[0]))
            {
                // FOR VALGRIND  {
                memset(ab_data, 0, PIPE_BUF);
                // FOR VALGRIND  }

                if (write(pst_event->ai_fd[1], ab_data, 1) > 0)
                {
                    // ok
                }
            }
        }
        else
        {
            if (MAC_AVAIL(pst_event->ai_fd[0]))
            {
                if (read(pst_event->ai_fd[0], ab_data, PIPE_BUF) > 0)
                {
                    // ok
                }
            }
        }

        b_result    = true;
    }

    RETURN(b_result);
}
HAXSEM AXSEM_destroy(HAXSEM h_event)
{
    PA7EVENT           pst_event   = (PA7EVENT)h_event;

    ENTER(pst_event);

    if (pst_event->inited)
    {
        sem_destroy(&pst_event->sem);
    }

    FREE(pst_event);

    RETURN(NULL);
}
HAXSEM AXSEM_create(int         id,
                    BOOL        initial)
{
    PA7EVENT           event   = NULL;

    ENTER(true);

    if ((event = CREATE(A7EVENT)) != NULL)
    {
        event->pshared = id;

        switch (sem_init(&event->sem, id, initial ? 0 : 1))
        {
            case 0:
                event->inited = true;
                break;

            default:
                event = (PA7EVENT)AXSEM_destroy((HAXSEM)event);
                break;
        }
    }

    RETURN((HAXSEM)event);
}
BOOL AXSEM_wait(HAXSEM           h_event,
                  UINT               TO)
{
    BOOL                    result          = false;
//    PA7EVENT                event           = NULL;
//    const struct timespec   timeout;


    ENTER(true);

//    timeout.tv_sec  = 0;
//    timeout.tv_nsec = TO / 1000;
//
//    result = sem_timedwait(&event->sem, ) result

    RETURN(result);
}
BOOL AXSEM_get(HAXSEM           h_event)
{
    BOOL                    result          = false;
    PA7EVENT                event           = (PA7EVENT)h_event;


    ENTER(true);

    switch(sem_wait(&event->sem))
    {
        case 0:
            result = true;
            break;

        default:
            break;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXSEM_set
// PURPOSE
//
// PARAMETERS
//      HAXSEM h_event --
// RESULT
//      BOOL
//            TRUE:   Ok
//            false:  Error
// ***************************************************************************
BOOL AXSEM_set(HAXSEM           h_event)
{
    BOOL                    result          = false;
    PA7EVENT                event           = (PA7EVENT)h_event;

    ENTER(true);

    switch (sem_post(&event->sem))
    {
        case 0:
            result = true;
            break;

        default:
            break;
    }

    RETURN(result);
}

// ***************************************************************************
// TITLE
//      Events on Win32 Platform
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: events.c,v 1.4 2003/11/10 17:01:38 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************


#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include <arsenal.h>
#include <dreamix.h>


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------


// ***************************************************************************
// FUNCTION
//      la6_event_destroy
// PURPOSE
//      Destroy Event control structure
// PARAMETERS
//      HLA6EVENT h_event -- Handler of Event
// RESULT
//      HLA6EVENT -- Always NULL
// ***************************************************************************
HLA6EVENT la6_event_destroy(HLA6EVENT h_event)
{
    if (h_event)
        CloseHandle((HANDLE)h_event);

    return NULL;
}

// ***************************************************************************
// FUNCTION
//      la6_event_create
// PURPOSE
//      Create Event control structure
// PARAMETERS
//      BOOL      b_initial      -- Initial state of event
//      BOOL      b_manual_reset -- Make ability for manual event reset
// RESULT
//      HLA6EVENT  -- Handler of a new createtd Event control structure
//                    or NULL
// ***************************************************************************
HLA6EVENT la6_event_create(BOOL b_initial, BOOL b_manual_reset)
{
    return (HLA6EVENT)CreateEvent(NULL, b_manual_reset, b_initial, NULL);
}

// ***************************************************************************
// FUNCTION
//      la6_event_wait
// PURPOSE
//      Wait for event
// PARAMETERS
//      HLA6EVENT h_event    -- Handler of Event
//      DWORD     d_timeout  -- Timeout in msecs
// RESULT
//      BOOL  -- TRUE if event happen or FALSE if not
// ***************************************************************************
BOOL la6_event_wait(HLA6EVENT h_event, DWORD d_timeout)
{
    BOOL    b_result = FALSE;


    if (h_event)
    {
        b_result = (WaitForSingleObject(h_event,
                            d_timeout == DEFWAIT_INFINITE ?
                            INFINITE : d_timeout)) == WAIT_OBJECT_0;
    }

    return b_result;
}

// ***************************************************************************
// FUNCTION
//      la6_event_wait_multi
// PURPOSE
//
// PARAMETERS
//      HLA6EVENT   * ph_events     --
//      UINT          u_number        --
//      DWORD         d_timeout_msecs -- Waiting period, can be DEFWAIT_INFINITE
// RESULT
//      DWORD
//          DEFWAIT_ERROR:      Error has occured
//          DEFWAIT_TIMEOUT:    Waitinf timeout
//          Other:              Index of happen event
// ***************************************************************************
DWORD la6_event_wait_multi(HLA6EVENT *      ph_events,
                           UINT             u_number,
                           DWORD            d_timeout_msecs)
{
    DWORD       d_selected      = DEFWAIT_ERROR;
    DWORD       d_result;


    if (ph_events && u_number)
    {
        switch (d_result = WaitForMultipleObjects(u_number, ph_events, FALSE,
                    d_timeout_msecs == DEFWAIT_INFINITE ? INFINITE : d_timeout_msecs))
        {
            case WAIT_TIMEOUT:
                d_selected = DEFWAIT_TIMEOUT;
                break;

            default:
                if (d_result < WAIT_ABANDONED_0)
                    d_selected = d_result;
                break;
        }
    }

    return d_selected;
}

// ***************************************************************************
// FUNCTION
//      la6_event_set
// PURPOSE
//      Set state of event
// PARAMETERS
//      HLA6EVENT h_event    -- Handler of Event
//      BOOL      b_signaled -- State of event
// RESULT
//      BOOL  -- TRUE if all is ok or FALSE if error has occured
// ***************************************************************************
BOOL la6_event_set(HLA6EVENT h_event, BOOL b_signaled)
{
    BOOL    b_result = FALSE;


    if (h_event)
    {
        b_result = (b_signaled ? SetEvent(h_event) : ResetEvent(h_event));
    }

    return b_result;
}

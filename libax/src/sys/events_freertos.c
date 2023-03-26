// ***************************************************************************
// TITLE
//      Events for Standalone
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: events.c,v 1.2 2003/11/10 17:01:38 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************


#include <arsenal.h>
#include <axsystem.h>

#include <FreeRTOS.h>
#include <task.h>

// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

BOOL axevent_wait(HAXEVENT   h_event,
                  UINT       d_timeout)
{
return false;
}

BOOL axevent_set(HAXEVENT h_event, BOOL b_signaled)
{
return false;
}

HAXEVENT axevent_destroy(HAXEVENT h_event)
{
//    PAXEVENT           pst_event   = (PAXEVENT)h_event;

    ENTER(h_event);

    FREE(h_event);
    
    RETURN(NULL);
}
HAXEVENT axevent_create(BOOL b_initial, BOOL b_manual_reset)
{
//    TaskHandle_t         result      = nil;

//    ENTER(true);

//    if ((result = CREATE(TaskHandle_t)) != nil)
//    {
//
//    }

//    RETURN((HAXEVENT)result);

    return nil;
}


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
#include <stdlib.h>
#include <string.h>
#include <a7time.h>
#include <a7string.h>
#include <stdarg.h>
#include <string.h>
#include <dreamix.h>

#include <a7string.h>
#include <a7threads.h>

//static PBYTE        globalPtr = (PBYTE)100000;
static HA7MUTEX     LCK;

// ***************************************************************************
// FUNCTION
//      function2
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL function2()
{
    BOOL            result          = FALSE;
    PBYTE           on;

    ENTER(TRUE);

    a7mutex_lock(LCK, true);

//    if (globalPtr)
//    {
//        globalPtr--;
//    }
//    else
//        *globalPtr = 0;


    A7TRACE((A7P, "about to alloc"));

    if ((on = (PBYTE)MALLOC(100)) != nil)
    {
        A7TRACE((A7P, "about to override"));
        memset(on, 0xcc, 102);
        FREE(on);
    }

    a7mutex_unlock(LCK);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      function1
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL function1()
{
    BOOL            result          = FALSE;

    ENTER(TRUE);

    result = function2();

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      threadProc1
// PURPOSE
//
// PARAMETERS
//      HA7THREADX h_ctrl --
//      PVOID      param  --
// RESULT
//      int  --
// ***************************************************************************
int  threadProc1(HA7THREADX     h_ctrl,
                 PVOID          param)
{
    A7SIG           v_sig;
    UINT            u_param;

    TX_ENTER(h_ctrl, param);

    while (!MAC_THREADX_WAIT_QUIT(v_sig, u_param, 1))
    {
        function1();
    }

    TX_EXIT(h_ctrl, 0);
}
// ***************************************************************************
// FUNCTION
//      threadProc1
// PURPOSE
//
// PARAMETERS
//      HA7THREADX h_ctrl --
//      PVOID      param  --
// RESULT
//      int  --
// ***************************************************************************
int  threadProc2(HA7THREADX     h_ctrl,
                 PVOID          param)
{
    A7SIG           v_sig;
    UINT            u_param;

    TX_ENTER(h_ctrl, param);

    while (!MAC_THREADX_WAIT_QUIT(v_sig, u_param, 1))
    {
        function1();
    }

    TX_EXIT(h_ctrl, 0);
}
// ***************************************************************************
// FUNCTION
//      main
// PURPOSE
//
// PARAMETERS
//      int      argc   --
//      char *   argv[] --
// RESULT
//      int  --
// ***************************************************************************
int main(int        argc,
         char *     argv[])
{
    int             result      = 0;

    MAIN_ENTER(TRUE);

    LCK = a7mutex_create();

//    a7threadX_create(0, (PFNA7THREADX)threadProc1, &result, -1);
//    a7threadX_create(0, (PFNA7THREADX)threadProc2, &result, -1);

    printf("main stage '%s'\n", __A7PLACE__);

    while (true)
    {
        function1();

        a7sleep(1000);

        A7TRACE((A7P, "trace message"));
    }

    MAIN_RETURN(result);
}

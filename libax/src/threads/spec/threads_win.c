// ***************************************************************************
// TITLE
//      Threads on Win32 platform
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: threads.c,v 1.3 2003/08/06 12:25:58 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************

#include <windows.h>

#include <arsenal.h>
#include <a7threads.h>
#include <pvt_threads.h>


#if (TARGET_SYSTEM != __AX_wince__)
#include <process.h>
#endif                                      //  #if (TARGET_SYSTEM...


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

//extern int _endthreadex(int);
//extern int _beginthreadex(int);

// ***************************************************************************
// FUNCTION
//      a7thread_exit
// PURPOSE
//      Exit thread
// PARAMETERS
//      DWORD d_retcode -- Return code value
// RESULT
//      none
// ***************************************************************************
void a7thread_exit(UINT u_retcode)
{
#if (TARGET_SYSTEM == __AX_wince__)
    ExitThread(0);
#else
    _endthreadex(0);
#endif                                      //  #if (TARGET_SYSTEM...
}
// ***************************************************************************
// FUNCTION
//      a7thread_destroy
// PURPOSE
//
// PARAMETERS
//      HA7THREAD h_thread --
// RESULT
//      HA7THREAD  --
// ***************************************************************************
HA7THREAD a7thread_destroy(HA7THREAD  h_thread)
{
    ENTER(h_thread);

    CloseHandle(h_thread);

    RETURN(NULL);
}
// ***************************************************************************
// FUNCTION
//      a7thread_create
// PURPOSE
//      Create thread
// PARAMETERS
//      HA7THREAD   * ph_thread -- Pointer to buffer for thread handle
//      DWORD          d_flags   -- Creation flags
//      PFNA7THREAD   pf_entry  -- Pointer to thread function
//      PVOID          p_param   -- User's parameter for thread function
// RESULT
//      BOOL
//          true:  Thread created
//          false: An error has occured
// ***************************************************************************
BOOL a7thread_create(HA7THREAD *    ph_thread,
                     UINT           u_flags,
                     PFNA7THREAD    pf_entry,
                     PVOID          p_param)
{
    BOOL            b_result        = false;
    HANDLE          h_thread;

    ENTER(true);

    if (pf_entry)
    {

#if (TARGET_SYSTEM == __AX_wince__)
        if ((h_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)pf_entry, p_param, 0, &d_tid)) != NULL)
#else
        if ((h_thread = (HANDLE)_beginthreadex(NULL, 0, (unsigned (__stdcall*)(void*))pf_entry, p_param, 0, NULL)) != NULL)
#endif                                      //  #if (TARGET_SYSTEM...
        {
//            A7TRACE((A7P, "  create h_thread = %p\n", h_thread));

            if (ph_thread)
                *ph_thread = (HA7THREAD)h_thread;
            else
                CloseHandle(h_thread);

            b_result = true;
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      a7thread_priority_set
// PURPOSE
//
// PARAMETERS
//      HA7THREAD h_thread   --
//      UINT       i_priority --
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
BOOL a7thread_priority_set(HA7THREAD        h_thread,
                           INT              i_priority)
{
#define MAC_MAP(a,b)    case a: nPriority = b; break;
#define DEF_WRONG_VAL   -100

    BOOL            b_result    = false;
    int             nPriority   = DEF_WRONG_VAL;

    ENTER(h_thread);

    switch (i_priority)
    {
        MAC_MAP(-2, THREAD_PRIORITY_LOWEST);
        MAC_MAP(-1, THREAD_PRIORITY_BELOW_NORMAL);
        MAC_MAP(0,  THREAD_PRIORITY_NORMAL);
        MAC_MAP(1,  THREAD_PRIORITY_ABOVE_NORMAL);
        MAC_MAP(2,  THREAD_PRIORITY_HIGHEST);
        MAC_MAP(3,  THREAD_PRIORITY_TIME_CRITICAL);

        default:
            break;
    }

    if (nPriority != DEF_WRONG_VAL)
    {
        b_result = SetThreadPriority(h_thread, nPriority);
    }
    else
        A7TRACE((A7P, "  priority %d NOT set for %p\n", i_priority, h_thread));


    RETURN(b_result);

#undef  DEF_WRONG_VAL
#undef  MAC_MAP
}
// ***************************************************************************
// FUNCTION
//      a7thread_get_id
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      UINT --
// ***************************************************************************
UINT a7thread_get_id()
{
    return GetCurrentThreadId();
}

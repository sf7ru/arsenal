// ***************************************************************************
// TITLE
//      Processes Win32 Platform
// PROJECT
//      Arsenal Library
// ***************************************************************************
//
// FILE
//      $Id: process.c,v 1.6 2003/12/10 12:57:39 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// HISTORY
//      $Log: process.c,v $
//      Revision 1.6  2003/12/10 12:57:39  A.Kozhukhar
//      *** empty log message ***
//
//      Revision 1.5  2003/12/01 08:26:03  A.Kozhukhar
//      *** empty log message ***
//
//      Revision 1.4  2003/11/10 17:01:38  A.Kozhukhar
//      *** empty log message ***
//
//      Revision 1.3  2003/08/06 12:25:58  A.Kozhukhar
//      *** empty log message ***
//
//      Revision 1.2  2003/04/24 10:07:28  A.Kozhukhar
//      *** empty log message ***
//
//      Revision 1.1.1.1  2003/02/14 13:17:33  A.Kozhukhar
//      Initial release
//
// ***************************************************************************


#include <stdlib.h>
#include <windows.h>

#include <arsenal.h>

#include <axsystem.h>

#include <axstring.h>

#include <pvt_win.h>



// ---------------------------------------------------------------------------
// ----------------------------- LOCAL TYPES ---------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// STRUCTURE
//      AXPROCESS
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_AXPROCESS
{
    PAXPIPE             pst_pipe_in;
    PAXPIPE             pst_pipe_out;
    PAXPIPE             pst_pipe_err;
    DWORD               d_exit_code;
    PROCESS_INFORMATION st_pi;
} AXPROCESS, * PAXPROCESS;


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------


// ***************************************************************************
// FUNCTION
//      axprocess_id_get
// PURPOSE
//      Get ID of process
// PARAMETERS
//      HAXPROCESS h_process -- Handler of process
// RESULT
//      DWORD   -- Process ID
// ***************************************************************************
UINT axprocess_get_id(HAXPROCESS h_process)
{
    PAXPROCESS     pst_process     = (PAXPROCESS)h_process;
    DWORD           d_pid           = AXII;

    if (pst_process)
    {
        d_pid = pst_process->st_pi.dwProcessId;
    }
    else
        d_pid = GetCurrentProcessId();

    return d_pid;
}

// ***************************************************************************
// FUNCTION
//      axprocess_destroy
// PURPOSE
//      Destroy process
// PARAMETERS
//      HAXPROCESS h_process -- Handler of process
// RESULT
//      HAXPROCESS -- Always NULL
// ***************************************************************************
HAXPROCESS axprocess_destroy(HAXPROCESS h_process)
{
    PAXPROCESS             pst_process     = (PAXPROCESS)h_process;
    HAXPIPE                pst_pipe        = NULL;

    if (pst_process)
    {
        if (pst_process->pst_pipe_in)
            axpipe_destroy((HAXPIPE)pst_process->pst_pipe_in);

        if (pst_process->pst_pipe_out)
            axpipe_destroy((HAXPIPE)pst_process->pst_pipe_out);

        if (pst_process->st_pi.hThread != INVALID_HANDLE_VALUE)
            CloseHandle(pst_process->st_pi.hThread);

        if (pst_process->st_pi.hProcess != INVALID_HANDLE_VALUE)
            CloseHandle(pst_process->st_pi.hProcess);

        FREE(pst_process);
    }

    return NULL;
}

// ***************************************************************************
// FUNCTION
//      axprocess_get
// PURPOSE
//      Get process by Process ID
// PARAMETERS
//      DWORD       d_pid -- Process ID
// RESULT
//      HAXPROCESS -- Handler of Process
// ***************************************************************************
HAXPROCESS axprocess_get(DWORD d_pid)
{
    PAXPROCESS     pst_process     = NULL;


    if ((pst_process = CREATE(AXPROCESS)) != NULL)
    {
        if (d_pid == AXII)
        {
            pst_process->st_pi.dwProcessId  = GetCurrentProcessId();
            pst_process->st_pi.dwThreadId   = GetCurrentThreadId();
        }
        else
        {
            pst_process->st_pi.dwProcessId  = d_pid;
            pst_process->st_pi.dwThreadId   = (DWORD)(INVALID_HANDLE_VALUE);
        }

        if ((pst_process->st_pi.hProcess = OpenProcess(PROCESS_ALL_ACCESS,
                TRUE, pst_process->st_pi.dwProcessId)) != INVALID_HANDLE_VALUE)
        {
            pst_process->st_pi.hThread = INVALID_HANDLE_VALUE;
        }
        else
            pst_process = (PAXPROCESS)axprocess_destroy((HAXPROCESS)pst_process);
    }

    return (HAXPROCESS)pst_process;
}

// ***************************************************************************
// FUNCTION
//      axprocess_create
// PURPOSE
//      Create process and process
// PARAMETERS
//      PSTR        psz_filename -- Handler of name and path of binary file
//      PSTR        psz_cmdline  -- Command line parameters
//      DWORD       d_flags      -- Creation flags
//                  AXPSA_STDPIPES:        Catch standard IN/OUT/ERR pipes
//                  AXPSA_HIDEN:           Hide child process
//                  AXPSA_DETACH:          Detach child process
// RESULT
//      HAXPROCESS -- Handler of the just created process
// ***************************************************************************
HAXPROCESS axprocess_create(PSTR psz_filename, PSTR psz_cmdline, UINT d_flags)
{
    PAXPROCESS         pst_process         = NULL;
    PSTR                psz_my_cmdline      = NULL;
    UINT                u_len;
    STARTUPINFO         st_si;


    if (psz_filename)
    {
        if ((pst_process = CREATE(AXPROCESS)) != NULL)
        {
// ---- The F-word Win32 requires a space symbol at start of command line --------

            if (psz_cmdline && ((u_len = (UINT)strlen(psz_cmdline)) != 0))
            {
                u_len   +=  sizeof(CHAR) << 1;

                if ((psz_my_cmdline = MALLOC(u_len)) != NULL)
                {
                    *psz_my_cmdline = ' ';
                    strz_cpy(psz_my_cmdline + sizeof(CHAR),
                                    psz_cmdline, u_len - sizeof(CHAR));
                }
                else
                    pst_process = (PAXPROCESS)axprocess_destroy((HAXPROCESS)pst_process);
            }

// --------------------------- Creating pipes --------------------------------

            if (pst_process && (d_flags & AXPSA_STDPIPES))
            {
                if ( !( ((pst_process->pst_pipe_in  =
                                    (PAXPIPE)axpipe_create(0)) != NULL)    &&
                        ((pst_process->pst_pipe_out =
                                    (PAXPIPE)axpipe_create(0)) != NULL)    &&
                        ((pst_process->pst_pipe_err =
                                    (PAXPIPE)axpipe_create(0)) != NULL)    ))
                {
                    pst_process = (PAXPROCESS)axprocess_destroy((HAXPROCESS)pst_process);
                }
            }

// -------------------------- Creating process -------------------------------

            if (pst_process)
            {
                memset(&st_si, 0, sizeof(st_si));
                st_si.cb          = sizeof(st_si);

                st_si.dwFlags     = STARTF_USESTDHANDLES |
                                    ((d_flags & AXPSA_HIDEN) ? STARTF_USESHOWWINDOW : 0);
                st_si.wShowWindow = ((d_flags & AXPSA_HIDEN) ? SW_HIDE : SW_SHOWNORMAL);

                st_si.hStdInput   = (pst_process->pst_pipe_in  ?
                        pst_process->pst_pipe_in->h_in   : GetStdHandle(STD_INPUT_HANDLE));
                st_si.hStdOutput  = (pst_process->pst_pipe_out ?
                        pst_process->pst_pipe_out->h_out : GetStdHandle(STD_OUTPUT_HANDLE));
                st_si.hStdError   = (pst_process->pst_pipe_err ?
                        pst_process->pst_pipe_err->h_out : GetStdHandle(STD_ERROR_HANDLE));

                if ( !CreateProcess(psz_filename, psz_my_cmdline,
                                    NULL, NULL, !(d_flags & AXPSA_DETACH), 0, NULL, NULL,
                                    &st_si, &pst_process->st_pi))
                {
                    pst_process = (PAXPROCESS)axprocess_destroy((HAXPROCESS)pst_process);
                }
            }

            if (psz_my_cmdline)
                FREE(psz_my_cmdline);
        }
    }

    return (HAXPROCESS)pst_process;
}

// ***************************************************************************
// FUNCTION
//      axprocess_get_exit_code
// PURPOSE
//      Retrieve Process Exit Code
// PARAMETERS
//      HAXPROCESS h_process  -- Handler of process
//      PDWORD      pd_result    -- Handler of DWORD for Exit Code value
// RESULT
//      BOOL
//          TRUE:  if Process stopped and Exit Code available
//          FALSE: Process still running or Exit Code unavailable
// ***************************************************************************
BOOL axprocess_get_exit_code(HAXPROCESS h_process, 
                             PUINT      pd_result)
{
    PAXPROCESS             pst_process     = (PAXPROCESS)h_process;
    BOOL                    b_result        = FALSE;
    DWORD                   d_result;


    if (pst_process)
    {
        if (  GetExitCodeProcess(pst_process->st_pi.hProcess, &d_result)    &&
              (d_result != STILL_ACTIVE)                                    )
        {
            b_result        = TRUE;

            if (pd_result)
                *pd_result  = d_result;
        }
    }

    return b_result;
}

// ***************************************************************************
// FUNCTION
//      axprocess_wait
// PURPOSE
//      Wait for process
// PARAMETERS
//      HAXPROCESS h_process  -- Handler of process
//      DWORD       d_timeout    -- Timeout for operation in msecs
// RESULT
//      AXTALKCODE -- Operation result
//          AXTC_ok:  All is ok
//          Other:     Error code value
// ***************************************************************************
int axprocess_wait(HAXPROCESS    h_process,
                     UINT           u_TO)
{
    int                     i_result        = -1;
    PAXPROCESS             pst_process     = (PAXPROCESS)h_process;
//    AXTALKCODE             v_retcode       = AXTC_fatal;
    DWORD                   d_result;

    ENTER(pst_process);

    d_result = WaitForSingleObject(pst_process->st_pi.hProcess,
                    (u_TO != DEFWAIT_INFINITE ? u_TO : INFINITE));

    switch (d_result)
    {
        case WAIT_OBJECT_0:     i_result = 1;       break;
        case WAIT_TIMEOUT:      i_result = 0;       break;
//            case WAIT_ABANDONED:    i_result = AXTC_access;    break;
    }

    RETURN(i_result);
}
// ***************************************************************************
// FUNCTION
//      axprocess_kill
// PURPOSE
//      Kill Process
// PARAMETERS
//      HAXPROCESS h_process -- Handler of process
//      DWORD       d_exit_code -- Force Exit Code
// RESULT
//      BOOL
//          TRUE:  Process killed
//          FALSE: An error has occured
// ***************************************************************************
BOOL axprocess_kill(HAXPROCESS h_process, UINT d_exit_code)
{
    PAXPROCESS             pst_process     = (PAXPROCESS)h_process;
    BOOL                    b_result        = FALSE;


    if (pst_process)
        b_result = TerminateProcess(pst_process->st_pi.hProcess, d_exit_code);

    return b_result;
}

// ***************************************************************************
// FUNCTION
//      axprocess_is_alive
// PURPOSE
//      Check whether a process is alive
// PARAMETERS
 //      HAXPROCESS h_process -- Handler of process
// RESULT
//      BOOL
//          TRUE:  All is ok
//          FALSE: An error has occured
// ***************************************************************************
BOOL axprocess_is_alive(HAXPROCESS h_process)
{
    PAXPROCESS             pst_process     = (PAXPROCESS)h_process;
    BOOL                    b_result        = FALSE;
    DWORD                   d_result;


    if (pst_process)
    {
        b_result =  (GetExitCodeProcess(pst_process->st_pi.hProcess, &d_result) &&
                    (d_result == STILL_ACTIVE));
    }

    return b_result;
}
BOOL axprocess_is_alive2(UINT pid)
{
    BOOL                    b_result        = FALSE;
    DWORD                   d_result;

    b_result =  (GetExitCodeProcess(pid, &d_result) && (d_result == STILL_ACTIVE));

    return b_result;
}
// ***************************************************************************
// FUNCTION
//      axprocess_stdin
// PURPOSE
//
// PARAMETERS
//      HAXPROCESS h_process --
// RESULT
//      HAXPIPE    --
// ***************************************************************************
HAXPIPE axprocess_stdin(HAXPROCESS h_process)
{
    PAXPIPE            pst_pipe            = NULL;
    PAXPROCESS         pst_process         = (PAXPROCESS)h_process;


    if (  pst_process                                               &&
          pst_process->pst_pipe_in                                  &&
          (pst_process->pst_pipe_in->h_out != INVALID_HANDLE_VALUE)  )
    {
        if ((pst_pipe = CREATE(AXPIPE)) != NULL)
        {
            pst_pipe->h_in  = INVALID_HANDLE_VALUE;
            pst_pipe->h_out = INVALID_HANDLE_VALUE;

            if (!DuplicateHandle(
                    GetCurrentProcess(), pst_process->pst_pipe_in->h_out,
                    GetCurrentProcess(), (LPHANDLE)&pst_pipe->h_out,
                    0, FALSE, DUPLICATE_SAME_ACCESS))
            {
                pst_pipe = (PAXPIPE)axpipe_destroy((HAXPIPE)pst_pipe);
            }
        }
    }

    return (HAXPIPE)pst_pipe;
}

// ***************************************************************************
// FUNCTION
//      axprocess_stdout
// PURPOSE
//
// PARAMETERS
//      HAXPROCESS h_process --
// RESULT
//      HAXPIPE    --
// ***************************************************************************
HAXPIPE axprocess_stdout(HAXPROCESS h_process)
{
    PAXPIPE            pst_pipe            = NULL;
    PAXPROCESS         pst_process         = (PAXPROCESS)h_process;


    if (  pst_process                                                   &&
          pst_process->pst_pipe_out                                     &&
          (pst_process->pst_pipe_out->h_in != INVALID_HANDLE_VALUE)     )
    {
        if ((pst_pipe = CREATE(AXPIPE)) != NULL)
        {
            pst_pipe->h_in  = INVALID_HANDLE_VALUE;
            pst_pipe->h_out = INVALID_HANDLE_VALUE;

            if (!DuplicateHandle(
                    GetCurrentProcess(), pst_process->pst_pipe_out->h_in,
                    GetCurrentProcess(), (LPHANDLE)&pst_pipe->h_in,
                    0, FALSE, DUPLICATE_SAME_ACCESS))
            {
                pst_pipe = (PAXPIPE)axpipe_destroy((HAXPIPE)pst_pipe);
            }
        }
    }

    return (HAXPIPE)pst_pipe;
}

// ***************************************************************************
// FUNCTION
//      axprocess_stderr
// PURPOSE
//
// PARAMETERS
//      HAXPROCESS h_process --
// RESULT
//      HAXPIPE    --
// ***************************************************************************
HAXPIPE axprocess_stderr(HAXPROCESS h_process)
{
    PAXPIPE            pst_pipe            = NULL;
    PAXPROCESS         pst_process         = (PAXPROCESS)h_process;


    if (  pst_process                                                   &&
          pst_process->pst_pipe_err                                     &&
          (pst_process->pst_pipe_err->h_in != INVALID_HANDLE_VALUE)     )
    {
        if ((pst_pipe = CREATE(AXPIPE)) != NULL)
        {
            pst_pipe->h_in  = INVALID_HANDLE_VALUE;
            pst_pipe->h_out = INVALID_HANDLE_VALUE;

            if (!DuplicateHandle(
                    GetCurrentProcess(), pst_process->pst_pipe_err->h_in,
                    GetCurrentProcess(), (LPHANDLE)&pst_pipe->h_in,
                    0, FALSE, DUPLICATE_SAME_ACCESS))
            {
                pst_pipe = (HAXPIPE)axpipe_destroy(pst_pipe);
            }
        }
    }

    return (HAXPIPE)pst_pipe;
}
// ***************************************************************************
// FUNCTION
//      axprocess_hup
// PURPOSE
//
// PARAMETERS
//      HAXPROCESS h_process --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL axprocess_hup(HAXPROCESS h_process)
{
    PAXPROCESS         pst_process         = (PAXPROCESS)h_process;
    BOOL                b_result            = FALSE;

    return b_result;
}

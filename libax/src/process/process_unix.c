// ***************************************************************************
// TITLE
//      Processes UNIX Platform
// PROJECT
//      Arsenal Library
// ***************************************************************************
//
// FILE
//      $Id: process.c,v 1.3 2003/12/01 08:26:03 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// HISTORY
//      $Log: process.c,v $
//      Revision 1.3  2003/12/01 08:26:03  A.Kozhukhar
//      *** empty log message ***
//
//      Revision 1.2  2003/04/24 10:07:28  A.Kozhukhar
//      *** empty log message ***
//
//      Revision 1.1  2003/02/28 10:35:30  A.Kozhukhar
//      Initial release
//
// ***************************************************************************

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <utmp.h>
#include <errno.h>

#include <arsenal.h>

#if (TARGET_SYSTEM == __AX_linux__)
#include <pty.h>
#endif

#include <records.h>

#include <axstring.h>
#include <pvt_unix.h>
#include <termio.h>
#include <pty.h>

#define F_SETPIPE_SZ  1031  //  Linux 2.6.35+
#define F_GETPIPE_SZ  1032  //# Linux 2.6.35+


// ---------------------------------------------------------------------------
// -------------------------- LOCAL DEFINITIONS ------------------------------
// ---------------------------------------------------------------------------

#define DEF_MAX_ARG_LEN         1024



// ---------------------------------------------------------------------------
// ----------------------------- LOCAL TYPES ---------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// STRUCTURE
//      PROCESS
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_PROCESS
{
    UINT                u_pid;

    int                 i_pty_master;
    int                 i_pty_slave;

    int                 ai_stdin        [2];
    int                 ai_stdout       [2];
    int                 ai_stderr       [2];
} PROCESS, * PPROCESS;



// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------


// ***************************************************************************
// FUNCTION
//      la6_process_id_get
// PURPOSE
//      Get ID of process
// PARAMETERS
//      HAXPROCESS h_process -- Handler of Process
// RESULT
//      DWORD   -- Process ID
// ***************************************************************************
UINT axprocess_get_id(HAXPROCESS h_process)
{
    UINT            d_pid           = AXII;
    PPROCESS        pst_process     = (PPROCESS)h_process;

    if (pst_process)
    {
        d_pid = pst_process->u_pid;
    }
    else
        d_pid = getpid();

    return d_pid;
}
// ***************************************************************************
// FUNCTION
//      la6_process_destroy
// PURPOSE
//      Destroy Process control structure
// PARAMETERS
//      HAXPROCESS h_process -- Handler of Process
// RESULT
//      HAXPROCESS -- Always NULL
// ***************************************************************************
HAXPROCESS axprocess_destroy(HAXPROCESS h_process)
{
    PPROCESS             pst_process     = (PPROCESS)h_process;

    if (pst_process)
    {
        if (pst_process->i_pty_master != -1)
            close(pst_process->i_pty_master);

        if (pst_process->i_pty_slave != -1)
            close(pst_process->i_pty_slave);

        if (pst_process->ai_stdin[0] != -1)
            close(pst_process->ai_stdin[0]);

        if (pst_process->ai_stdin[1] != -1)
            close(pst_process->ai_stdin[1]);

        if (pst_process->ai_stdout[0] != -1)
            close(pst_process->ai_stdout[0]);

        if (pst_process->ai_stdout[1] != -1)
            close(pst_process->ai_stdout[1]);

        if (pst_process->ai_stderr[0] != -1)
            close(pst_process->ai_stderr[0]);

        if (pst_process->ai_stderr[1] != -1)
            close(pst_process->ai_stderr[1]);

        FREE(pst_process);
    }

    return NULL;
}
// ***************************************************************************
// FUNCTION
//      la6_process_get
// PURPOSE
//      Get Process control structure by Process ID
// PARAMETERS
//      DWORD       d_pid -- Process ID
// RESULT
//      HAXPROCESS -- Pointer to the just created Process structure
// ***************************************************************************
HAXPROCESS axprocess_get(UINT d_pid)
{
    HAXPROCESS     pst_process     = NULL;

    //FIXME("implement");

    return pst_process;
}
// ***************************************************************************
// STATIC FUNCTION
//      _argv_destroy
// PURPOSE
//
// PARAMETERS
//      LA6RECARRAY pst_argv --
// RESULT
//      LA6RECARRAY --
// ***************************************************************************
static HRECARRAY _argv_destroy(HRECARRAY pst_argv)
{
    UINT        u_cnt;
    PSTR *      ppsz_rec;

    if (pst_argv)
    {
        for (   u_cnt = 0;
                (ppsz_rec = recarray_get(pst_argv, u_cnt)) != NULL;
                u_cnt++)
        {
            if (*ppsz_rec)
                FREE(*ppsz_rec);
        }

        recarray_destroy(pst_argv);
    }

    return NULL;
}
// ***************************************************************************
// STATIC FUNCTION
//      _cmdline_to_argv
// PURPOSE
//
// PARAMETERS
//      PSTR  psz_filename --
//      PSTR  psz_cmdline  --
// RESULT
//      PVOID   --
// ***************************************************************************
static PVOID _cmdline_to_argv(PSTR psz_filename, PSTR psz_cmdline)
{
    HRECARRAY           pst_argv            = NULL;
    PSTR                psz_rec;
    PSTR                psz_on_cmdline;


    if (psz_cmdline)
    {
        if ((pst_argv = recarray_create(sizeof(PSTR), 3, 3, nil)) != NULL)
        {
// -------------------------------- argv0 ------------------------------------

            if (  ((psz_rec = strz_dup(psz_filename))     == NULL)            ||
                  (recarray_add(pst_argv, &psz_rec) == AXII)  )
            {
                if (psz_rec)
                    FREE(psz_rec);

                pst_argv = _argv_destroy(pst_argv);
            }

// ------------------------------ argv1 ... ----------------------------------

            psz_on_cmdline  = psz_cmdline;

            while (pst_argv)
            {
                if ((psz_rec = MALLOC(DEF_MAX_ARG_LEN)) != NULL)
                {
                    if ((psz_on_cmdline = strz_substring_get_std(psz_rec,
                            psz_on_cmdline, DEF_MAX_ARG_LEN)) != NULL)
                    {
                        if (*psz_rec)
                        {
                            strz_esc_expand(psz_rec);

                            psz_rec = REALLOC(psz_rec, strlen(psz_rec) + sizeof(CHAR));

                            if (recarray_add(pst_argv,
                                             &psz_rec) == AXII)
                            {
                                pst_argv = _argv_destroy(pst_argv);
                                FREE(psz_rec);
                            }
                        }
                        else
                            FREE(psz_rec);
                    }
                    else
                    {
                        FREE(psz_rec);
                        break;
                    }
                }
                else
                    pst_argv = _argv_destroy(pst_argv);
            }

// --------------------------- argv'n' = NULL --------------------------------

            psz_rec     = NULL;

            if ((recarray_add(pst_argv, &psz_rec)) == AXII)
            {
                pst_argv = _argv_destroy(pst_argv);
            }
        }
    }

    return pst_argv ? recarray_freeze(pst_argv, NULL) : NULL;
}
#define A7OPT_USEOPENPTY

#if (defined(A7OPT_USEOPENPTY))
// ***************************************************************************
// FUNCTION
//      _la6_process_tune_pty
// PURPOSE
//
// PARAMETERS
//      int  i_pty --
// RESULT
//      BOOL
//          TRUE:  All is ok
//          FALSE: An error has occured
// ***************************************************************************
static BOOL _la6_process_tune_pty(int i_pty)
{
    BOOL                b_result        = false;
    struct termios      st_tios;

    ENTER(i_pty != -1);

    if (!tcgetattr(i_pty, &st_tios))
    {
        cfmakeraw(&st_tios);


        b_result = !tcsetattr(i_pty, TCSANOW, &st_tios);
    }

    RETURN(b_result);
}
#endif
// ***************************************************************************
// FUNCTION
//      axprocess_create
// PURPOSE
//      Create process and Process control structure
// PARAMETERS
//      PSTR filename -- Pointer to name and path of executable
//      PSTR cmdline  -- Command line parameters
//      UINT flags    -- Creation flags
//                  AXPSA_STDPIPES:        Catch standard IN/OUT/ERR pipes
//                  AXPSA_HIDEN:           Hide child process
//                  AXPSA_DETACH:          Detach child process
// RESULT
//      HAXPROCESS -- Pointer to the just created Process control structure
// ***************************************************************************
HAXPROCESS axprocess_create(PSTR     filename,
                            PSTR     cmdline,
                            UINT     flags)
{
    PPROCESS            pst_process     = NULL;
    PVOID               p_argv;
    int                 si;
    int                 so;
    int                 se;

    if (filename)
    {
        if ((pst_process = CREATE(PROCESS)) != NULL)
        {
            pst_process->i_pty_master   = -1;
            pst_process->i_pty_slave    = -1;

            pst_process->ai_stdin[0]    = -1;
            pst_process->ai_stdin[1]    = -1;
            pst_process->ai_stdout[0]   = -1;
            pst_process->ai_stdout[1]   = -1;
            pst_process->ai_stderr[0]   = -1;
            pst_process->ai_stderr[1]   = -1;

#if (defined(A7OPT_USEOPENPTY))
            if (flags & AXPSA_DETACH)
            {
// ---------------------------- Creating PTY ---------------------------------

                //A7TRACE((A7P, "  Warning: using 'openpty'!\n"));

                if ( !( !openpty(&pst_process->i_pty_master,
                                &pst_process->i_pty_slave, NULL, NULL, NULL)    &&
                        _la6_process_tune_pty(pst_process->i_pty_master)        &&
                        _la6_process_tune_pty(pst_process->i_pty_slave)         ))
                {
                    pst_process = (PPROCESS)axprocess_destroy((HAXPROCESS)pst_process);
                }
            }
#endif

            if (  pst_process                       &&
                  (flags & AXPSA_STDPIPES)        &&
                  (-1 == pst_process->i_pty_master) )
            {
// --------------------------- Creating pipes --------------------------------
                if( !( (pipe(pst_process->ai_stdin)     == 0)   &&
                       (pipe(pst_process->ai_stdout)    == 0)   &&
                       (pipe(pst_process->ai_stderr)    == 0)   ))
                {
                    pst_process = (PPROCESS)axprocess_destroy((HAXPROCESS)pst_process);
                }
            }

            if (pst_process)
            {
                switch (pst_process->u_pid = fork())
                {
                    case -1:
                        pst_process = (PPROCESS)axprocess_destroy((HAXPROCESS)pst_process);
                        break;

                    case 0:
                        if ((p_argv = _cmdline_to_argv(filename, cmdline)) != NULL)
                        {
                            if (flags & AXPSA_STDPIPES)
                            {
// ---------------------- Closing standatd streams ---------------------------

                                close(STDIN_FILENO);
                                close(STDOUT_FILENO);
                                close(STDERR_FILENO);

                                // flags & LA6PSA_DETACH
                                if (-1 != pst_process->i_pty_slave)
                                {
// ---------------- Creating new standard streams on PTY ---------------------

                                    si = dup(pst_process->i_pty_slave);
                                    so = dup(pst_process->i_pty_slave);
                                    se = dup(pst_process->i_pty_slave);
                                }
                                else
                                {
// --------------- Creating new standard streams on pipes --------------------

                                    si = dup(pst_process->ai_stdin[0]);
                                    so = dup(pst_process->ai_stdout[1]);
                                    se = dup(pst_process->ai_stderr[1]);
                                }

                                si = so;
                                so = se;
                                se = si;
                            }

                            if (flags & AXPSA_DETACH)
                            {
// ------------------------------ Detaching ----------------------------------

                                setsid();
                                // Valgrind does not happy with following string
#ifdef TIOCSCTTY
                                ioctl(0, TIOCSCTTY);
#endif
                                signal(SIGHUP, SIG_IGN);
                            }
                            else
                            {
                            }

// -------------------------- Destroying legacy ------------------------------

                            axprocess_destroy((HAXPROCESS)pst_process);

// -------------------------- Executing process ------------------------------

                            execv(filename, p_argv);
                        }

                        _exit(-1);
                        break;

                    default:
                        break;
                }
            }
        }
    }

    return (HAXPROCESS)pst_process;
}

// ***************************************************************************
// FUNCTION
//      la6_process_get_exit_code
// PURPOSE
//      Retrieve Process Exit Code
// PARAMETERS
//      HAXPROCESS h_process  -- Handler of Process
//      PDWORD      pd_result    -- Pointer to DWORD for Exit Code value
// RESULT
//      BOOL
//          TRUE:  if Process stopped and Exit Code available
//          FALSE: Process still running or Exit Code unavailable
// ***************************************************************************
BOOL axprocess_get_exit_code(HAXPROCESS h_process,
                             PUINT      pd_result)
{
    PPROCESS            pst_process         = (PPROCESS)h_process;
    BOOL                b_result            = false;
    int                 i_res;
    int                 i_tries             = 30;

    if (pst_process)
    {
        for (i_tries = 10; !b_result && i_tries; i_tries--)
        {
            switch (waitpid(pst_process->u_pid, &i_res, WNOHANG))
            {
                case 0:
                case -1:
                    axsleep(100);
                    break;

                default:
                    b_result        = true;

                    if (pd_result)
                        *pd_result  = WEXITSTATUS(i_res);
                    break;
            }
        }
    }

    return b_result;
}
// ***************************************************************************
// FUNCTION
//      axprocess_wait
// PURPOSE
//
// PARAMETERS
//      HAXPROCESS h_process --
//      UINT       u_TO      --
// RESULT
//      int --
// ***************************************************************************
int axprocess_wait(HAXPROCESS    h_process,
                   UINT           u_TO)
{
    int                 i_result            = true;
    PPROCESS            pst_process         = (PPROCESS)h_process;
    UINT                u_cnt               = 10;
    UINT                u_sleep;
    int                 i_res;

    ENTER(true);

    if ((u_sleep = u_TO / u_cnt) == 0)
        u_sleep = 1;

    i_result = 0;

    while (u_cnt--)
    {
        switch (waitpid(pst_process->u_pid, &i_res, WNOHANG))
        {
            case 0:
                axsleep(u_sleep);
                break;

            case -1:
                u_cnt       = 0;
                break;

            default:
                i_result    = 1;
                u_cnt       = 0;
                break;
        }
    }

    RETURN(i_result);
}
// ***************************************************************************
// FUNCTION
//      la6_process_kill
// PURPOSE
//      Kill Process
// PARAMETERS
//      HAXPROCESS h_process -- Handler of Process
//      DWORD       d_exit_code -- Force Exit Code
// RESULT
//      BOOL
//          TRUE:  Process killed
//          FALSE: An error has occured
// ***************************************************************************
BOOL axprocess_kill(HAXPROCESS  h_process,
                    UINT        d_exit_code)
{
    PPROCESS            pst_process         = (PPROCESS)h_process;
    BOOL                b_result            = false;
//    int                 i_res;


    if (pst_process && pst_process->u_pid)
    {
        b_result = !kill(pst_process->u_pid, SIGKILL);
        axprocess_get_exit_code((HAXPROCESS)pst_process, NULL);
    }

    return b_result;
}
// ***************************************************************************
// FUNCTION
//      la6_process_hup
// PURPOSE
//      Kill Process
// PARAMETERS
//      HAXPROCESS h_process -- Handler of Process
// RESULT
//      BOOL
//          TRUE:  Process killed
//          FALSE: An error has occured
// ***************************************************************************
BOOL axprocess_hup(HAXPROCESS h_process)
{
    PPROCESS            pst_process         = (PPROCESS)h_process;
    BOOL                b_result            = false;

    if (pst_process && pst_process->u_pid)
    {
        b_result = !kill(pst_process->u_pid, SIGHUP);
        axprocess_get_exit_code((HAXPROCESS)pst_process, NULL);
    }

    return b_result;
}
// ***************************************************************************
// FUNCTION
//      la6_process_int
// PURPOSE
//
// PARAMETERS
//      HAXPROCESS h_process --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL axprocess_int(HAXPROCESS h_process)
{
    PPROCESS            pst_process         = (PPROCESS)h_process;
    BOOL                b_result            = false;

    if (pst_process && pst_process->u_pid)
    {
        b_result = !kill(pst_process->u_pid, SIGINT);
    }

    return b_result;
}
// ***************************************************************************
// FUNCTION
//      la6_process_stdin
// PURPOSE
//
// PARAMETERS
//      HAXPROCESS h_process --
// RESULT
//      HAXPIPE    --
// ***************************************************************************
HAXPIPE axprocess_stdin(HAXPROCESS h_process)
{
    PAXPIPE             pst_pipe            = NULL;
    PPROCESS            pst_process         = (PPROCESS)h_process;
    int                 i_pipe;

    if (pst_process)
    {
        if ((i_pipe = ((pst_process->i_pty_master != -1) ?
                pst_process->i_pty_master : pst_process->ai_stdin[1])) != -1)
        {
            if ((pst_pipe = CREATE(AXPIPE)) != NULL)
            {
                pst_pipe->i_in      = -1;
                pst_pipe->i_out     = dup(i_pipe);
            }
        }
    }

    return (HAXPIPE)pst_pipe;
}

// ***************************************************************************
// FUNCTION
//      la6_process_stdout
// PURPOSE
//
// PARAMETERS
//      HAXPROCESS h_process --
// RESULT
//      HAXPIPE    --
// ***************************************************************************
HAXPIPE axprocess_stdout(HAXPROCESS h_process)
{
    PAXPIPE             pst_pipe            = NULL;
    PPROCESS            pst_process         = (PPROCESS)h_process;
    int                 i_pipe;


    if (pst_process)
    {
        if ((i_pipe = ((pst_process->i_pty_master != -1) ?
                pst_process->i_pty_master : pst_process->ai_stdout[0])) != -1)
        {
            if ((pst_pipe = CREATE(AXPIPE)) != NULL)
            {
                pst_pipe->i_in      = dup(i_pipe);
                pst_pipe->i_out     = -1;
            }
        }
    }

    return (HAXPIPE)pst_pipe;
}

// ***************************************************************************
// FUNCTION
//      la6_process_is_alive
// PURPOSE
//      Check whether a process is alive
// PARAMETERS
//      HAXPROCESS h_process -- Handler of Process
// RESULT
//      BOOL
//          TRUE:  All is ok
//          FALSE: An error has occured
// ***************************************************************************
BOOL axprocess_is_alive(HAXPROCESS h_process)
{
    BOOL                b_result        = false;
    PPROCESS            pst_process     = (PPROCESS)h_process;
    int                 i_res;

    if (pst_process)
    {
        do
        {
            if (waitpid(pst_process->u_pid, &i_res, WNOHANG) == 0)
            {
                    b_result = true;
            }

        } while (!b_result && (errno == EINTR));
    }

    return b_result;
}
// ***************************************************************************
// FUNCTION
//      a7process_is_alive
// PURPOSE
//
// PARAMETERS
//      UINT pid --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL axprocess_is_alive2(UINT pid)
{
    return (kill(pid, 0) != -1);
}
// ***************************************************************************
// FUNCTION
//      la6_process_stderr
// PURPOSE
//
// PARAMETERS
//      HAXPROCESS h_process --
// RESULT
//      HAXPIPE    --
// ***************************************************************************
HAXPIPE axprocess_stderr(HAXPROCESS h_process)
{
    PAXPIPE             pst_pipe            = NULL;
    PPROCESS            pst_process         = (PPROCESS)h_process;
    int                 i_pipe;

    if (pst_process)
    {
        if ((i_pipe = ((pst_process->i_pty_master != -1) ?
                pst_process->i_pty_master : pst_process->ai_stderr[0])) != -1)
        {
            if ((pst_pipe = CREATE(AXPIPE)) != NULL)
            {
                pst_pipe->i_in      = dup(i_pipe);
                pst_pipe->i_out     = -1;
            }
        }
    }

    return (HAXPIPE)pst_pipe;
}


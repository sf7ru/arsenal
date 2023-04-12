◘// ***************************************************************************
// TITLE
//      System Service for UNIX Platform
// PROJECT
//      Arsenal Library
// ***************************************************************************
//
// FILE
//      $Id: service.c,v 1.1 2003/02/28 10:34:31 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// HISTORY
//      $Log: service.c,v $
//      Revision 1.1  2003/02/28 10:34:31  A.Kozhukhar
//      Initial release
//
// ***************************************************************************


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <syslog.h>
#include <errno.h>
#include <pwd.h>
#include <signal.h>

#include <arsenal.h>
#include <axsystem.h>

#include <AXService.h>



// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

static PAXSERVICECTL g_pst_ctrl     = NULL;

// ***************************************************************************
// FUNCTION
//      _sig_SIGHUP
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      int   i --
// RESULT
//      void --
// ***************************************************************************
static void _sig_SIGHUP(int i)
{
    ENTER(true);

    if (g_pst_ctrl)
    {
        (*g_pst_ctrl->pfn_sig_handler)(g_pst_ctrl, AXSIG_QUIT);
    }

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      service_console2devnull
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//      int  --
// ***************************************************************************
int service_console2devnull(void)
{
    int i_stdin       = -1;
    int rd;

    ENTER(true);

    switch (i_stdin = open("/dev/null",O_RDWR))
    {
        case 0:
            rd = dup(i_stdin); // stdout
            rd = dup(i_stdin); // stderr
            rd = rd;
            break;

        case -1:
            break;

        default:
            AXTRACE((AXP, "service_devnull_open: console already exist!\n"));
            close(i_stdin);
            break;
    }

    RETURN(i_stdin);
}
// ***************************************************************************
// FUNCTION
//      axservice_register
// PURPOSE
//      Register current executable as System Service
// PARAMETERS
//      PSTR psz_name   -- Name of Service
//      PSTR psz_desc   -- Description of Service
//      PSTR psz_user   -- User account name of Service's owner
//      PSTR psz_passwd -- Password for User account
// RESULT
//      BOOL -- true if all is ok or false if error has occured
// ***************************************************************************
BOOL axservice_register(PSTR psz_name, PSTR psz_desc, PSTR psz_user, PSTR psz_passwd)
{
    BOOL        b_result            = false;

    FIXME("implement");

    return b_result;
}

// ***************************************************************************
// FUNCTION
//      axservice_unregister
// PURPOSE
//      Unregister current executable as System Service
// PARAMETERS
//      PSTR psz_name -- Name of Service
// RESULT
//      BOOL -- true if all is ok or false if error has occured
// ***************************************************************************
BOOL axservice_unregister(PSTR psz_name)
{
    BOOL        b_result            = false;

    FIXME("implement");

    return b_result;
}

/*
static void child_handler(int signum)
{
    switch(signum) {
    case SIGALRM: exit(EXIT_FAILURE); break;
    case SIGUSR1: exit(EXIT_SUCCESS); break;
    case SIGCHLD: exit(EXIT_FAILURE); break;
    }
}

// ***************************************************************************
// FUNCTION
//      _create_lock
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_path --
// RESULT
//      BOOL --
// ***************************************************************************
static BOOL _create_lock(PSTR psz_path)
{
    BOOL        b_result    = false;
//    int                     lfp                 = -1;

    ENTER(true)

    b_result = true;
    // Create the lock file as the current user
//        if ( !(lockfile && lockfile[0])                         ||
//             ((lfp = open(psz_path,O_RDWR|O_CREAT,0640)) >= 0)  )

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      _drop_user
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//      BOOL --
// ***************************************************************************
static BOOL _drop_user(PSTR psz_user)
{
    BOOL                b_result    = false;
    struct passwd *     pw;

    ENTER(true)

    // Drop user if there is one, and we were run as root
    if (!getuid() || !geteuid())
    {
        if ((pw = getpwnam(psz_user)) != NULL)
        {
            AXTRACE("  stage -> %s\n", psz_user);
            setuid(pw->pw_uid);

            b_result = true;
        }
    }

    RETURN(b_result);
}
*/
/*
// ***************************************************************************
// FUNCTION
//      axservice_reborn
// PURPOSE
//      Reborn to Service
// PARAMETERS
//      PAXSERVICECTL pst_ctrl -- Pointer to Control structure
// RESULT
//      BOOL -- true if all is ok or false if error has occured
// ***************************************************************************
BOOL axservice_reborn(PAXSERVICECTL pst_ctrl)
{
    BOOL                    b_result            = false;
    pid_t                   pid;
    pid_t                   sid;
    pid_t                   parent;
    PSTR                    psz_locfile         = NULL;
    PSTR                    psz_user            = "service";

    ENTER(pst_ctrl);

    // already a service
    if (getppid() != 1)
    {
        // Create the lock file as the current user
        if (    (!psz_locfile || _create_lock(psz_locfile)) &&
                (!psz_user    || _drop_user(psz_user))      )
        {
            // Trap signals that we expect to recieve
            signal(SIGCHLD,child_handler);
            signal(SIGUSR1,child_handler);
            signal(SIGALRM,child_handler);

            // Fork off the parent process
            switch (pid = fork())
            {
                case -1:
                    // Error
                    break;

                case 0:
                    // Wait for confirmation from the child via SIGTERM or SIGCHLD, or
                    // for two seconds to elapse (SIGALRM).  pause() should not return.
                    alarm(2);
                    pause();
                    break;

                default:
                    AXTRACE("  IJ\n");
                    // At this point we are executing as the child process
                    parent = getppid();

                    // Cancel certain signals
                    signal(SIGCHLD,SIG_DFL); // A child process dies
                    signal(SIGTSTP,SIG_IGN); // Various TTY signals
                    signal(SIGTTOU,SIG_IGN);
                    signal(SIGTTIN,SIG_IGN);
                    signal(SIGHUP, SIG_IGN); // Ignore hangup signal
                    signal(SIGTERM,SIG_DFL); // Die on SIGTERM

                    // Change the file mode mask
                    umask(0);

                    // Create a new SID for the child process
                    if ((sid = setsid()) >= 0)
                    {
                        // Change the current working directory.  This prevents the current
                        // directory from being locked; hence not being able to remove it.
                        if (!chdir("/"))
                        {
                            // Redirect standard files to /dev/null
                            freopen( "/dev/null", "r", stdin);
                            freopen( "/dev/null", "w", stdout);
                            freopen( "/dev/null", "w", stderr);

                            // Tell the parent process that we are A-okay
                            kill( parent, SIGUSR1);
                            b_result = true;
                        }
                        else
                            AXTRACE("  err 7\n");
                    }
                    else
                    {
                        perror("setsid");
                        AXTRACE("  err 6\n");
                    }
                    break;
            }
        }
        else
            AXTRACE("  err 2\n");
    }
    else
        AXTRACE("  err 1\n");

    RETURN(b_result);
}
*/

/*
// ***************************************************************************
// FUNCTION
//      axservice_reborn
// PURPOSE
//      Spawn to Service
// PARAMETERS
//      PAXSERVICECTL pst_ctrl -- Pointer to Control structure
// RESULT
//      BOOL -- true if all is ok or false if error has occured
// ***************************************************************************
BOOL axservice_reborn(PAXSERVICECTL pst_ctrl)
{
    BOOL            b_result            = false;
    pid_t           pid;

    ENTER(pst_ctrl);

    // Clear file creation mask
    umask(0);

    // Fork off the parent process
    switch (pid = fork())
    {
        case -1:
            // Error
            break;

        case 0:
            if (setsid() < 0)
                perror("setsid: ");

            // Fork off the parent process
            switch (pid = fork())
            {
                case -1:
                    break;

                case 0:
                    chdir("/");
                    freopen( "/dev/null", "r", stdin);
                    freopen( "/dev/null", "w", stdout);
                    freopen( "/dev/null", "w", stderr);

                    b_result = true;
                    break;

                default:
                    _exit(0);
                     break;
            }
            break;

        default:
            _exit(0);
            break;
    }

    RETURN(b_result);
}
*/
// ***************************************************************************
// FUNCTION
//      axservice_sighandler
// PURPOSE
//
// PARAMETERS
//      PAXSERVICECTL pst_ctrl --
// RESULT
//      void --
// ***************************************************************************
void axservice_sighandler(PAXSERVICECTL pst_ctrl)
{
    g_pst_ctrl = pst_ctrl;

    if (pst_ctrl)
    {
        signal(SIGHUP, _sig_SIGHUP);
        signal(SIGINT, _sig_SIGHUP);
    }
    else
    {
        signal(SIGHUP, SIG_DFL);
        signal(SIGINT, SIG_DFL);
    }

}
// ***************************************************************************
// FUNCTION
//      axservice_reborn
// PURPOSE
//      Spawn to Service
// PARAMETERS
//      PAXSERVICECTL pst_ctrl -- Pointer to Control structure
// RESULT
//      BOOL -- true if all is ok or false if error has occured
// ***************************************************************************
BOOL axservice_reborn(PAXSERVICECTL pst_ctrl)
{
    BOOL            b_result            = false;
    pid_t           pid;
    int             i;
    int             num_fds;

    ENTER(pst_ctrl);

    if (getppid() != 1)
    {
        // Fork off the parent process
        switch (pid = fork())
        {
            case -1:
                // Error
                break;

            case 0:
                // Clear file creation mask
                umask(0);

                num_fds = getdtablesize();

                for (i = 0; i < num_fds; i++)
                {
                    close(i);
                }

                setsid();

                axservice_sighandler(pst_ctrl);

                if (pst_ctrl->pfn_entry_point)
                {
                    (*pst_ctrl->pfn_entry_point)(pst_ctrl);
                }

                axservice_sighandler(NULL);

                b_result = true;
                break;

            default:
                _exit(0);
                break;
        }
    }

    RETURN(b_result);
}


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

#include <sys/resource.h>
#include <signal.h>

#include <a7string.h>
#include <a7file.h>
#include <AXApp.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

//static  PCSTR               gBaseDir    = nil;
//static  PCSTR               gRelease    = nil;
//static  PCSTR               gBinary     = nil;


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

/*
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      sig_fatal
// PURPOSE
//
// PARAMETERS
//      int sig --
// RESULT
//      void --
// ***************************************************************************
static void sig_fatal(int sig)
{
    AXDATE      date;
    pid_t       pid;
    CHAR        path        [ AXLPATH ];
    CHAR        fname       [ AXLPATH ];
    CHAR        binfname    [ AXLPATH ];

    pid = getpid();

    if (gBaseDir)
    {
        a7time_get_date(&date, false);

        strz_sformat(path, AXLPATH, "%s%s%s/%.4d%.2d%.2d%.2d%.2d%.2d-%u",
             gBaseDir,
             gRelease ? "/"      : "",
             gRelease ? gRelease : "",
             date.u_year,
             date.u_month,
             date.u_day,
             date.u_hour,
             date.u_minute,
             date.u_second,
             pid);

        if (la6_path_create(path))
        {
            if (!chdir(path))
            {
                printf("codedump directory: '%s'\n", path);

                if (gBinary)
                {
                    fname_get_name(fname, AXLPATH, gBinary);

                    strz_sformat(binfname, AXLPATH, "%s/%s", path, fname);

                    if (a7file_copy(binfname, gBinary, false))
                    {
                        printf("copied: '%s' -> '%s'\n", binfname, gBinary);
                    }
                }
            }
        }
    }


    signal(sig, SIG_DFL);
    kill(pid, sig);
}
*/
// ***************************************************************************
// FUNCTION
//      AXApp::setCrashDumpDir
// PURPOSE
//
// PARAMETERS
//      PCSTR base        --
//      PCSTR releaseDesc --
// RESULT
//      void --
// ***************************************************************************
void AXApp::setCrashDumpDir(PCSTR   baseDir,
                            PCSTR   releaseTag,
                            PCSTR   binaryFname)
{
//    gBaseDir    = baseDir;
//    gRelease    = releaseTag;
//    gBinary     = binaryFname;
}

// ***************************************************************************
// FUNCTION
//      AXApp::setCrashHook
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL AXApp::setCrashHook(AXApp *       that)
{
    BOOL                b_result        = true;
//    struct rlimit       core_limits;
//    struct sigaction    sa;

    ENTER(TRUE);

//    // allowing coredumps
//    core_limits.rlim_cur = core_limits.rlim_max = RLIM_INFINITY;
//    setrlimit(RLIMIT_CORE, &core_limits);
//
//
//    // Setting one fatal signal handler
//    sigemptyset(&sa.sa_mask);
//    sa.sa_flags         = SA_ONESHOT;
//    sa.sa_handler       = sig_fatal;
//
//    sigaction(SIGSEGV,  &sa, nil);
//    sigaction(SIGBUS,   &sa, nil);
//    sigaction(SIGABRT,  &sa, nil);
//    sigaction(SIGILL,   &sa, nil);

    RETURN(b_result);
}

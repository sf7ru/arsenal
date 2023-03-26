// ***************************************************************************
// TITLE
//      Essay
// PROJECT
//
// ***************************************************************************
//
// FILE
//      $Id:$
// HISTORY
//      $Log:$
// ***************************************************************************

#define _GNU_SOURCE

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>

#include <axsystem.h>
#include <axtime.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

//#define PIPE_BUF            16


// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// ---------------------------------------------------------------------------

static PFNAXITIMERCB    g_pfn_cb    = NULL;
static PVOID            g_p_ptr     = NULL;

static int              g_ai_fd     [2] = { -1, -1 };

volatile sig_atomic_t   g_b_on      = 0;

static AXTIME           itemerWas;

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      _timer
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      int   signo --
// RESULT
//      void --
// ***************************************************************************
static void _timer(int signo)
{
    int         i_data      = 1;

    if (g_b_on)
    {
        if (write(g_ai_fd[1], &i_data, 1))
        {
            ;
        }
    }
}
// ***************************************************************************
// FUNCTION
//      _itimer_clock
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      AXTIME --
// ***************************************************************************
AXTIME _itimer_clock()
{
    //CLOCK_MONOTONIC CLOCK_MONOTONIC_RAW*/
    #define TIME_SOURCE CLOCK_MONOTONIC


    AXTIME             result;
    struct timespec     tm;

    clock_gettime(TIME_SOURCE, &tm);

    result  = tm.tv_sec * 1000000000LL;
    result += tm.tv_nsec;
    result /= 1000;
    //printf("_clock stage clock_gettime %lu + %lu = %llu\n", tm.tv_sec, tm.tv_nsec, result);

    return result;
}
// ***************************************************************************
// FUNCTION
//      _thread
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//         PVOID --
// RESULT
//      void --
// ***************************************************************************
static void _thread(PVOID   param)
{
    UINT            u_retcode       = 0;
    PFNAXITIMERCB   call;
    AXTIME         now;
    LONG            passed;
    char            ac_data         [ PIPE_BUF ];

    itemerWas = _itimer_clock();

    g_b_on = 1;

    while ((call = g_pfn_cb) != NULL)
    {
        if (read(g_ai_fd[0], ac_data, PIPE_BUF) > 0)
        {
            now         = _itimer_clock();
            passed      = now - itemerWas;
            itemerWas   = now;

            if (passed > 0)
            {
                (*call)(g_p_ptr, passed);
            }
            else
                printf(" ITIMER MONOTONIC TIME ZERO INC\n");
        }
    }

    g_b_on = 0;

    pthread_exit(&u_retcode);
}
// ***************************************************************************
// FUNCTION
//      axitimer_set
// PURPOSE
//
// PARAMETERS
//      PFNAXITIMERCB   pfn_cb --
//      PVOID           p_ptr  --
//      UINT            u_sec  --
//      UINT            u_usec --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL axitimer_set(PFNAXITIMERCB     pfn_cb,
                  PVOID             p_ptr,
                  UINT              u_sec,
                  UINT              u_usec)
{
    BOOL                b_result        = false;
    struct itimerval    itvl;
    struct sigaction    sact;
    pthread_t           v_thread;
    pthread_attr_t      attr;
    struct sched_param  st_param;
    int                 i_flags;
//    cpu_set_t           cpuset;

    ENTER(pfn_cb);

    if (!g_b_on)
    {
        itvl.it_interval.tv_sec     = u_sec;
        itvl.it_interval.tv_usec    = u_usec;
        itvl.it_value.tv_sec        = u_sec;
        itvl.it_value.tv_usec       = u_usec;

        g_ai_fd[0]                  = -1;
        g_ai_fd[1]                  = -1;

        sact.sa_handler             = _timer;
        sact.sa_flags               = SA_RESTART;

        if (  (sigemptyset(&sact.sa_mask)        >= 0)  &&
              (sigaddset(&sact.sa_mask, SIGALRM) >= 0)  &&
              !sigaction(SIGALRM,     &sact, NULL)      &&
              !setitimer(ITIMER_REAL, &itvl, NULL)      )
        {
            if (pipe(g_ai_fd) == 0)
            {
                i_flags = fcntl(g_ai_fd[1], F_GETFL, 0);
                fcntl(g_ai_fd[1], F_SETFL, i_flags | O_NDELAY);

                g_p_ptr     = p_ptr;
                g_pfn_cb    = pfn_cb;

                pthread_attr_init(&attr);

                if (!pthread_create(&v_thread, &attr, (void*(*)(void*))_thread, NULL))
                {
                    memset(&st_param, 0, sizeof(st_param));
                    st_param.sched_priority = 30;

                    pthread_setschedparam(v_thread, SCHED_FIFO, &st_param);

//                    CPU_ZERO(&cpuset);
//                    CPU_SET(0,&cpuset);
//                    pthread_setaffinity_np(v_thread, sizeof(cpu_set_t), &cpuset);

                    b_result = true;
                }
                else
                    axitimer_stop();
            }
            else
                axitimer_stop();
        }
        else
            axitimer_stop();
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axitimer_stop
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL axitimer_stop(void)
{
    BOOL                b_result        = false;
    struct sigaction    sact;
    struct itimerval    itvl;

    ENTER(true);

    if (g_b_on)
    {
        memset(&itvl, 0, sizeof(itvl));

        setitimer(ITIMER_REAL, &itvl, NULL);

        sact.sa_handler             = SIG_IGN;
        sact.sa_flags               = SA_RESTART;

        sigemptyset(&sact.sa_mask);
        sigaddset(&sact.sa_mask, SIGALRM);
        sigaction(SIGALRM, &sact, NULL);

        g_pfn_cb    = NULL;

        // push for read(g_ai_fd[0],..
        if (write(g_ai_fd[1], &itvl, sizeof(itvl)))
        {
            ;
        }

        while (g_b_on)
        {
            usleep(100);
        }

        if (g_ai_fd[0] != -1)
        {
            close(g_ai_fd[0]);
            g_ai_fd[0] = -1;
        }

        if (g_ai_fd[1] != -1)
        {
            close(g_ai_fd[1]);
            g_ai_fd[1] = -1;
        }

        b_result    = true;
    }

    RETURN(b_result);
}

/*
    BOOL                b_result        = false;
    struct itimerval    itvl;
    struct sigaction    sact;
    sigset_t            sset;
    pthread_t           v_thread;
    pthread_attr_t      attr;
    struct sched_param  st_param;

        memset(&sact, 0, sizeof(sact));
        memset(&sset, 0, sizeof(sset));

        sact.sa_handler             = _timer;
        sact.sa_flags               = SA_RESTART;
        sact.sa_mask                = sset;

        itvl.it_interval.tv_sec     = u_sec;
        itvl.it_interval.tv_usec    = u_usec;
        itvl.it_value.tv_sec        = u_sec;
        itvl.it_value.tv_usec       = u_usec;

        g_ai_fd[0]                  = -1;
        g_ai_fd[1]                  = -1;

        if (  (sigemptyset(&sset)        >= 0)      &&
              (sigaddset(&sset, SIGALRM) >= 0)      &&
              !sigaction(SIGALRM,     &sact, NULL)  &&
              !setitimer(ITIMER_REAL, &itvl, NULL)  )
        {
            if (pipe(g_ai_fd) == 0)
            {
                g_p_ptr     = p_ptr;
                g_pfn_cb    = pfn_cb;

                pthread_attr_init(&attr);

                if (!pthread_create(&v_thread, &attr, (void*(*)(void*))_thread, NULL))
                {
                    memset(&st_param, 0, sizeof(st_param));
                    st_param.sched_priority = 30;

                    pthread_setschedparam(v_thread, SCHED_FIFO, &st_param);

                    b_result = true;
                }
                else
                    axitimer_stop();
            }
            else
                axitimer_stop();
        }
        else
            axitimer_stop();
 */

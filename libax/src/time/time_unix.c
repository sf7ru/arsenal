// ***************************************************************************
// TITLE
//      Time UNIX Platform
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: time.c,v 1.8 2004/03/24 08:28:26 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************

#include <stdlib.h>

#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/times.h>
#include <sys/time.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>

#include <arsenal.h>
#include <axtime.h>

// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      axdate_get
// PURPOSE
//
// PARAMETERS
//      PAXDATE pst_time --
// RESULT
//      void --
// ***************************************************************************
void axdate_get(PAXDATE     pst_time)
{
    time_t          st_time;
    struct tm *     pst_tm;

    time(&st_time);

    memset(pst_time, 0, sizeof(AXDATE));
    pst_time->day   = 1;
    pst_time->month = 1;
    pst_time->year  = 1900;

    if ((pst_tm = gmtime(&st_time)) != NULL)
    {
        pst_time->hour          = (U16)pst_tm->tm_hour;
        pst_time->minute        = (U16)pst_tm->tm_min;
        pst_time->second        = (U16)pst_tm->tm_sec;
        pst_time->milliseconds  = 0;

        pst_time->day          += (U16)pst_tm->tm_mday - 1;
        pst_time->month        += (U16)pst_tm->tm_mon;
        pst_time->year         += (U16)pst_tm->tm_year;
        pst_time->day_of_week   = (U16)pst_tm->tm_wday;
    }
}
void axdate_get_local(PAXDATE     pst_time)
{
    time_t          st_time;
    struct tm *     pst_tm;

    time(&st_time);

    memset(pst_time, 0, sizeof(AXDATE));
    pst_time->day   = 1;
    pst_time->month = 1;
    pst_time->year  = 1900;

    if ((pst_tm = localtime(&st_time)) != NULL)
    {
        pst_time->hour          = (U16)pst_tm->tm_hour;
        pst_time->minute        = (U16)pst_tm->tm_min;
        pst_time->second        = (U16)pst_tm->tm_sec;
        pst_time->milliseconds  = 0;

        pst_time->day          += (U16)pst_tm->tm_mday - 1;
        pst_time->month        += (U16)pst_tm->tm_mon;
        pst_time->year         += (U16)pst_tm->tm_year;
        pst_time->day_of_week   = (U16)pst_tm->tm_wday;
    }
}
// ***************************************************************************
// FUNCTION
//      a7date_short
// PURPOSE
//
// PARAMETERS
//      PAXDATE pst_time --
// RESULT
//      AXUTIME --
// ***************************************************************************
AXUTIME axdate_to_utime(PAXDATE  pst_time)
{
    AXUTIME          result      = 0;
    struct tm       tm;

    ENTER(true);

    memset(&tm, 0, sizeof(tm));

    tm.tm_year  = pst_time->year - 1900;
    tm.tm_mon   = pst_time->month - 1;
    tm.tm_mday  = pst_time->day;
    tm.tm_hour  = pst_time->hour;
    tm.tm_min   = pst_time->minute;
    tm.tm_sec   = pst_time->second;

    result      = timegm(&tm);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      axudelay
// PURPOSE
//      Delay for some microseconds
// PARAMETERS
//      UINT useconds - Sleep period in milliseconds
// RESULT
//      none
// ***************************************************************************
void axudelay(UINT u_delay)
{
    UINT                u_left   = u_delay;
    UINT                u_passed;
    struct timeval      start;
    struct timeval      curr;
    struct timeval      res;
    struct timezone     zn;

    // usleep can be interrupted by I/O operation much earlier, then expected

    if (u_delay > 10)
    {
        // We will check the time if parameter contains amount of msecons

        gettimeofday(&start, &zn);

        while (  //printf("usleep(%d)\n", u_left) &&
                        (usleep(u_left) == -1))
        {
            gettimeofday(&curr, &zn);
            timersub(&curr, &start, &res);

            u_passed = (res.tv_sec * 1000000) + res.tv_usec;

            if (u_passed < u_delay)
                u_left = u_delay - u_passed;
            else
                break;
        }
    }
    else
    {
        // We will not check, if paramter is an one msec,
        // and function called just for allow threads to switch
        usleep(u_left);
    }
}

// ***************************************************************************
// FUNCTION
//      a7sleep
// PURPOSE
//      Sleep for some time
// PARAMETERS
//      DU16 d_mseconds - Sleep period in milliseconds
// RESULT
//      none
// ***************************************************************************
void axsleep(UINT d_mseconds)
{
    UINT                u_delay  = d_mseconds * 1000;
    UINT                u_left   = u_delay;
    UINT                u_passed;
    struct timeval      start;
    struct timeval      curr;
    struct timeval      res;
    struct timezone     zn;

    // usleep can be interrupted by I/O operation much earlier, then expected

    if (d_mseconds > 1)
    {
        // We will check the time if parameter contains amount of msecons

        gettimeofday(&start, &zn);

        while (  //printf("usleep(%d)\n", u_left) &&
                        (usleep(u_left) == -1))
        {
            gettimeofday(&curr, &zn);
            timersub(&curr, &start, &res);

            u_passed = (res.tv_sec * 1000000) + res.tv_usec;

            if (u_passed < u_delay)
                u_left = u_delay - u_passed;
            else
                break;
        }
    }
    else
    {
        // We will not check, if paramter is an one msec,
        // and function called just for allow threads to switch
        usleep(u_left);
    }
}
// ***************************************************************************
// FUNCTION
//      la6_clock_get_ticks_per_sec
// PURPOSE
//
// PARAMETERS
//      none
// RESULT
//      ULONG   --
// ***************************************************************************
ULONG axclock_get_ticks_per_sec(void)
{
    return sysconf(_SC_CLK_TCK);
}

// ***************************************************************************
// FUNCTION
//      la6_clock
// PURPOSE
//
// PARAMETERS
//      none
// RESULT
//      ULONG   --
// ***************************************************************************
ULONG axclock(void)
{
    return times(NULL);
}



// ***************************************************************************
// FUNCTION
//      la6_random
// PURPOSE
//
// PARAMETERS
//      none
// RESULT
//      ULONG   --
// ***************************************************************************
ULONG axrandom(void)
{
    ULONG               u_value         = 0;

    ENTER(true);

//    GetSystemTime(&st_time);
//    u_value = st_time.wMilliseconds + GetCurrentThreadId();
    u_value = rand();

    RETURN(u_value);
}
// ***************************************************************************
// FUNCTION
//      a7time_gm2local
// PURPOSE
//
// PARAMETERS
//      AXUTIME time --
// RESULT
//      AXUTIME --
// ***************************************************************************
AXUTIME axtime_gm2local(AXUTIME  time)
{
    AXUTIME          result          = false;
    struct tm *     pst_tmp;

    ENTER(true);

    pst_tmp = localtime((time_t*)&time);
    result  = timegm(pst_tmp);

//    printf("a7time_gm2local stage %d -> %d \n", (int)time, (int)result);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      axtime_local2gm
// PURPOSE
//
// PARAMETERS
//      AXUTIME * pv_time --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL axutime_local2gm(AXUTIME * pv_time)
{
    BOOL            b_result        = false;
    struct tm *     tptr;
    time_t          time;

    ENTER(pv_time);

    time = (time_t)*pv_time;

    tptr     = gmtime(&time);
    *pv_time = (AXUTIME)mktime(tptr);

    b_result = true;

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axtime_get
// PURPOSE
//
// PARAMETERS
//      none
// RESULT
//      AXUTIME --
// ***************************************************************************
AXUTIME axutime_get()
{
    time_t          v_now;

    v_now = time(NULL);

    return (AXUTIME)v_now;
}
// ***************************************************************************
// FUNCTION
//      axdelay
// PURPOSE
//
// PARAMETERS
//      UINT TO --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
void axdelay(UINT TO)
{
    axsleep(TO);
}

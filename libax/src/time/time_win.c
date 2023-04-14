// ***************************************************************************
// TITLE
//      Time Win32 Platform
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: time.c,v 1.5 2003/09/17 11:57:07 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************


#include <stdio.h>

#include <time.h>

#include <windows.h>

#include <arsenal.h>
#include <axtime.h>

#include <pvt_win.h>


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      a7time_get
// PURPOSE
//      Get current time
// PARAMETERS
//      PAXDATE pst_time -- Pointer to Time structure
// RESULT
//      none
// ***************************************************************************
void axdate_get(PAXDATE     pst_time)
{
    SYSTEMTIME st_time;

    if (pst_time)
    {
        GetSystemTime(&st_time);

        MAC_TIME_WIN2AX(pst_time, (&st_time));
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
AXUTIME axutime_get()
{
    time_t          v_now;

    v_now = time(NULL);

    return (AXUTIME)v_now;
}
/*
// ***************************************************************************
// FUNCTION
//      a7time_set_rtc
// PURPOSE
//
// PARAMETERS
//      PAXDATE pst_time --
// RESULT
//      BOOL
//          TRUE:  All is ok
//          FALSE: An error has occured
// ***************************************************************************
BOOL a7time_set_rtc(PAXDATE pst_time)
{
    FIXME("implement");

    return FALSE;
}

// ***************************************************************************
// FUNCTION
//      a7time_set
// PURPOSE
//
// PARAMETERS
//      PAXDATE pst_time --
// RESULT
//      BOOL
//          TRUE:  All is ok
//          FALSE: An error has occured
// ***************************************************************************
BOOL a7time_set(PAXDATE pst_time)
{
    FIXME("implement");

    return FALSE;
}
*/
// ***************************************************************************
// FUNCTION
//      a7sleep
// PURPOSE
//      Sleep for some time
// PARAMETERS
//      DWORD d_mseconds - Sleep period in milliseconds
// RESULT
//      none
// ***************************************************************************
void axsleep(UINT d_mseconds)
{
    Sleep(d_mseconds);
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
    return CLOCKS_PER_SEC;
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
    return clock();
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
    SYSTEMTIME          st_time;

    ENTER(TRUE);

    GetSystemTime(&st_time);

    u_value = st_time.wMilliseconds + GetCurrentThreadId();

    RETURN(u_value);
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      is_leap
// PURPOSE
//
// PARAMETERS
//      unsigned y --
// RESULT
//      int --
// ***************************************************************************
static int is_leap(unsigned y)
{
    y += 1900;
    return (y % 4) == 0 && ((y % 100) != 0 || (y % 400) == 0);
}
// ***************************************************************************
// FUNCTION
//      timegm
// PURPOSE
//
// PARAMETERS
//      struct tm *tm --
// RESULT
//      time_t --
// ***************************************************************************
time_t timegm(struct tm *tm)
{
    static const unsigned ndays[2][12] =
    {
            {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
            {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
    };
    time_t res = 0;
    int i;

    for (i = 70; i < tm->tm_year; ++i)
            res += is_leap(i) ? 366 : 365;

    for (i = 0; i < tm->tm_mon; ++i)
            res += ndays[is_leap(tm->tm_year)][i];
    res += tm->tm_mday - 1;
    res *= 24;
    res += tm->tm_hour;
    res *= 60;
    res += tm->tm_min;
    res *= 60;
    res += tm->tm_sec;

    return res;
}
// ***************************************************************************
// FUNCTION
//      a7time_pack
// PURPOSE
//
// PARAMETERS
//      PAXDATE pst_time --
// RESULT
//      AXTIME --
// ***************************************************************************
AXTIME axtime_pack(PAXDATE  pst_time)
{
    AXTIME          result      = 0;
    struct tm       tm;

    ENTER(TRUE);

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

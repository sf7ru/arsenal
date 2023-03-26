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
#include <dreamix.h>

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
//      PA7DATE pst_time -- Pointer to Time structure
// RESULT
//      none
// ***************************************************************************
void a7time_get_date(PA7DATE     pst_time)
{
    SYSTEMTIME st_time;

    if (pst_time)
    {
        GetSystemTime(&st_time);

        MAC_TIME_WIN2LA6(pst_time, (&st_time));
    }
}
/*
// ***************************************************************************
// FUNCTION
//      a7time_set_rtc
// PURPOSE
//
// PARAMETERS
//      PA7DATE pst_time --
// RESULT
//      BOOL
//          TRUE:  All is ok
//          FALSE: An error has occured
// ***************************************************************************
BOOL a7time_set_rtc(PA7DATE pst_time)
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
//      PA7DATE pst_time --
// RESULT
//      BOOL
//          TRUE:  All is ok
//          FALSE: An error has occured
// ***************************************************************************
BOOL a7time_set(PA7DATE pst_time)
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
void a7sleep(DWORD d_mseconds)
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
ULONG la6_clock_get_ticks_per_sec(void)
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
ULONG la6_clock(void)
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
ULONG la6_random(void)
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
//      PA7DATE pst_time --
// RESULT
//      A7TIME --
// ***************************************************************************
A7TIME a7time_pack(PA7DATE  pst_time)
{
    A7TIME          result      = 0;
    struct tm       tm;

    ENTER(TRUE);

    memset(&tm, 0, sizeof(tm));

    tm.tm_year  = pst_time->u_year - 1900;
    tm.tm_mon   = pst_time->u_month - 1;
    tm.tm_mday  = pst_time->u_day;
    tm.tm_hour  = pst_time->u_hour;
    tm.tm_min   = pst_time->u_minute;
    tm.tm_sec   = pst_time->u_second;

    result      = timegm(&tm);

    RETURN(result);
}

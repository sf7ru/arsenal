// ***************************************************************************
// TITLE
//      Time
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: time_common.c,v 1.7 2003/12/17 14:16:02 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <arsenal.h>

#include <axtime.h>
#include <limits.h>

// ---------------------------------------------------------------------------
// -------------------------- LOCAL DEFINITIONS ------------------------------
// ---------------------------------------------------------------------------

#define DEF_SIZEOF_U16          (1L << (8 * sizeof(U16 ))) - 1
#define MAC_DAYSINMONTH(a)      _ax_days_in_month[(UINT)axtime_year_is_leap(a->year)][(UINT)a->month - 1]

// ---------------------------------------------------------------------------
// ----------------------------- STATIC DATA ---------------------------------
// ---------------------------------------------------------------------------

U8      _ax_days_in_month     [ 2 ][ 12 ] =
{
    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
    { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

static int is_leap(unsigned y)
{
    y += 1900;
    return (y % 4) == 0 && ((y % 100) != 0 || (y % 400) == 0);
}

// ***************************************************************************
// FUNCTION
//      axtimegm
// PURPOSE
//
// PARAMETERS
//      struct tm *tm --
// RESULT
//      time_t --
// ***************************************************************************
time_t axtimegm(struct tm *tm)
{
    time_t res = 0;
    int i;

    for (i = 70; i < tm->tm_year; ++i)
            res += is_leap(i) ? 366 : 365;

    for (i = 0; i < tm->tm_mon; ++i)
            res += _ax_days_in_month[is_leap(tm->tm_year)][i];
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
//      la6_time_is_correct
// PURPOSE
//      Check correctness of time
// PARAMETERS
//      PAXDATE pst_time -- Pointer to Time structure
// RESULT
//      BOOL  -- true if time is correct or false if not
// ***************************************************************************
BOOL axdate_is_correct(PAXDATE date)
{
    return (    date                                                            &&
                (date->month && (date->month  < 13))                    &&
                (date->day && (date->day <= MAC_DAYSINMONTH(date))) &&
                (date->hour   < 24)                                           &&
                (date->minute < 60)                                           &&
                (date->second < 60)                                           );
}

// ***************************************************************************
// FUNCTION
//      la6_time_year_is_leap
// PURPOSE
//      Check is the year are leap
// PARAMETERS
//      int  year -- Year
// RESULT
//      BOOL  -- true if year is leap or false if not
// ***************************************************************************
BOOL axtime_year_is_leap(UINT year)
{
    return (div(year,   4).rem ? false :
           (div(year, 100).rem ? true  :
           (div(year, 400).rem ? false : true)));
}

// ***************************************************************************
// FUNCTION
//      la6_time_is_come
// PURPOSE
//      Check is the time came
// PARAMETERS
//      PAXDATE pst_time -- Pointer to Time structure
// RESULT
//      BOOL  -- true if time is come or false if not
// ***************************************************************************
BOOL axdate_is_come(PAXDATE     pst_time)
{
    AXDATE     st_curr_time;

    axdate_get(&st_curr_time);

    return  (st_curr_time.year             >  pst_time->year  ? true :
            (st_curr_time.year    == pst_time->year     ?
                (st_curr_time.month        > pst_time->month  ? true :
            (st_curr_time.month   == pst_time->month    ?
                (st_curr_time.day          > pst_time->day    ? true :
            (st_curr_time.day     == pst_time->day      ?
                (st_curr_time.hour         > pst_time->hour   ? true :
            (st_curr_time.hour    == pst_time->hour     ?
                (st_curr_time.minute       > pst_time->minute ? true :
            (st_curr_time.minute  == pst_time->minute   ?
                (st_curr_time.second       > pst_time->second ? true :
            (st_curr_time.second  == pst_time->second   ?
                (st_curr_time.milliseconds >= pst_time->milliseconds ? true :
            false) : false)) : false)) : false)) : false)) : false)) : false));
}
static BOOL _time_is_between(PAXDATE            n,
                             PAXDATE            b,
                             PAXDATE            e)
{
    BOOL        result          = false;

    ENTER(true);

    //printf("taskMain.cpp(536): stage b->hour = %d e->hour = %d, now->hour = %d\n", b->hour, e->hour, n->hour);

    if ((n->hour > b->hour) && (n->hour < e->hour)) // straight in bounds
    {
        //printf("taskMain.cpp(540): stage 1\n");
        result = true;
    }
    else if (n->hour == b->hour)
    {
        //printf("taskMain.cpp(540): stage 2\n");
        if (n->minute > b->minute)
        {
            //printf("taskMain.cpp(540): stage 3\n");
            result = true;
        }
        else if (n->minute == b->minute)
        {
            //printf("taskMain.cpp(540): stage 4\n");
            result = n->second >= b->second;
        }
    }
    else if (n->hour == e->hour)
    {
        //printf("taskMain.cpp(540): stage 5\n");

        if (n->minute < e->minute)
        {
            //printf("taskMain.cpp(540): stage 6\n");
            result = true;
        }
        else if (n->minute == e->minute)
        {
            //printf("taskMain.cpp(540): stage 7\n");
            result = n->second < b->second;
        }
    }

    RETURN(result);
}
BOOL axdate_time_is_between(PAXDATE            now,
                            PAXDATE            t1,
                            PAXDATE            t2)
{
    return (t2->hour > t1->hour) ? _time_is_between(now, t1, t2) : !_time_is_between(now, t2, t1);
}
// ***************************************************************************
// FUNCTION
//      la6_time_add_ex
// PURPOSE
//
// PARAMETERS
//      PAXDATE pst_time        --
//      PAXDATE pst_time_to_add --
//      U32     d_add_seconds   --
// RESULT
//      none
// ***************************************************************************
void axdate_add_ex(PAXDATE  date,
                   PAXDATE  to_add,
                   UINT     d_add_seconds)
{
    ldiv_t          div_time;
    register U32   d_days;
    register U32   d_in_mth;


    if (date && to_add)
    {
// -------------------------------- Time -------------------------------------

        div_time = ldiv(date->milliseconds + to_add->milliseconds,     1000);
        date->milliseconds = (U16 )div_time.rem;
        div_time = ldiv(div_time.quot + date->second + to_add->second + d_add_seconds, 60);
        date->second       = (U16 )div_time.rem;
        div_time = ldiv(div_time.quot + date->minute + to_add->minute, 60);
        date->minute       = (U16 )div_time.rem;
        div_time = ldiv(div_time.quot + date->hour   + to_add->hour,   24);
        date->hour         = (U16 )div_time.rem;

// -------------------------------- Date -------------------------------------

        date->year  += to_add->year;
        date->month += to_add->month;

        while (date->month > 12)
        {
            to_add->year++;
            date->month -= 12;
        }

        for ( d_days = div_time.quot + date->day + to_add->day;
              d_days > (d_in_mth = MAC_DAYSINMONTH(date));
              d_days -= d_in_mth)
        {
            if (++(date->month) > 12)
            {
                date->year++;
                date->month = 1;
            }
        }

        date->day = (U16 )d_days;
    }
}
// ***************************************************************************
// FUNCTION
//      axdate_add_secs
// PURPOSE
//
// PARAMETERS
//      PAXDATE pst_time      --
//      UINT    d_secs_to_add --
// RESULT
//      void --
// ***************************************************************************
void axdate_add_secs(PAXDATE  pst_time,
                     UINT     d_secs_to_add)
{
    AXDATE     st_time;


    if (pst_time && d_secs_to_add)
    {
        memset(&st_time, 0, sizeof(AXDATE));
        axdate_add_ex(pst_time, &st_time, d_secs_to_add);
    }
}
// ***************************************************************************
// FUNCTION
//      axdate_fom_utime
// PURPOSE
//
// PARAMETERS
//      PAXDATE date --
//      AXUTIME time --
// RESULT
//      void --
// ***************************************************************************
BOOL axdate_from_utime(PAXDATE   date,
                       AXUTIME   time)
{
    BOOL            result          = false;
    time_t          t_time      = time;
    struct tm *     tm;

    ENTER(true);

    tm     = gmtime(&t_time);

    if (tm)
    {
        date->year      = 1900 + tm->tm_year;
        date->month     = 1 + tm->tm_mon;
        date->day       = tm->tm_mday;
        date->hour      = tm->tm_hour;
        date->minute    = tm->tm_min;
        date->second    = tm->tm_sec;

        result          = true;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      axdate_fom_utime
// PURPOSE
//
// PARAMETERS
//      PAXDATE date --
//      AXUTIME time --
// RESULT
//      void --
// ***************************************************************************
BOOL axdate_from_time(PAXDATE   date,
                      AXTIME   time)
{
    BOOL            result          = false;
    time_t          t_time      = time;
    struct tm *     tm;

    ENTER(true);

    tm     = gmtime(&t_time);

    if (tm)
    {
        date->year      = 1900 + tm->tm_year;
        date->month     = 1 + tm->tm_mon;
        date->day       = tm->tm_mday;
        date->hour      = tm->tm_hour;
        date->minute    = tm->tm_min;
        date->second    = tm->tm_sec;

        result          = true;
    }

    RETURN(result);
}

// ***************************************************************************
// FUNCTION
//      la6_time_seconds_since
// PURPOSE
//      Pack the time to standard 32bit value
// PARAMETERS
//      WORD        w_since_year -- Year since which seconds should be count
//      PA7DATE    pst_time     -- Pointer to Time structure
// RESULT
//      ULONG -- Number of seconds if all is ok, or 0 if not
// ***************************************************************************
ULONG axdate_seconds_since(UINT      since_year,
                           PAXDATE   date)
{
    ULONG               u_seconds   = 0;
    UINT                u_year;
    int                 i_month;
    PU8                 p_days;

#define DEF_PACKED_YERS_IN_ULONG    ((ULONG)(ULONG_MAX / (366L * 24L * 60L * 60L)))
#define DEF_CORR_TO_ZERO        1

    if (axdate_is_correct(date))
    {
        if ( !( (date->year < since_year)                               &&
                (date->year > (since_year + DEF_PACKED_YERS_IN_ULONG))  ))
        {
            for (u_year = since_year; u_year < date->year; u_year++)
                u_seconds += (SECS_IN_DAY * (axtime_year_is_leap(u_year) ? 366 : 365));

            p_days = _ax_days_in_month[axtime_year_is_leap(date->year)];

            for (i_month = 0; i_month < (date->month - DEF_CORR_TO_ZERO); i_month++)
                u_seconds += (p_days[i_month] * SECS_IN_DAY);

            u_seconds += (date->day - DEF_CORR_TO_ZERO) * SECS_IN_DAY;
            u_seconds += date->hour * SECS_IN_HOUR;
            u_seconds += date->minute * SECS_IN_MINUTE;
            u_seconds += date->second;
        }
    }

    return u_seconds;
}

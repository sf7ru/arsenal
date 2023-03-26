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


#ifndef __AXTIMEH__
#define __AXTIMEH__

#include <arsenal.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define DAYS_IN_WEEK        7
#define DAYS_IN_LONG_MONTH  31
#define DAYS_IN_YEAR        365

#define HOURS_IN_DAY        24

#define MINS_IN_HOUR        (60)
#define MINS_IN_DAY         (HOURS_IN_DAY * MINS_IN_HOUR)

#define SECS_IN_MINUTE      (60)
#define SECS_IN_HOUR        (SECS_IN_MINUTE * MINS_IN_HOUR)
#define SECS_IN_DAY         (SECS_IN_HOUR * HOURS_IN_DAY)
#define SECS_IN_WEEK        (SECS_IN_DAY * DAYS_IN_WEEK)
#define SECS_IN_MONTH       (SECS_IN_DAY * DAYS_IN_LONG_MONTH)
#define SECS_IN_YEAR        (SECS_IN_DAY * DAYS_IN_YEAR)
#define MSECS_IN_SEC        (1000)
#define USECS_IN_MSEC       (1000)
#define USECS_IN_SEC        (MSECS_IN_SEC * USECS_IN_MSEC)



// ---------------------------------------------------------------------------
// -------------------------------- TYPES ------------------------------------
// ---------------------------------------------------------------------------

typedef U64         AXTIME;
typedef AXTIME *    PAXTIME;
typedef U64         AXCLOCK;
typedef U32         AXUTIME;


typedef enum
{
    TIMEUNIT_year       = 0,
    TIMEUNIT_month,
    TIMEUNIT_day,
    TIMEUNIT_hour,
    TIMEUNIT_minute,
    TIMEUNIT_second,
    TIMEUNIT_msec,
    TIMEUNIT_usec,
    TIMEUNIT_nsec,
} TIMEUNIT;

typedef void (*PFNA7ITIMERCB)(PVOID, UINT ticks);


// ***************************************************************************
// STRUCTURE
//      AXCLOCKDATA
// PURPOSE
//      Device control structure
// ***************************************************************************
typedef struct __tag_AXCLOCKDATA
{
    AXCLOCK         axvalue;
    ULONG           sysvalue;
} AXCLOCKDATA, * PAXCLOCKDATA;

// ***************************************************************************
// STRUCTURE
//      AXDATE
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_AXDATE
{
    U16                  year;             // 0 - 65535
    U16                  month;            // 1 - 12
    U16                  day_of_week;      // 1 - 7
    U16                  day;              // 1 - 31
    U16                  hour;             // 0 - 23
    U16                  minute;           // 0 - 59
    U16                  second;           // 0 - 59
    U16                  milliseconds;     // 0 - 99
} AXDATE, * PAXDATE;


// --------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif                                      //  #ifdef __cplusplus


void                _axtime_init                ();
void                _axtime_deinit              ();

AXTIME              axtime_get_monotonic        ();

AXUTIME             axutime_get_monotonic       ();

AXUTIME             axutime_get                 ();

void                axsleep                     (UINT               mseconds);

void                axdelay                     (UINT               mseconds);

void                axudelay                    (UINT               useconds);

BOOL                axdate_is_correct           (PAXDATE            date);

BOOL                axtime_year_is_leap         (UINT               year);

PSTR                axtime_month_name           (UINT               month);

PSTR                axtime_month_short_name     (UINT               month);

BOOL                axdate_is_come              (PAXDATE            pst_time);

BOOL                axdate_time_is_between      (PAXDATE            now,
                                                 PAXDATE            begin,
                                                 PAXDATE            end);

void                axdate_add_ex               (PAXDATE            date,
                                                 PAXDATE            to_add,
                                                 UINT               d_add_seconds);

void                axdate_add_secs             (PAXDATE            pst_time,
                                                 UINT               d_secs_to_add);

void                axdate_get                  (PAXDATE            pst_time);

void                axdate_get_local            (PAXDATE            pst_time);

AXUTIME             axdate_to_utime             (PAXDATE            pst_time);

BOOL                axdate_from_utime           (PAXDATE            pst_date,
                                                 AXUTIME            time);

BOOL                axdate_set_rtc_x            (PSTR               psz_dev,
                                                 PAXDATE            pst_time);

ULONG               axdate_seconds_since        (UINT               since_year,
                                                 PAXDATE            date);


BOOL                axutime_local2gm            (AXUTIME *          pv_time);

AXUTIME             axutime_gm2local            (AXUTIME            time);

BOOL                axdate_set_rtc              (PAXDATE            pst_time);

BOOL                axdate_set                  (PAXDATE            pst_time);


void                axsleep                     (UINT               d_mseconds);

ULONG               axclock_get_ticks_per_sec   ();

ULONG               axclock                     ();

ULONG               axrandom                    ();


BOOL                axitimer_set                (PFNA7ITIMERCB      pfn_cb,
                                                 PVOID              p_ptr,
                                                 UINT               u_sec,
                                                 UINT               u_usec);

BOOL                axitimer_stop               (void);


#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus

#endif                                      //  #ifndef __AXTIMEH__


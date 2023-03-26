#include <linux/rtc.h>

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


// ***************************************************************************
// FUNCTION
//      a7time_set_rtc
// PURPOSE
//
// PARAMETERS
//      PAXDATE pst_time --
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
BOOL axdate_set_rtc_x(PSTR      psz_dev,
                      PAXDATE   pst_time)
{
    BOOL                b_result        = false;
    int                 fd_rtc;
    struct rtc_time     st_rtc_time;

    if (pst_time)
    {
        if ((fd_rtc = open(psz_dev, O_RDONLY)) != -1)
        {
            memset(&st_rtc_time, 0, sizeof(struct rtc_time));
            st_rtc_time.tm_hour     = pst_time->hour;
            st_rtc_time.tm_min      = pst_time->minute;
            st_rtc_time.tm_sec      = pst_time->second;
            st_rtc_time.tm_mday     = pst_time->day;
            st_rtc_time.tm_mon      = pst_time->month   - 1;
            st_rtc_time.tm_year     = pst_time->year    - 1900;

            b_result = (BOOL)(ioctl(fd_rtc, RTC_SET_TIME, &st_rtc_time) != -1);

            close(fd_rtc);
        }
    }

    return b_result;
}
// ***************************************************************************
// FUNCTION
//      axdate_set_rtc
// PURPOSE
//
// PARAMETERS
//      PAXDATE pst_time --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL axdate_set_rtc(PAXDATE pst_time)
{
    BOOL            b_result        = false;

    ENTER(true);

    b_result = axdate_set_rtc_x("/dev/rtc", pst_time);

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axdate_set
// PURPOSE
//
// PARAMETERS
//      PAXDATE pst_time --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL axdate_set(PAXDATE pst_time)
{
    BOOL                b_result        = false;
    time_t              v_time;

    if (pst_time)
    {
        v_time      = axdate_to_utime(pst_time);
        b_result    = (BOOL)!stime(&v_time);
    }

    return b_result;
}

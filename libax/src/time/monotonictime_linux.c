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

#include <time.h>
#include <sys/time.h>

#include <axtime.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      axutime_get_monotonic
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//      AXUTIME  --
// ***************************************************************************
AXUTIME axutime_get_monotonic(void)
{
    AXUTIME          v_time        = 0;

#ifdef A7OPT_DONTUSELIBRT
    struct timeval      tm;
    struct timezone     zn;
    gettimeofday(&tm, &zn);
#else
    struct timespec     tm;
    clock_gettime(CLOCK_MONOTONIC_RAW, &tm);
#endif

    v_time = tm.tv_sec;

    return v_time;
}
AXTIME axtime_get_monotonic(void)
{
    AXTIME         v_time        = 0;

#ifdef A7OPT_DONTUSELIBRT
    struct timeval      tm;
    gettimeofday(&tm, NULL);
#define USEC (tm.tv_usec / 1000)
#else
    struct timespec     tm;
    clock_gettime(CLOCK_MONOTONIC_RAW, &tm);
#define USEC (tm.tv_nsec / 1000000)
#endif

//printf("axtime_get_monotonic_x stage %u %lu\n", tm.tv_sec, tm.tv_nsec);
    v_time  = tm.tv_sec;
    v_time *= 1000;
    v_time += USEC;

    return v_time;
}

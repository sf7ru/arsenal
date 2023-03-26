
#include <arsenal.h>
#include <axtime.h>
#include <time.h>
#include <string.h>
#include <FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <task.h>


// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

extern volatile U64 g_ax_system_uptime_ms;
//extern U64          g_ax_MS_PER_TICK;


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

extern time_t       axtimegm                    (struct tm *        tm);


// ***************************************************************************
// FUNCTION
//      vApplicationTickHook
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      void --
// ***************************************************************************
void vApplicationTickHook( void )
{
    //sys_watchdog_feed();

     /* If FreeRTOS is running, user should use a dedicated task to call mesh service,
      * so we will not call it if freertos is running
      */
     //if (taskSCHEDULER_RUNNING == xTaskGetSchedulerState())
     {
         g_ax_system_uptime_ms += (1000 / configTICK_RATE_HZ);

// IncTick MUST be in primery irq handler, this hook call when task switching is active only
//#if (TARGET_ARCH == __AX_stm32__)
//         HAL_IncTick();
//#endif

         /* We don't need RIT if FreeRTOS is running */
//         if (sys_rit_running()) {
//             sys_rit_disable();

             /* The timer value so far may be an odd number, and if MS_PER_TICK() is not 1
              * then we may increment it like 12, 22, 32, etc. so round this number once.
              */
//         g_a9_system_uptime_ms = (g_a9_system_uptime_ms / 10) * 10;
//         }
     }
//     else
//     {
//         g_a9_system_uptime_ms += g_a9_time_per_rit_isr_ms;
//
//         /**
//          * Small hack to support interrupts if FreeRTOS is not running :
//          * FreeRTOS API resets our base priority register, then all
//          * interrupts higher priority than IP_SYSCALL will not get locked out.
//          *   @see more notes at isr_priorities.h.  @see IP_SYSCALL
//          */
//         __set_BASEPRI(0);
//     }
}
// ***************************************************************************
// FUNCTION
//      axdate_to_utime
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

    result      = axtimegm(&tm);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      axsleep
// PURPOSE
//
// PARAMETERS
//      UINT to --
// RESULT
//      void --
// ***************************************************************************
void axsleep(UINT to)
{
    if (taskSCHEDULER_RUNNING == xTaskGetSchedulerState())
    {
        vTaskDelay(to);
    }
    else
        axdelay(to);
}
// ***************************************************************************
// FUNCTION
//      axclock
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      ULONG --
// ***************************************************************************
ULONG axclock()
{
    return xTaskGetTickCount();
}
// ***************************************************************************
// FUNCTION
//      axclock_get_ticks_per_sec
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      ULONG --
// ***************************************************************************
ULONG axclock_get_ticks_per_sec()
{
    return configTICK_RATE_HZ;
}
// ***************************************************************************
// FUNCTION
//      axutime_get
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      AXUTIME --
// ***************************************************************************
AXUTIME axutime_get()
{
    return 0;
}
void axdate_get(PAXDATE     pst_time)
{
}

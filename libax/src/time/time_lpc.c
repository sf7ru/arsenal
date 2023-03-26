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

#include <arsenal.h>

#include <chip.h>
#include <lpc_utilities.h>

#include <FreeRTOSConfig.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      _axtime_configure_timer_for_run
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      void --
// ***************************************************************************
void _axtime_init(void)
{
    LPC_TIMER0->TCR = 1;  // Enable Timer
    LPC_TIMER0->CTCR = 0; // Increment on PCLK

    // Set CPU / 1 PCLK
    lpc_pclk(SYSCTL_PCLK_TIMER0, SYSCTL_CLKDIV_1);


    /**
     * Enable resolution of 2 uS per timer tick.
     * This provides 2uS * 2^32 = 140 minutes of runtime before counter over-flows
     */
    LPC_TIMER0->PR = (SystemCoreClock * TIMER0_US_PER_TICK) / (1000 * 1000);
}
// ***************************************************************************
// FUNCTION
//      _axtime_configure_timer_for_run
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      void --
// ***************************************************************************
void _axtime_deinit(void)
{
    // Reset timer service
    LPC_TIMER0->TCR = 0;
    LPC_TIMER0->CTCR = 0;
    LPC_SYSCON->PCLKSEL[0] &= ~(3 << 2);
    LPC_TIMER0->PR = 0;

}
// ***************************************************************************
// FUNCTION
//      axudelay
// PURPOSE
//
// PARAMETERS
//      UINT useconds --
// RESULT
//      void --
// ***************************************************************************
void axudelay(UINT               useconds)
{
    const unsigned int ticksForThisDelay = useconds / TIMER0_US_PER_TICK;
    const unsigned int targetTick = LPC_TIMER0->TC + ticksForThisDelay;

    // Take care of TC overflow case by allowing TC value to go beyond
    // targetTick, and then the next loop will handle the rest.
    if(targetTick <= LPC_TIMER0->TC)
    {
        while(LPC_TIMER0->TC > targetTick)
        {
            ;
        }
    }

    // Wait until the TC value reaches targetTick
    while(LPC_TIMER0->TC < targetTick)
    {
        ;
    }
}
// ***************************************************************************
// FUNCTION
//      axdelay
// PURPOSE
//
// PARAMETERS
//      UINT to --
// RESULT
//      void --
// ***************************************************************************
void axdelay(UINT to)
{
    axudelay(1000 * to);
}


#include <chip.h>


// ***************************************************************************
// FUNCTION
//      lpc_pclk
// PURPOSE
//
// PARAMETERS
//      CHIP_SYSCTL_PCLK_T   peripheral --
//      CHIP_SYSCTL_CLKDIV_T divider    --
// RESULT
//      void --
// ***************************************************************************
void lpc_pclk(CHIP_SYSCTL_PCLK_T        peripheral,
              CHIP_SYSCTL_CLKDIV_T      divider)
{
    /**
     * This is a quick and dirty trick to use uint64_t such that we don't have to
     * use if/else statements to pick either PCLKSEL0 or PCLKSEL1 register.
     */
    uint64_t *pclk_ptr = (uint64_t*)  &(LPC_SYSCTL->PCLKSEL[0]);
    const uint32_t bitpos = ((uint32_t)peripheral * 2);
    const uint32_t b11 = 3; // 0b11 in binary = 3

    *pclk_ptr &= ~((uint64_t)b11 << bitpos);
    *pclk_ptr |=  ((uint64_t)(divider & b11) << bitpos);
}

void lpc_pconp(CHIP_SYSCTL_CLOCK_T peripheral, bool on)
{
    if (on) {
        LPC_SYSCTL->PCONP |= (1 << peripheral);
    }
    else {
        LPC_SYSCTL->PCONP &= ~(1 << peripheral);
    }
}

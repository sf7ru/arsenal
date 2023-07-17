#include <arsenal.h>
#include <HalPlatformDefs.h>

#ifdef PLATFORM_STM32F4XX
#define LOOPS_1US (HSE_VALUE / 1000000UL)
#define KOEF            1.3
#else
#define LOOPS_1US (HSE_VALUE / 4000000UL)
#define KOEF            1.0
#endif

#pragma GCC push_options
#pragma GCC optimize ("O0")
void axudelay(UINT to)
{
    uint32_t cd = (to * LOOPS_1US * KOEF);

    if (cd > 10)
    {
        cd -= 10;
    }

    while (cd--)
    {
        __ASM volatile ("NOP");
        __ASM volatile ("NOP");
        __ASM volatile ("NOP");
    }
}
#pragma GCC pop_options

//static char initialized = 0;
//
//uint32_t DWT_Delay_Init(void) {
//    /* Disable TRC */
//    CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk; // ~0x01000000;
//    /* Enable TRC */
//    CoreDebug->DEMCR |=  CoreDebug_DEMCR_TRCENA_Msk; // 0x01000000;
//
//    /* Disable clock cycle counter */
//    DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //~0x00000001;
//    /* Enable  clock cycle counter */
//    DWT->CTRL |=  DWT_CTRL_CYCCNTENA_Msk; //0x00000001;
//
//    /* Reset the clock cycle counter value */
//    DWT->CYCCNT = 0;
//
//    /* 3 NO OPERATION instructions */
//    __ASM volatile ("NOP");
//    __ASM volatile ("NOP");
//    __ASM volatile ("NOP");
//
//    // Check if clock cycle counter has started
//    if(DWT->CYCCNT)
//    {
//        return 0; //clock cycle counter started
//    }
//    else
//    {
//        return 1; //clock cycle counter not started
//    }
//}
//
//
//void axudelay(UINT microseconds)
//{
//    uint32_t clk_cycle_start = DWT->CYCCNT;
//
//    if (!initialized)
//    {
//        DWT_Delay_Init();
//        initialized = 1;
//    }
//
//    /* Go to number of cycles for system */
//    microseconds *= (HAL_RCC_GetHCLKFreq() / 1000000);
//
//    /* Delay till end */
//    while ((DWT->CYCCNT - clk_cycle_start) < microseconds);
////    {
////        __ASM volatile ("NOP");
////    }
//}


void axdelay(UINT to)
{
    axudelay(to * 1000);
}

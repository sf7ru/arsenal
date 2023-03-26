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
#include <serial.h>
#include <axtime.h>

#include <sys_config.h>  // sys_get_cpu_clock()
//#include <lpc_utilities.h>

#include <chip.h>

#include <pvt_lpc.h>
#include <uart_17xx_40xx.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define REQBASE(dev) *(LPC_USART_T **)((dev) + 1)

#define UART_MCR_RTSEN_MASK     (1 << 6)
#define UART_MCR_CTSEN_MASK     (1 << 7)
#define UART_MCR_FLOWCTRL_MASK  (UART_MCR_RTSEN_MASK | UART_MCR_CTSEN_MASK)

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------


        PVOID               g_lpc_serial_devs[ SERIAL_DEVS_MAX ] = { 0, 0, 0, 0 };

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

extern void         lpc_pclk                    (CHIP_SYSCTL_PCLK_T peripheral,
                                                 CHIP_SYSCTL_CLKDIV_T divider);

extern BOOL         _serial_set_baudrate        (LPC_USART_T * reqBase,
                                                 UINT          baudRate);

extern void         UART0_IRQHandler            ();
extern void         UART1_IRQHandler            ();
extern void         UART2_IRQHandler            ();
extern void         UART3_IRQHandler            ();

// ***************************************************************************
// FUNCTION
//      serial_baud
// PURPOSE
//
// PARAMETERS
//      LPC_USART_T * uart     --
//      int           baudrate --
// RESULT
//      void --
// ***************************************************************************
void serial_baud(LPC_USART_T *   uart,
                 int             baudrate)
{

    // The LPC2300 and LPC1700 have a divider and a fractional divider to control the
    // baud rate. The formula is:
    //
    // Baudrate = (1 / PCLK) * 16 * DL * (1 + DivAddVal / MulVal)
    //   where:
    //     1 < MulVal <= 15
    //     0 <= DivAddVal < 14
    //     DivAddVal < MulVal
    //
    // set pclk to /1
    switch ((int)uart) {
        case (int)LPC_UART0: LPC_SYSCON->PCLKSEL[0] &= ~(0x3 <<  6); LPC_SYSCON->PCLKSEL[0] |= (0x1 <<  6); break;
        case (int)LPC_UART1: LPC_SYSCON->PCLKSEL[0] &= ~(0x3 <<  8); LPC_SYSCON->PCLKSEL[0] |= (0x1 <<  8); break;
        case (int)LPC_UART2: LPC_SYSCON->PCLKSEL[1] &= ~(0x3 << 16); LPC_SYSCON->PCLKSEL[1] |= (0x1 << 16); break;
        case (int)LPC_UART3: LPC_SYSCON->PCLKSEL[1] &= ~(0x3 << 18); LPC_SYSCON->PCLKSEL[1] |= (0x1 << 18); break;
        default: break;
    }

    uint32_t PCLK = SystemCoreClock;

    //printf("SystemCoreClock = %u\n", SystemCoreClock);

    // First we check to see if the basic divide with no DivAddVal/MulVal
    // ratio gives us an integer result. If it does, we set DivAddVal = 0,
    // MulVal = 1. Otherwise, we search the valid ratio value range to find
    // the closest match. This could be more elegant, using search methods
    // and/or lookup tables, but the brute force method is not that much
    // slower, and is more maintainable.
    uint16_t DL = PCLK / (16 * baudrate);

    uint8_t DivAddVal = 0;
    uint8_t MulVal = 1;
    int hit = 0;
    uint16_t dlv;
    uint8_t mv, dav;
    if ((PCLK % (16 * baudrate)) != 0) {     // Checking for zero remainder
        int err_best = baudrate, b;
        for (mv = 1; mv < 16 && !hit; mv++)
        {
            for (dav = 0; dav < mv; dav++)
            {
                // baudrate = PCLK / (16 * dlv * (1 + (DivAdd / Mul))
                // solving for dlv, we get dlv = mul * PCLK / (16 * baudrate * (divadd + mul))
                // mul has 4 bits, PCLK has 27 so we have 1 bit headroom which can be used for rounding
                // for many values of mul and PCLK we have 2 or more bits of headroom which can be used to improve precision
                // note: X / 32 doesn't round correctly. Instead, we use ((X / 16) + 1) / 2 for correct rounding

                if ((mv * PCLK * 2) & 0x80000000) // 1 bit headroom
                    dlv = ((((2 * mv * PCLK) / (baudrate * (dav + mv))) / 16) + 1) / 2;
                else // 2 bits headroom, use more precision
                    dlv = ((((4 * mv * PCLK) / (baudrate * (dav + mv))) / 32) + 1) / 2;

                // datasheet says if DLL==DLM==0, then 1 is used instead since divide by zero is ungood
                if (dlv == 0)
                    dlv = 1;

                // datasheet says if dav > 0 then DL must be >= 2
                if ((dav > 0) && (dlv < 2))
                    dlv = 2;

                // integer rearrangement of the baudrate equation (with rounding)
                b = ((PCLK * mv / (dlv * (dav + mv) * 8)) + 1) / 2;

                // check to see how we went
                b = abs(b - baudrate);
                if (b < err_best)
                {
                    err_best  = b;

                    DL        = dlv;
                    MulVal    = mv;
                    DivAddVal = dav;

                    if (b == baudrate)
                    {
                        hit = 1;
                        break;
                    }
                }
            }
        }
    }

    // set LCR[DLAB] to enable writing to divider registers
    uart->LCR |= (1 << 7);

    // set divider values
    uart->DLM = (DL >> 8) & 0xFF;
    uart->DLL = (DL >> 0) & 0xFF;
    uart->FDR = (uint32_t) DivAddVal << 0
                   | (uint32_t) MulVal    << 4;

    // clear LCR[DLAB]
    uart->LCR &= ~(1 << 7);
}
// ***************************************************************************
// FUNCTION
//      serial_format
// PURPOSE
//
// PARAMETERS
//      LPC_USART_T * uart      --
//      int           data_bits --
//      SERIALPARITY  parity    --
//      int           stop_bits --
// RESULT
//      void --
// ***************************************************************************
int serial_format(PSERIALPROPS p)
{
    int parity_enable, parity_select;

    int stop_bits  = p->u_stop;
    int data_bits  = p->u_bits - 1;

    switch (p->b_parity)
    {
        case SERIALPARITY_none:     parity_enable = 0; parity_select = 0; break;
        case SERIALPARITY_odd :     parity_enable = 1; parity_select = 0; break;
        case SERIALPARITY_even:     parity_enable = 1; parity_select = 1; break;
//        case SERIALPARITY_forced1:  parity_enable = 1; parity_select = 2; break;
//        case SERIALPARITY_forced:   parity_enable = 1; parity_select = 3; break;

        default:
            parity_enable = 0, parity_select = 0;
            break;
    }

    return data_bits            << 0
                   | stop_bits            << 2
                   | parity_enable        << 3
                   | parity_select        << 4;
}
//// ***************************************************************************
//// FUNCTION
////      serial_set_flow_control
//// PURPOSE
////
//// PARAMETERS
////      LPC_USART_T * uart --
////      SERIALFLOW    flow --
//// RESULT
////      void --
//// ***************************************************************************
//void serial_set_flow_control(LPC_USART_T * uart, SERIALFLOW flow)
//{
//    // Only UART1 has hardware flow control on LPC176x
//    LPC_USART_T * uart1 = (uint32_t)uart == (uint32_t) LPC_UART1 ? LPC_UART1 : NULL;
//    //int index = obj->index;
//
//    // First, disable flow control completely
//    if (uart1)
//        uart1->MCR = uart1->MCR & ~UART_MCR_FLOWCTRL_MASK;
//
//    switch (flow)
//    {
//        case SERIALFLOW_rts:
//            break;
//
//        default:
//            break;
//    }
//}
// ***************************************************************************
// FUNCTION
//      _serial_open
// PURPOSE
//
// PARAMETERS
//      PSERIALPROPS p    --
//      UINT         type --
// RESULT
//      PVOID --
// ***************************************************************************
PVOID _serial_open(PSERIALPROPS p,
                   UINT         type,
                   PUINT        irqn)
{
    LPC_USART_T *   result          = nil;

    ENTER(true);

    switch (p->u_port)
    {
        case 0:
            // pin P0.2(98) P0.3(99)
            Chip_IOCON_PinMux(LPC_IOCON, 0, 2, IOCON_MODE_PULLUP, IOCON_FUNC1);
            Chip_IOCON_PinMux(LPC_IOCON, 0, 3, IOCON_MODE_PULLUP, IOCON_FUNC1);

            result              = LPC_UART0;
            *irqn               = UART0_IRQn;
            lpc_pclk(SYSCTL_PCLK_UART0, SYSCTL_CLKDIV_1);
            LPC_SYSCON->PCONP  |= (1 << SYSCTL_CLOCK_UART0);
            break;

        case 1:
            if (type == SERIALTYPE_secondary)
            {
                // pin P2.1(74) P2.0(75)
                Chip_IOCON_PinMux(LPC_IOCON, 2,  1, IOCON_MODE_PULLUP, IOCON_FUNC2);
                Chip_IOCON_PinMux(LPC_IOCON, 2,  0, IOCON_MODE_PULLUP, IOCON_FUNC2);

                // Disconnect others
                Chip_IOCON_PinMux(LPC_IOCON, 0,  15, IOCON_MODE_PULLUP, IOCON_FUNC0);
                Chip_IOCON_PinMux(LPC_IOCON, 0,  16, IOCON_MODE_PULLUP, IOCON_FUNC0);
            }
            else
            {
                // pin P0.15(62) P0.16(63)
                Chip_IOCON_PinMux(LPC_IOCON, 0,  15, IOCON_MODE_PULLUP, IOCON_FUNC1);
                Chip_IOCON_PinMux(LPC_IOCON, 0,  16, IOCON_MODE_PULLUP, IOCON_FUNC1);

                // Disconnect others
                Chip_IOCON_PinMux(LPC_IOCON, 2,  1, IOCON_MODE_PULLUP, IOCON_FUNC0);
                Chip_IOCON_PinMux(LPC_IOCON, 2,  0, IOCON_MODE_PULLUP, IOCON_FUNC0);
            }

            result              = LPC_UART1;
            *irqn               = UART1_IRQn;
            lpc_pclk(SYSCTL_PCLK_UART1, SYSCTL_CLKDIV_1);
            LPC_SYSCON->PCONP  |= (1 << SYSCTL_CLOCK_UART1);
            break;

        case 2:
            if (type == SERIALTYPE_secondary)
            {
                // pin P2.9(64) P2.8(65)
                Chip_IOCON_PinMux(LPC_IOCON, 2,  9, IOCON_MODE_PULLUP, IOCON_FUNC2);
                Chip_IOCON_PinMux(LPC_IOCON, 2,  8, IOCON_MODE_PULLUP, IOCON_FUNC2);

                // Disconnect others
                Chip_IOCON_PinMux(LPC_IOCON, 0, 10, IOCON_MODE_PULLUP, IOCON_FUNC0);
                Chip_IOCON_PinMux(LPC_IOCON, 0, 11, IOCON_MODE_PULLUP, IOCON_FUNC0);
            }
            else
            {
                // pin P0.10(48) P0.11(49)
                Chip_IOCON_PinMux(LPC_IOCON, 0, 10, IOCON_MODE_PULLUP, IOCON_FUNC1);
                Chip_IOCON_PinMux(LPC_IOCON, 0, 11, IOCON_MODE_PULLUP, IOCON_FUNC1);

                // Disconnect others
                Chip_IOCON_PinMux(LPC_IOCON, 2,  9, IOCON_MODE_PULLUP, IOCON_FUNC0);
                Chip_IOCON_PinMux(LPC_IOCON, 2,  8, IOCON_MODE_PULLUP, IOCON_FUNC0);
            }

            result              = LPC_UART2;
            *irqn               = UART2_IRQn;
            lpc_pclk(SYSCTL_PCLK_UART2, SYSCTL_CLKDIV_1);
            LPC_SYSCON->PCONP  |= (1 << SYSCTL_CLOCK_UART2);
            break;

        case 3:
            if (type == SERIALTYPE_primary)
            {
                // pin P0.0(46) P0.1(47)
                Chip_IOCON_PinMux(LPC_IOCON, 0, 0, IOCON_MODE_PULLUP, IOCON_FUNC2); // TXD3
                Chip_IOCON_PinMux(LPC_IOCON, 0, 1, IOCON_MODE_PULLUP, IOCON_FUNC2); // RXD3

                // Disconnect others
                Chip_IOCON_PinMux(LPC_IOCON, 0, 25, IOCON_MODE_PULLUP, IOCON_FUNC0); // TXD3
                Chip_IOCON_PinMux(LPC_IOCON, 0, 26, IOCON_MODE_PULLUP, IOCON_FUNC0); // RXD3
                Chip_IOCON_PinMux(LPC_IOCON, 4, 28, IOCON_MODE_PULLUP, IOCON_FUNC0); // TXD3
                Chip_IOCON_PinMux(LPC_IOCON, 4, 29, IOCON_MODE_PULLUP, IOCON_FUNC0); // RXD3
            }
            else if (type == SERIALTYPE_secondary)
            {
                // pin P0.25(7) P0.26(6)
                Chip_IOCON_PinMux(LPC_IOCON, 0, 25, IOCON_MODE_PULLUP, IOCON_FUNC3); // TXD3
                Chip_IOCON_PinMux(LPC_IOCON, 0, 26, IOCON_MODE_PULLUP, IOCON_FUNC3); // RXD3

                // Disconnect others
                Chip_IOCON_PinMux(LPC_IOCON, 0, 0, IOCON_MODE_PULLUP, IOCON_FUNC0); // TXD3
                Chip_IOCON_PinMux(LPC_IOCON, 0, 1, IOCON_MODE_PULLUP, IOCON_FUNC0); // RXD3
                Chip_IOCON_PinMux(LPC_IOCON, 4, 28, IOCON_MODE_PULLUP, IOCON_FUNC0); // TXD3
                Chip_IOCON_PinMux(LPC_IOCON, 4, 29, IOCON_MODE_PULLUP, IOCON_FUNC0); // RXD3
            }
            else
            {
                // pin P4.28(82) P4.29(85)
                Chip_IOCON_PinMux(LPC_IOCON, 4, 28, IOCON_MODE_PULLUP, IOCON_FUNC3); // TXD3
                Chip_IOCON_PinMux(LPC_IOCON, 4, 29, IOCON_MODE_PULLUP, IOCON_FUNC3); // RXD3

                // Disconnect others
                Chip_IOCON_PinMux(LPC_IOCON, 0, 0, IOCON_MODE_PULLUP, IOCON_FUNC0); // TXD3
                Chip_IOCON_PinMux(LPC_IOCON, 0, 1, IOCON_MODE_PULLUP, IOCON_FUNC0); // RXD3
                Chip_IOCON_PinMux(LPC_IOCON, 0, 25, IOCON_MODE_PULLUP, IOCON_FUNC0); // TXD3
                Chip_IOCON_PinMux(LPC_IOCON, 0, 26, IOCON_MODE_PULLUP, IOCON_FUNC0); // RXD3
            }

            result              = LPC_UART3;
            *irqn               = UART3_IRQn;
            lpc_pclk(SYSCTL_PCLK_UART3, SYSCTL_CLKDIV_1);
            LPC_SYSCON->PCONP  |= (1 << SYSCTL_CLOCK_UART3);
            break;

        default:
            break;
    }

    if (result)
    {
        Chip_UART_Init(result);
        _serial_set_baudrate(result, p->u_baudrate * 100);
        Chip_UART_ConfigData(result, serial_format(p));
        Chip_UART_SetupFIFOS(result, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV1));
        Chip_UART_TXEnable(result);
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      serial_close
// PURPOSE
//
// PARAMETERS
//      PAXDEV dev --
// RESULT
//      PAXDEV --
// ***************************************************************************
PAXDEV _serial_close(PAXDEV dev)
{
    int             i;
    LPC_USART_T *   reqBase     = REQBASE(dev);

    ENTER(true);

    for (i = 0; i < SERIAL_DEVS_MAX; i++)
    {
        if (g_lpc_serial_devs[i] == (PVOID)dev)
        {
            g_lpc_serial_devs[i] = nil;

            Chip_UART_DeInit(reqBase);
        }
    }

    FREE(dev);

    RETURN(nil);
}
// ***************************************************************************
// FUNCTION
//      serial_set_dtr
// PURPOSE
//
// PARAMETERS
//      PAXDEV dev   --
//      BOOL   value --
// RESULT
//      void --
// ***************************************************************************
void serial_set_dtr(PAXDEV             dev,
                    BOOL               value)
{

}
// ***************************************************************************
// FUNCTION
//      serial_set_rts
// PURPOSE
//
// PARAMETERS
//      PAXDEV dev   --
//      BOOL   value --
// RESULT
//      void --
// ***************************************************************************
void serial_set_rts(PAXDEV             dev,
                    BOOL               value)
{

}
// ***************************************************************************
// FUNCTION
//      serial_set_baudrate
// PURPOSE
//
// PARAMETERS
//      PAXDEV dev      --
//      UINT   baudRate --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL serial_set_baudrate(PAXDEV        dev,
                         UINT          baudRate)
{
    LPC_USART_T *   reqBase         = REQBASE(dev);

    return _serial_set_baudrate(reqBase, baudRate);
}

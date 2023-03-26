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

#include <chip.h>

#include <pvt_lpc.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define UART_SRB_SIZE 64    // Send
#define UART_RRB_SIZE 256   // Receive

#define HANDLE_IRQ(a)   void UART##a##_IRQHandler() { if (g_lpc_serial_devs[a] != nil)    {   \
                        Chip_UART_IRQRBHandler(LPC_UART##a, &g_lpc_serial_devs[a]->rxring, &g_lpc_serial_devs[a]->txring); } }


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

// ***************************************************************************
// STRUCTURE
//      SERIALDEV
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_SERIALDEV
{
    AXDEV               st_dev;
    LPC_USART_T *       regBase;
    RINGBUFF_T          txring;
    RINGBUFF_T          rxring;
    uint8_t             rxbuff      [ UART_RRB_SIZE ];
    uint8_t             txbuff      [ UART_SRB_SIZE ];
} SERIALDEV, * PSERIALDEV;

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

extern  PSERIALDEV          g_lpc_serial_devs[ SERIAL_DEVS_MAX ];

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

HANDLE_IRQ(0);
HANDLE_IRQ(1);
HANDLE_IRQ(2);
HANDLE_IRQ(3);


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
PAXDEV serial_close(PAXDEV dev)
{
    return _serial_close(dev);
}
// ***************************************************************************
// FUNCTION
//      serial_open
// PURPOSE
//
// PARAMETERS
//      PSERIALPROPS p    --
//      UINT         type --
// RESULT
//      PAXDEV --
// ***************************************************************************
PAXDEV serial_open(PSERIALPROPS p,
                   UINT         type)
{
    PSERIALDEV      result          = nil;

    ENTER(true);

    if ((p->u_port < SERIAL_DEVS_MAX) && (nil == g_lpc_serial_devs[p->u_port]))
    {
        if ((result = CREATE(SERIALDEV)) != NULL)
        {
            RingBuffer_Init(&result->rxring, result->rxbuff, 1, UART_RRB_SIZE);
            RingBuffer_Init(&result->txring, result->txbuff, 1, UART_SRB_SIZE);

            result->st_dev.pfn_close        = (PAXDEVFNCLOSE)_serial_close;
            result->st_dev.pfn_ctl          = (PAXDEVFNCTL)nil;
            result->st_dev.pfn_read         = (PAXDEVFNREAD)_serial_read;
            result->st_dev.pfn_write        = (PAXDEVFNWRITE)_serial_write;
            result->st_dev.pfn_accept       = (PAXDEVFNACCEPT)nil;
            g_lpc_serial_devs[p->u_port]    = result;

            if ((result->regBase = _serial_open(p, type)) != nil)
            {
                // Ok
            }
            else
            {
                SAFECLOSE2(serial, result, PAXDEV);
            }
        }
    }

    RETURN((PAXDEV)result);
}
// ***************************************************************************
// FUNCTION
//      _wait_while_empty
// PURPOSE
//
// PARAMETERS
//      RINGBUFF_T * buff --
//      UINT         TO   --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL _wait_while_full(RINGBUFF_T *        buff,
                      UINT                full,
                      UINT                TO)
{
    ULONG           cd              = TO;


    while (cd && (RingBuffer_GetCount(buff) == full))
    {
        axdelay(1);
        cd--;
    }

    return cd != 0;
}
// ***************************************************************************
// FUNCTION
//      _serial_write
// PURPOSE
//
// PARAMETERS
//      PAXDEV dev  --
//      PCVOID data --
//      INT    size --
//      UINT   TO   --
// RESULT
//      INT --
// ***************************************************************************
INT _serial_write(PAXDEV             dev,
                  PCVOID             data,
                  INT                size,
                  UINT               TO)
{
    PSERIALDEV      s               = (PSERIALDEV)dev;
    int             left            = size;
    PU8             on              = (PU8)data;
    int             rd;

    ENTER(true);

    while (left)
    {
        rd = MAC_MIN(UART_SRB_SIZE, left);

        if ((rd = Chip_UART_SendRB(s->regBase, &s->txring, on, rd)) == 0)
        {
            if (!_wait_while_full(&s->txring, UART_SRB_SIZE, TO))
            {
                break;
            }
        }
        else
        {
            left -= rd;
            on   += rd;
        }
    }

    RETURN(size - left);
}
// ***************************************************************************
// FUNCTION
//      _wait_while_empty
// PURPOSE
//
// PARAMETERS
//      RINGBUFF_T * buff --
//      UINT         TO   --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL _wait_while_empty(RINGBUFF_T *        buff,
                    UINT                TO)
{
    ULONG           cd              = TO;


    while (cd && (RingBuffer_GetCount(buff) == 0))
    {
        axdelay(1);
        cd--;
    }

    return cd != 0;
}
// ***************************************************************************
// FUNCTION
//
// PURPOSE
//
// PARAMETERS
//      PAXDEV dev  --
//      PVOID  data --
//      INT    size --
//      UINT   TO   --
// RESULT
//      INT _serial_read                --
// ***************************************************************************
INT _serial_read(PAXDEV             dev,
                 PVOID              data,
                 INT                size,
                 UINT               TO)
{
    INT             result          = 0;
    PSERIALDEV      s               = (PSERIALDEV)dev;
    INT             rd;

    ENTER(true);

    rd = MAC_MIN(size, UART_RRB_SIZE);

    if (!TO || _wait_while_empty(&s->rxring, TO))
    {
        result = Chip_UART_ReadRB(s->regBase, &s->rxring, data, rd);
    }

    RETURN(result);
}
BOOL _serial_set_baudrate(LPC_USART_T * reqBase,
                          UINT          baudRate)
{
    BOOL            result          = true;

    ENTER(true);

    Chip_UART_SetBaud(reqBase, baudRate);

    RETURN(result);
}

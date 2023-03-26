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
#include <pvt_stm32.h>
#include <axtime.h>


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
    PVOID               internal;

//    LPC_USART_T *       regBase;
//    RINGBUFF_T          txring;
//    RINGBUFF_T          rxring;
//    uint8_t             rxbuff      [ UART_RRB_SIZE ];
//    uint8_t             txbuff      [ UART_SRB_SIZE ];
} SERIALDEV, * PSERIALDEV;


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------


PAXDEV serial_open(PSERIALPROPS p,
                   UINT         type)
{
    PSERIALDEV      result          = nil;

    ENTER(true);

    if ((p->u_port < SERIAL_DEVS_MAX) )
    {
        if ((result = CREATE(SERIALDEV)) != NULL)
        {
            if ((result->internal = _serial_open(p, type, 0)) != nil)
            {
                result->st_dev.pfn_close        = (PAXDEVFNCLOSE)serial_close;
                result->st_dev.pfn_ctl          = (PAXDEVFNCTL)nil;
                result->st_dev.pfn_read         = (PAXDEVFNREAD)_serial_read;
                result->st_dev.pfn_write        = (PAXDEVFNWRITE)_serial_write;
                result->st_dev.pfn_accept       = (PAXDEVFNACCEPT)nil;
            }
            else
            {
                SAFECLOSE2(serial, result, PAXDEV);
            }
        }
    }

    RETURN((PAXDEV)result);
}
PAXDEV serial_close(PAXDEV dev)
{
    PSERIALDEV      d = (PSERIALDEV)dev;

    ENTER(true);

    _serial_close(d->internal);

    FREE(dev);

    RETURN(nil);
}
INT _serial_write(PAXDEV             dev,
                  PCVOID             data,
                  INT                size,
                  UINT               TO)
{
    return __serial_write(((PSERIALDEV)dev)->internal, data, size, TO);
}
INT _serial_read(PAXDEV             dev,
                 PVOID              data,
                 INT                size,
                 UINT               TO)
{
    return __serial_read(((PSERIALDEV)dev)->internal, data, size, TO);
}

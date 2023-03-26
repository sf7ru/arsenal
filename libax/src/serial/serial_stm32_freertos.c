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
#include <stdint.h>


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


//typedef struct {
//    uint8_t usart;
//    uint8_t byte;
//} myMes;


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

//osMessageQId RecQHandle;
//
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
//{
//    myMes m;
//
//    switch((uint32_t)UartHandle->Instance)
//    {
//        case (uint32_t)USART1:
//            //HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_10);
//            m.usart=1;
//            m.byte=b1;
//            xQueueSend( RecQHandle, &m, portMAX_DELAY  );
//            // Do this need?
//            //HAL_UART_Receive_IT(&huart1, &b1,1);
//            break;
//        case (uint32_t)USART2:
//            HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_12);
//            m.usart=2;
//            m.byte=b2;
//            xQueueSend( RecQHandle, &m, portMAX_DELAY  );
//            HAL_UART_Receive_IT(&huart2, &b2,1);
//            break;
//    }
//}

INT serial_ctl(PAXDEV             dev,
               UINT               fn,
               PVOID              pparam,
               UINT               iparam)
{
    INT         result          = -1;

    ENTER(true);

    switch (fn)
    {
        case AXDEVCTL_GETFD:
            result = __serial_get_fd(((PSERIALDEV)dev)->internal);
            break;
    }

    RETURN(result);
}


PAXDEV serial_open(PSERIALPROPS p,
                   UINT         type)
{
    PSERIALDEV      result          = nil;

    ENTER(true);

    if ((p->u_port < SERIAL_DEVS_MAX) )// && (nil == g_lpc_serial_devs[p->u_port]))
    {
        if ((result = CREATE(SERIALDEV)) != NULL)
        {
            if ((result->internal = _serial_open(p, type, 0)) != nil)
            {
//            RingBuffer_Init(&result->rxring, result->rxbuff, 1, UART_RRB_SIZE);
//            RingBuffer_Init(&result->txring, result->txbuff, 1, UART_SRB_SIZE);

                result->st_dev.pfn_close        = (PAXDEVFNCLOSE)serial_close;
                result->st_dev.pfn_ctl          = (PAXDEVFNCTL)serial_ctl;
                result->st_dev.pfn_read         = (PAXDEVFNREAD)_serial_read;
                result->st_dev.pfn_write        = (PAXDEVFNWRITE)_serial_write;
                result->st_dev.pfn_accept       = (PAXDEVFNACCEPT)nil;
//                g_lpc_serial_devs[p->u_port]    = result;

//            if ((result->regBase = _serial_open(p, type)) != nil)
//            {
//                // Ok
//            }
//            else
//            {
//                SAFECLOSE2(serial, result, PAXDEV);
//            }
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
__attribute__((weak)) int __serial_get_pending_tx()
{
    return 0;
}
INT _serial_get_pending_tx(PAXDEV             dev)
{
    return __serial_get_pending_tx(((PSERIALDEV)dev)->internal);
}

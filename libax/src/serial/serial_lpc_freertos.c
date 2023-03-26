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
#include <lpc_utilities.h>

#include <chip.h>
#include <FreeRTOS.h>
#include <queue.h>
#include "task.h"
#include <semphr.h>

#include <pvt_lpc.h>

#include <customboard.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#ifndef CONF_UART_RX_BUFF_SIZE
#define CONF_UART_RX_BUFF_SIZE      2048
#endif

#ifndef CONF_UART_TX_BUFF_SIZE
#define CONF_UART_TX_BUFF_SIZE      128
#endif

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

    xQueueHandle        txLCK;
    RINGBUFF_T          txring;
    xQueueHandle        rxLCK;
    RINGBUFF_T          rxring;

    UINT                port;
    UINT                irqn;

    uint8_t *           rxbuff;
    uint8_t *           txbuff;
} SERIALDEV, * PSERIALDEV;

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

extern  PSERIALDEV          g_lpc_serial_devs[ SERIAL_DEVS_MAX ];


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

extern void Chip_UART_ABIntHandler(LPC_USART_T *pUART);


// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      handleInterrupt
// PURPOSE
//
// PARAMETERS
//      PSERIALDEV dev --
// RESULT
//      long --
// ***************************************************************************
static void handleInterrupt(PSERIALDEV dev)
{
    __disable_irq();

    portBASE_TYPE woken = pdFALSE;

    /* Want to handle any errors? Do it here. */

    /* Handle transmit interrupt if enabled */
    if (dev->regBase->IER & UART_IER_THREINT)
    {
        //if (xSemaphoreTakeFromISR(dev->txLCK, &woken) != pdFALSE)
        {
            Chip_UART_TXIntHandlerRB(dev->regBase, &dev->txring);

            /* Disable transmit interrupt if the ring buffer is empty */
            if (RingBuffer_IsEmpty(&dev->txring))
            {
                Chip_UART_IntDisable(dev->regBase, UART_IER_THREINT);
            }

            //xSemaphoreGiveFromISR(dev->txLCK, &woken);
        }
    }

    /* Handle receive interrupt */

    /* New data will be ignored if data not popped in time */
    if (Chip_UART_ReadLineStatus(dev->regBase) & UART_LSR_RDR)
    {
        //if (xSemaphoreTakeFromISR(dev->rxLCK, &woken) != pdFALSE)
        {
            while (Chip_UART_ReadLineStatus(dev->regBase) & UART_LSR_RDR)
            {
                uint8_t ch = Chip_UART_ReadByte(dev->regBase);
                RingBuffer_Insert(&dev->rxring, &ch);
            }

            //xSemaphoreGiveFromISR(dev->rxLCK, &woken);
        }
    }

    /* Handle Autobaud interrupts */
    Chip_UART_ABIntHandler(dev->regBase);


    __enable_irq();

    portEND_SWITCHING_ISR(woken);
}
// ***************************************************************************
// FUNCTION
//      UART0_IRQHandler
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      void --
// ***************************************************************************
void UART0_IRQHandler()
{
    if (g_lpc_serial_devs[0])
        handleInterrupt(g_lpc_serial_devs[0]);
}
// ***************************************************************************
// FUNCTION
//      UART1_IRQHandler
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      void --
// ***************************************************************************
void UART1_IRQHandler()
{
    uint8_t IIRValue, LSRValue;
    uint8_t Dummy = Dummy;

    IIRValue = LPC_UART1->IIR;

    IIRValue &= 0x0E;            /* check bit 1~3, interrupt identification */

    if (IIRValue == UART_IIR_INTID_RLS)        /* Receive Line Status */
    {
        LSRValue = LPC_UART1->LSR;
        /* Receive Line Status */

        if ( LSRValue & (UART_LSR_OE|UART_LSR_PE|UART_LSR_FE|UART_LSR_RXFE|UART_LSR_BI) )
        {
            /* There are errors or break interrupt */
            /* Read LSR will clear the interrupt */
            //UART1Status = LSRValue;
            Dummy = LPC_UART1->RBR;        /* Dummy read on RX to clear
                                interrupt, then bail out */
            return;
        }

        if ( LSRValue & UART_LSR_RDR )    /* Receive Data Ready */
        {
            /* If no error on RLS, normal ready, save into the data buffer. */
            /* Note: read RBR will clear the interrupt */
//            UART1Buffer[UART1Count] = LPC_UART1->RBR;
//            UART1Count++;
//            if ( UART1Count == BUFSIZE )
//            {
//                UART1Count = 0;        /* buffer overflow */
//            }
        }
    }
//    else if ( IIRValue ==  UART_IIR_INTID_RDA )    /* Receive Data Available */
//    {
//        /* Receive Data Available */
////        UART1Buffer[UART1Count] = LPC_UART1->RBR;
////        UART1Count++;
////        if ( UART1Count == BUFSIZE )
////        {
////            UART1Count = 0;        /* buffer overflow */
////        }
//    }
//    else if ( IIRValue == IIR_CTI )    /* Character timeout indicator */
//    {
//        /* Character Time-out indicator */
//        UART1Status |= 0x100;        /* Bit 9 as the CTI error */
//    }
//    else if ( IIRValue == IIR_THRE )    /* THRE, transmit holding register empty */
//    {
//        /* THRE interrupt */
//        LSRValue = LPC_UART1->LSR;        /* Check status in the LSR to see if
//                                valid data in U0THR or not */
//        if ( LSRValue & LSR_THRE )
//        {
//            UART1TxEmpty = 1;
//        }
//        else
//        {
//            UART1TxEmpty = 0;
//        }
//    }


    if (g_lpc_serial_devs[1])
        handleInterrupt(g_lpc_serial_devs[1]);
}
// ***************************************************************************
// FUNCTION
//      UART2_IRQHandler
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      void --
// ***************************************************************************
void UART2_IRQHandler()
{
    if (g_lpc_serial_devs[2])
        handleInterrupt(g_lpc_serial_devs[2]);
}

// ***************************************************************************
// FUNCTION
//      UART3_IRQHandler
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      void --
// ***************************************************************************
void UART3_IRQHandler()
{
    if (g_lpc_serial_devs[3])
        handleInterrupt(g_lpc_serial_devs[3]);
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      setBaudRate
// PURPOSE
//
// PARAMETERS
//      PSERIALDEV   dev      --
//      unsigned int baudRate --
// RESULT
//      void --
// ***************************************************************************
//static void setBaudRate(PSERIALDEV      dev,
//                        unsigned int    baudRate)
//{
//    uint16_t bd;
//
//    dev->regBase->LCR = (1 << 7); // Enable DLAB to access DLM, DLL, and IER
//    {
//        bd = (SystemCoreClock / (16 * baudRate)) + 0.5;
//        dev->regBase->DLM = (bd >> 8);
//        dev->regBase->DLL = (bd >> 0);
//    }
//    dev->regBase->LCR = 3; // Disable DLAB and set 8bit per char
//}

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
    PSERIALDEV s = (PSERIALDEV)dev;

    NVIC_DisableIRQ(s->irqn);

    SAFEFREE(s->rxbuff);
    SAFEFREE(s->txbuff);

    if (s->txLCK)
        vSemaphoreDelete(s->txLCK);

    if (s->rxLCK)
        vSemaphoreDelete(s->rxLCK);

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
            result->port = p->u_port;

            if (  ((result->rxLCK = xSemaphoreCreateMutex()) != nil) &&
                  ((result->txLCK = xSemaphoreCreateMutex()) != nil) )
            {
                if ((result->regBase = (LPC_USART_T *)_serial_open(p, type, &result->irqn)) != nil)
                {
                    if (  ((result->rxbuff = MALLOC(CONF_UART_RX_BUFF_SIZE)) != nil)    &&
                          ((result->txbuff = MALLOC(CONF_UART_TX_BUFF_SIZE)) != nil)    )
                    {
                        RingBuffer_Init(&result->rxring, result->rxbuff, 1, CONF_UART_RX_BUFF_SIZE);
                        RingBuffer_Init(&result->txring, result->txbuff, 1, CONF_UART_TX_BUFF_SIZE);

                        Chip_UART_SetupFIFOS(result->regBase, (UART_FCR_FIFO_EN | UART_FCR_RX_RS |
                                                               UART_FCR_TX_RS | UART_FCR_TRG_LEV3));

                        Chip_UART_IntEnable(result->regBase, (UART_IER_RBRINT | UART_IER_RLSINT));
                        NVIC_SetPriority(result->irqn, 1);
                        NVIC_EnableIRQ(result->irqn);

                        result->st_dev.pfn_close  = (PAXDEVFNCLOSE) _serial_close;
                        result->st_dev.pfn_ctl    = (PAXDEVFNCTL) nil;
                        result->st_dev.pfn_read   = (PAXDEVFNREAD) _serial_read;
                        result->st_dev.pfn_write  = (PAXDEVFNWRITE) _serial_write;
                        result->st_dev.pfn_accept = (PAXDEVFNACCEPT) nil;
                        g_lpc_serial_devs[p->u_port] = result;
                    }
                    else
                    {
                        SAFECLOSE2(serial, result, PAXDEV);
                    }
                }
                else
                {
                    SAFECLOSE2(serial, result, PAXDEV);
                }
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
    INT             result          = 0;
    PSERIALDEV      s               = (PSERIALDEV)dev;
    INT             rd;
    UINT            countdown;


    ENTER(true);

    if (taskSCHEDULER_RUNNING == xTaskGetSchedulerState())
    {
        if (xSemaphoreTake(s->txLCK, TO) != pdFALSE)
        {
            countdown   = TO;
            INT left    = size;

            do
            {
                if ((rd = Chip_UART_SendRB(s->regBase, &s->txring, data, left)) > 0)
                {
                    result += rd;
                }
                else
                {
                    axsleep(1);
                    countdown--;
                }

            } while ((result < left) && countdown);

            while (countdown && !RingBuffer_IsEmpty(&s->txring))
            {
                axsleep(1);
                countdown--;
            }

            xSemaphoreGive(s->txLCK);
        }
    }
    else
    {
        TO          = (TO ? TO : 10) * 10;
        countdown   = TO;

        while (countdown && (result < size))
        {
            countdown = TO;

            while (((s->regBase->LSR & UART_LSR_THRE) == 0) && countdown)      // Block until tx empty
            {
                countdown--;
            }

            if (countdown)
            {
                s->regBase->THR = *(((PU8)data) + result);

                result++;
            }
        }
    }

    RETURN(result);
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
    INT             result          = -1;
    PSERIALDEV      s               = (PSERIALDEV)dev;

    ENTER(true);

    if (taskSCHEDULER_RUNNING == xTaskGetSchedulerState())
    {
        do
        {
            if (xSemaphoreTake(s->rxLCK, TO) != pdFALSE)
            {
                result = Chip_UART_ReadRB(s->regBase, &s->rxring, data, size);

                xSemaphoreGive(s->rxLCK);
            }
            else
                result = 0;

            if (!result && TO)
                axsleep(1);

        } while (!result && TO--);
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      _serial_set_baudrate
// PURPOSE
//
// PARAMETERS
//      LPC_USART_T * reqBase  --
//      UINT          baudRate --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL _serial_set_baudrate(LPC_USART_T * reqBase,
                          UINT          baudRate)
{
    BOOL            result          = true;

    ENTER(true);

    Chip_UART_SetBaudFDR(reqBase, baudRate);

    RETURN(result);
}

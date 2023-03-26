#include <Hal.h>
#include "FreeRTOS.h"
#include "task.h"
#include "app.h"

#include <Hal.h>

#include <Spi.h>
#include <SpiDevice.h>

#include <customboard.h>


// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define SSEL                hal.gpio.getPin(CBRD_SPI_SOFT_SSEL)
#define SCK                 hal.gpio.getPin(CBRD_SPI_SOFT_SCK)
#define MOSI                hal.gpio.getPin(CBRD_SPI_SOFT_MOSI)
#define SET_MISO(a)         hal.gpio.setPin(CBRD_SPI_SOFT_MISO, (a))

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct __tag_SWSPITCBLK
{
    PVOID                   bank;
    PU8                     send;
    PU8                     recv;
    int                     len;
    int                     result;
} SWSPITCBLK, * PSWSPITCBLK;

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------


#ifdef LPC176X_USE_SOFTWARE_SPI_IRQ
static  PSWSPITCBLK         irqTrx      = 0;
#endif

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

extern PVOID  Lpc176x_get_GPIO_by_port    (UINT               port);

void                SWSPI_transmit              (PSWSPITCBLK        b);

extern "C"
{
#ifdef LPC176X_USE_SOFTWARE_SPI_IRQ
    void SPI_IRQHandler(void)
    {
        if (irqTrx)
        {
            Chip_SPI_Int_Disable(LPC_SPI);

            SWSPI_transmit(irqTrx);

            irqTrx = nil;

            Chip_SPI_Int_Enable(LPC_SPI);
        }
    }
#endif // #ifndef LPC176X_USE_SOFTWARE_SPI_IRQ
}

// ***************************************************************************
// FUNCTION
//      SWSPI_init
// PURPOSE
//
// PARAMETERS
//      SpiDevice * dev    --
//      UINT        iface  --
//      BOOL        master --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL SWSPI_init(SpiDevice *        dev,
                PSPIMODE           mode)
{
    BOOL            result          = false;

    ENTER(true);

#ifdef CBRD_SPI_SOFT_SCK
    Hal &           hal             = Hal::getInstance();

    if (mode->master)
    {

    }
    else
    {
        printf("SW SPI init\n");

        //dev->control    = (PVOID)Lpc176x_get_GPIO_by_port(CBRD_SPI_SOFT_PORT);

        hal.gpio.setMode(CBRD_SPI_SOFT_SCK);
        hal.gpio.setMode(CBRD_SPI_SOFT_SSEL);
        hal.gpio.setMode(CBRD_SPI_SOFT_MISO);
        hal.gpio.setMode(CBRD_SPI_SOFT_MOSI);

        result = true;
    }
#endif

    RETURN(result);
}
#ifdef CBRD_SPI_SOFT_SCK
// ***************************************************************************
// FUNCTION
//      SWSPI_transmit
// PURPOSE
//
// PARAMETERS
//      PSWSPITCBLK b --
// RESULT
//      void --
// ***************************************************************************
int SPISW_transfer(SpiDevice *    dev,
                   PU8            send,
                   PU8            recv,
                   int            len,
                   UINT           TO)

{
    int             result          = 0;
    UINT            byteS           = 0xFF;
    UINT            byteR           = 0xFF;
    PU8             onS             = send;
    PU8             onR             = recv;
    UINT            cnt;
    UINT            left;
    Hal &           hal             = Hal::getInstance();

    // Waiting for SSEL
    while (SSEL)
    {
        //axsleep(1);
    }
//    printf(">>\n");

    __disable_irq();

    for (left = len; !SSEL && left; left--)
    {
        if (onS)
            byteS = *(onS++);

//        printf("%X ", byteS);

        for (cnt = 0; cnt < 8; cnt++)
        {
            // wait for low
            while (SCK && !SSEL)
            {
            }

            // set bit
            SET_MISO((byteS & 0x80) ? 1 : 0);

            byteS <<= 1;

            // wait for high
            while (!SCK && !SSEL)
            {
            }

            byteR <<=1;

            if (MOSI)
            {
                byteR |= 1;
            }
        }

        if (onR)
        {
            *(onR++) = byteR;
            result++;
        }
    }

    __enable_irq();

//    printf("<<\n");

    while (!SSEL)
    {
        //axsleep(1);
    }

//    printf("!!\n");

    return result;
}
#endif // #ifdef CBRD_SPI_SOFT_PORT
/*
// ***************************************************************************
// FUNCTION
//      SWSPI_RWFrames_Blocking
// PURPOSE
//
// PARAMETERS
//      SpiDevice * dev  --
//      PU8         send --
//      PU8         recv --
//      int         len  --
// RESULT
//      INT --
// ***************************************************************************
INT SWSPI_RWFrames_Blocking(SpiDevice *    dev,
                            PU8            send,
                            PU8            recv,
                            int            len)
{
    INT             result          = -1;
    SWSPITCBLK      blk;

    ENTER(true);

    blk.bank    = (LPC_GPIO_T *)dev->control;
    blk.send    = send;
    blk.recv    = recv;
    blk.len     = len;
    blk.result  = 0;

#ifdef LPC176X_USE_SOFTWARE_SPI_IRQ
    irqTrx      = &blk;

    while (!blk.result)
    {
        axsleep(1);
    }
#else
    SWSPI_transmit(&blk);
#endif

    result      = len;

    RETURN(result);
}
*/
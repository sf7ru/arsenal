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


#include <Hal.h>
#include <Spi.h>
#include <SpiDevice.h>
#include <string.h>

#include <app.h>

#include <customboard.h>

#include "stm32f0xx_hal.h"

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------


#define BUFFER_SIZE                         (0x100)

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

#ifdef CONF_SPI1
extern  SPI_HandleTypeDef   hspi1;
#endif

#ifdef CONF_SPI2
extern  SPI_HandleTypeDef   hspi2;
#endif

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      Spi::Spi
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
Spi::Spi()
{
    memset(&inited, 0, sizeof(inited));
}
// ***************************************************************************
// TYPE
//      Destructor
// FUNCTION
//      Spi::~Spi
// PURPOSE
//      Class destruction
// PARAMETERS
//          --
// ***************************************************************************
Spi::~Spi()
{

}
// ***************************************************************************
// FUNCTION
//      Spi::deinit
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      void --
// ***************************************************************************
void Spi::deinit(UINT           ifaceNo)
{
}
// ***************************************************************************
// FUNCTION
//      Spi::init
// PURPOSE
//
// PARAMETERS
//      UINT no --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL Spi::init(SpiDevice *    dev,
               UINT           ifaceNo,
               BOOL           master,
               UINT           bitrate)
{
    BOOL                result          = false;

    ENTER(true);

    if (!inited[ifaceNo])
    {
        switch (ifaceNo)
        {
            case 1:
#ifdef CONF_SPI1
                AXTRACE("INIT DEV on SPI1\n");
                result          = true;
                inited[ifaceNo] = true;
                dev->control    = &hspi1;
#endif
                break;

            case 2:
#ifdef CONF_SPI2
                result          = true;
                inited[ifaceNo] = true;
                dev->control    = &hspi2;
#endif
                break;

            default:
                break;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      Spi::transfer
// PURPOSE
//
// PARAMETERS
//      PU8 send --
//      PU8 recv --
//      int len  --
// RESULT
//      int --
// ***************************************************************************
int Spi::transfer(SpiDevice *    dev,
                  PU8            send,
                  PU8            recv,
                  int            len,
                  UINT           TO)
{
    int             result          = -1;
    int             rd;

    ENTER(true);

    AXTRACE(">> Spi::transfer = %u", len);

    onTransAssertSsel();

    switch(rd = HAL_SPI_TransmitReceive((SPI_HandleTypeDef*)dev->control, (uint8_t*)send, (uint8_t *)recv, len, 5000))
    {
        case HAL_OK:
            result = len;
            break;

        case HAL_TIMEOUT:
        case HAL_ERROR:
            result = 0;
            break;

        default:
            break;
     }

    onTransDeassertSsel();

    AXTRACE("<< Spi::transfer: = %d", result);

    RETURN(result);
}
int Spi::transmit(SpiDevice *    dev,
                  PU8            send,
                  int            len)
{
    int             result          = -1;

    ENTER(true);

//    AXTRACE(">> Spi::transfer");

    onTransAssertSsel();

    switch(HAL_SPI_Transmit((SPI_HandleTypeDef*)dev->control, (uint8_t*)send, len, 5000))
    {
        case HAL_OK:
            result = len;
            break;

        case HAL_TIMEOUT:
        case HAL_ERROR:
            result = 0;
            break;

        default:
            break;
     }

    onTransDeassertSsel();

//    AXTRACE("<< Spi::transfer: = %d", result);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      Spi::receive
// PURPOSE
//
// PARAMETERS
//      SpiDevice * dev  --
//      PU8         buff --
//      int         len  --
// RESULT
//      int --
// ***************************************************************************
int Spi::receive(SpiDevice *    dev,
                 PU8            buff,
                 int            len)
{
    int             result          = -1;

    ENTER(true);

//    AXTRACE(">> Spi::transfer");

    onTransAssertSsel();

    switch(HAL_SPI_Receive((SPI_HandleTypeDef*)dev->control, (uint8_t*)buff, len, 5000))
    {
        case HAL_OK:
            result = len;
            break;

        case HAL_TIMEOUT:
        case HAL_ERROR:
            result = 0;
            break;

        default:
            break;
     }

    onTransDeassertSsel();

//    AXTRACE("<< Spi::transfer: = %d", result);

    RETURN(result);
}



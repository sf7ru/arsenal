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


#include <HalPlatformDefs.h>

#ifdef HAL_SPI_MODULE_ENABLED

#include <axtime.h>
#include <Spi.h>
#include <SpiDevice.h>
#include <axstring.h>


// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

//static  SPI_DATA_SETUP_T    spi_xf;
//static  SPI_CONFIG_FORMAT_T spi_format;
//static  volatile uint8_t    spi_xfer_completed      = 0;
//
//static  Chip_SSP_DATA_SETUP_T ssp0_xf;
//static  SSP_ConfigFormat    ssp0_format;
//static  volatile uint8_t    ssp0_xfer_completed      = 0;
//
//static  Chip_SSP_DATA_SETUP_T ssp1_xf;
//static  SSP_ConfigFormat    ssp1_format;
//static  volatile uint8_t    ssp1_xfer_completed      = 0;

static SPI_HandleTypeDef    hspi    [ SPI_IFACES_NO ] = { 0 };

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

//extern BOOL         SWSPI_init                  (SpiDevice *        dev,
//                                                 PSPIMODE           mode);

//extern int SPISW_transfer(SpiDevice *    dev,
//                   PU8            send,
//                   PU8            recv,
//                   int            len,
//                   UINT           TO);

UINT getSpiPrescaller(UINT baudrateKHz)
{
#define SELECT_PRESCALER(a) if (div <= (a)) { result = SPI_BAUDRATEPRESCALER_##a; }

    UINT    result  = SPI_BAUDRATEPRESCALER_256;
    UINT    div     = (HAL_RCC_GetPCLK2Freq() / (baudrateKHz * 1000));

    SELECT_PRESCALER(2)
    else SELECT_PRESCALER(4)
    else SELECT_PRESCALER(8)
    else SELECT_PRESCALER(16)
    else SELECT_PRESCALER(32)
    else SELECT_PRESCALER(64)
    else SELECT_PRESCALER(128)

    return result;

#undef SELECT_PRESCALER
}
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
    constructClass();
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
//      UINT ifaceNo --
// RESULT
//      void --
// ***************************************************************************
void Spi::deinit(UINT ifaceNo)
{
}
// ***************************************************************************
// FUNCTION
//      Spi::init
// PURPOSE
//
// PARAMETERS
//      SpiDevice * dev     --
//      UINT        ifaceNo --
//      BOOL        master  --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL Spi::init(SpiDevice *    dev)
{
    BOOL                    result          = false;
    SPI_HandleTypeDef *     phspi;

    ENTER(true);

    if (!LCK)
    {

        if (initBase())
        {
            phspi = &hspi[ifaceNo];
//            dev->mIfaceNo = ifaceNo;
//            dev->control  = phspi;

            phspi->Init.Mode              = mode.master ? SPI_MODE_MASTER : SPI_MODE_SLAVE;
            phspi->Init.Direction         = SPI_DIRECTION_2LINES;
            phspi->Init.DataSize          = SPI_DATASIZE_8BIT;
            phspi->Init.CLKPolarity       = mode.cpol ? SPI_POLARITY_HIGH : SPI_POLARITY_LOW;
            phspi->Init.CLKPhase          = mode.cph ? SPI_PHASE_2EDGE : SPI_PHASE_1EDGE;
            phspi->Init.NSS               = mode.master ? (mode.softSelect ? SPI_NSS_SOFT : SPI_NSS_HARD_OUTPUT)
                                                        : SPI_NSS_HARD_INPUT;

            phspi->Init.BaudRatePrescaler = getSpiPrescaller(mode.baudrateKHz);

            phspi->Init.FirstBit          = mode.firstMsb ? SPI_FIRSTBIT_MSB : SPI_FIRSTBIT_LSB;
            phspi->Init.TIMode            = SPI_TIMODE_DISABLE;
            phspi->Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
            phspi->Init.CRCPolynomial     = 10;

#if (!defined(STM32F4) && !defined(STM32F1))
            phspi->Init.CRCLength         = SPI_CRC_LENGTH_DATASIZE;
                phspi->Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;
#endif
            if (initMsp())
            {
                switch (ifaceNo)
                {
                    case 0:
                        phspi->Instance = SPI1;
                        result = (HAL_SPI_Init(phspi) == HAL_OK);
                        break;

                    case 1:
                        phspi->Instance = SPI2;
                        result = (HAL_SPI_Init(phspi) == HAL_OK);
                        break;

#ifdef SPI3
                    case 2:
                        phspi->Instance = SPI3;
                        result = (HAL_SPI_Init(phspi) == HAL_OK);
                        break;
#endif

                    default:
                        break;
                }
            }
        }
    }
    else
        result = true;

    if (result)
    {
        phspi = &hspi[ifaceNo];
        dev->mIfaceNo = ifaceNo;
        dev->control  = phspi;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      Spi::transfer
// PURPOSE
//
// PARAMETERS
//      SpiDevice * dev  --
//      PU8         send --
//      PU8         recv --
//      int         len  --
// RESULT
//      int --
// ***************************************************************************
int Spi::transfer(PU8            send,
                  PU8            recv,
                  int            len,
                  UINT           TO)
{
    int             result          = -1;
    int             rd;

    ENTER(true);

    onTransAssertSsel();

    switch (rd = HAL_SPI_TransmitReceive((SPI_HandleTypeDef *) lockedDevice->control, (uint8_t *) send, (uint8_t *) recv,
                                         len, TO))
    {
        case HAL_OK:
            result = len;
            break;

        case HAL_TIMEOUT:
            result = 0;
            break;

        case HAL_ERROR:
            result = 0;
            break;

        default:
            break;
    }

    onTransDeassertSsel();

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      Spi::transmit
// PURPOSE
//
// PARAMETERS
//      SpiDevice * dev  --
//      PU8         send --
//      int         len  --
// RESULT
//      int --
// ***************************************************************************
int Spi::transmit(PU8            send,
                  int            len)
{
    int             result          = -1;

    ENTER(true);

    onTransAssertSsel();

    switch(HAL_SPI_Transmit((SPI_HandleTypeDef*)lockedDevice->control, (uint8_t*)send, len, 5000))
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
int Spi::receive(PU8            recv,
                 int            len)
{
    int             result          = -1;

    ENTER(true);

    onTransAssertSsel();

    switch(HAL_SPI_Receive((SPI_HandleTypeDef*)lockedDevice->control, (uint8_t*)recv, len, 5000))
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

    RETURN(result);
}

#endif // #ifdef HAL_SPI_MODULE_ENABLED
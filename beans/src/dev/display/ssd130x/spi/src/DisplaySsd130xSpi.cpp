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


#include <DisplaySsd130xSpi.h>
#include <customboard.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      DisplaySsd130xSpi::DisplaySsd130xSpi
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
DisplaySsd130xSpi::DisplaySsd130xSpi()
{
}
// ***************************************************************************
// FUNCTION
//      DisplaySsd130xSpi::onTransAssertSsel
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      void --
// ***************************************************************************
void DisplaySsd130xSpi::onTransAssertSsel()
{
    hal->gpio.setPin(ppCs, 0);
}
// ***************************************************************************
// FUNCTION
//      DisplaySsd130xSpi::onTransDeassertSsel
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      void --
// ***************************************************************************
void DisplaySsd130xSpi::onTransDeassertSsel()
{
    hal->gpio.setPin(ppCs, 1);
}
// ***************************************************************************
// FUNCTION
//      DisplaySsd130xSpi::init
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL DisplaySsd130xSpi::dispInit(int            ifaceNo,
                                 PORTPIN        ppDC,
                                 PORTPIN        ppCS,
                                 PORTPIN        ppRES)
{
    BOOL            result          = false;

    ENTER(true);

    this->ppDatCmd  = ppDC;
    this->ppCs      = ppCS;
    this->ppReset   = ppRES;

    this->spi       = &Spi::getInstance(ifaceNo);

    hal->gpio.setMode(ppCs);

    onTransDeassertSsel();

    if (SpiDevice::init(ifaceNo))
    {
        result = DisplaySsd130x::init();
    }
    else
        AXTRACE("SSD130X spi device init fault");


    RETURN(result);
}
int DisplaySsd130xSpi::read(PU8            cmd,
                            INT            size)
{
    return spi->transmit(cmd, size);
}
int DisplaySsd130xSpi::write(PU8            cmd,
                             INT            size)
{
    return spi->transmit(cmd, size);
}


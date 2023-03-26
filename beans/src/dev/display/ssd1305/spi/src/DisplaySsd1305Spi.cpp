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


#include <DisplaySsd1305Spi.h>
#include <customboard.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      DisplaySsd1305Spi::DisplaySsd1305Spi
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
DisplaySsd1305Spi::DisplaySsd1305Spi()
{
}
// ***************************************************************************
// FUNCTION
//      DisplaySsd1305Spi::onTransAssertSsel
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      void --
// ***************************************************************************
void DisplaySsd1305Spi::onTransAssertSsel()
{
    hal->gpio.setPin(CBRD_DISP_CS, 0);
}
// ***************************************************************************
// FUNCTION
//      DisplaySsd1305Spi::onTransDeassertSsel
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      void --
// ***************************************************************************
void DisplaySsd1305Spi::onTransDeassertSsel()
{
    hal->gpio.setPin(CBRD_DISP_CS, 1);
}
// ***************************************************************************
// FUNCTION
//      DisplaySsd1305Spi::init
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL DisplaySsd1305Spi::dispInit(UINT ifaceNo)
{
    BOOL            result          = false;

    ENTER(true);

    spi     = &Spi::getInstance(ifaceNo);

    if (SpiDevice::init(ifaceNo))
    {
        result = DisplaySsd1305::init();
    }
    else
        AXTRACE("SSD1305 spi device init fault");

    RETURN(result);
}
int DisplaySsd1305Spi::read(PU8            cmd,
                            INT            size)
{
    return spi->transmit(cmd, size);
}
int DisplaySsd1305Spi::write(PU8            cmd,
                             INT            size)
{
    return spi->transmit(cmd, size);
}


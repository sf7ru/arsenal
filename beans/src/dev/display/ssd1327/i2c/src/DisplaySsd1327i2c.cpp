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

#include <DisplaySsd1327.h>
#include <Hal.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

DisplaySsd1327::DisplaySsd1327()
{
    ppDatCmd    = PP_UNDEF;
    ppReset     = PP_UNDEF;
}
BOOL DisplaySsd1327::dispInit(int            ifaceNo,
                              int            address,
                              PORTPIN        ppReset,
                              PORTPIN        ppDC,
                              PORTPIN        ppCS,
                              PORTPIN        ppRES)
{
    BOOL        result          = false;
    Hal &       hal             = Hal::getInstance();

    ENTER(true);

    this->ppDatCmd  = ppDC;
    this->ppCs      = ppCS;
    this->ppReset   = ppRES;

    hal.gpio.setMode(ppDatCmd);

    if (PP_IS_DEFINED(ppReset))
        hal.gpio.setMode(ppReset);

    onTransDeassertSsel();

    if (I2cDevice::init(address, ifaceNo))
    {


        result = true;
    }

    RETURN(result);
}

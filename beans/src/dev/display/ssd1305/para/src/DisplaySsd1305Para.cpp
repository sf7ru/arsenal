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


#include <DisplaySsd1305Para.h>
#include <customboard.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      DisplaySsd1305Para::DisplaySsd1305Para
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
DisplaySsd1305Para::DisplaySsd1305Para()
{
}
BOOL DisplaySsd1305Para::dispInit()
{
    BOOL            result          = false;

    ENTER(true);

    hal->gpio.setMode(CBRD_DISP_D0);
    hal->gpio.setMode(CBRD_DISP_D1);
    hal->gpio.setMode(CBRD_DISP_D2);
    hal->gpio.setMode(CBRD_DISP_D3);
    hal->gpio.setMode(CBRD_DISP_D4);
    hal->gpio.setMode(CBRD_DISP_D5);
    hal->gpio.setMode(CBRD_DISP_D6);
    hal->gpio.setMode(CBRD_DISP_D7);

    hal->gpio.setMode(CBRD_DISP_WR, 1);
    hal->gpio.setMode(CBRD_DISP_RD, 1);

    if (init())
    {

        result = true;
    }

    RETURN(result);
}
int DisplaySsd1305Para::read(PU8            cmd,
                             INT            size)
{
//    hal->gpio.setPin(CBRD_DISP_RD, 0);
//    hal->gpio.setPin(CBRD_DISP_WR, 1);


    return size;
}
int DisplaySsd1305Para::write(PU8            data,
                              INT            size)
{
    for (int i = 0; i < size; i++)
    {
        U8 b = *(data + i);
        hal->gpio.setPin(CBRD_DISP_D0, (b >> 0) & 1u);
        hal->gpio.setPin(CBRD_DISP_D1, (b >> 1) & 1u);
        hal->gpio.setPin(CBRD_DISP_D2, (b >> 2) & 1u);
        hal->gpio.setPin(CBRD_DISP_D3, (b >> 3) & 1u);
        hal->gpio.setPin(CBRD_DISP_D4, (b >> 4) & 1u);
        hal->gpio.setPin(CBRD_DISP_D5, (b >> 5) & 1u);
        hal->gpio.setPin(CBRD_DISP_D6, (b >> 6) & 1u);
        hal->gpio.setPin(CBRD_DISP_D7, (b >> 7) & 1u);

        hal->gpio.setPin(CBRD_DISP_WR, 0);
        //axdelay(1);
        hal->gpio.setPin(CBRD_DISP_WR, 1);
        //axdelay(1);
    }

    return size;
}

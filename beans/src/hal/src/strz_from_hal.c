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

#include <PortPin.h>

static PCSTR GpioMode_names [] =
                 {
                     "none",
                     "pull down",
                     "pull up",
                     "open drain",
                     "repeater",
                     "push-pull"
                 };




PCSTR strz_from_GpioMode(GpioMode mode)
{
    PCSTR result = "unknown";

    if ((mode >= 0) && (mode <= GpioPushPull))
    {
        result = GpioMode_names[mode];
    }

    return result;
}


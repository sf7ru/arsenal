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

#include <Sys.h>

#include <stm32f1xx_hal.h>
#include <stdint.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

INT Sys::getUidString(PSTR           buff,
                      INT            size)
{
    INT         result          = 0;
    U32         bin             [ 3 ];

    ENTER(true);

    if (size >= ((8 * 3) + 1))
    {
        HAL_GetUID((uint32_t *) bin);

        result = sprintf(buff, "%.4X%.4X%.4X", bin[0], bin[1], bin[2]);
    }

    RETURN(result);
}

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

#include <stm32f7xx_hal.h>

#include <Sys.h>
#include <axstring.h>
#include <stdint.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

INT Sys::getUidString(PSTR           buff,
                      INT            size)
{
    INT         result          = 0;
    uint32_t    bin             [ 3 ];

    ENTER(true);

    if (size >= ((8 * 3) + 1))
    {
        bin[0] = HAL_GetUIDw0();
        bin[1] = HAL_GetUIDw1();
        bin[2] = HAL_GetUIDw2();

        strz_bin2hex(buff +  0, &bin[2], 4);
        strz_bin2hex(buff +  8, &bin[1], 4);
        strz_bin2hex(buff + 16, &bin[0], 4);

        result = true;
    }

    RETURN(result);
}
extern "C" void HAL_GetUID(uint32_t *UID)
{
  UID[0] = (uint32_t)(READ_REG(*((uint32_t *)UID_BASE)));
  UID[1] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 4U))));
  UID[2] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 8U))));
}

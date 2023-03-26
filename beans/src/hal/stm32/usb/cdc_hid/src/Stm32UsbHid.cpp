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

#include <arsenal.h>
#include <UsbHid.h>
#include <axtime.h>
#include <app.h>

#include "usbd_cdc_if.h"
#include "usb_device.h"

#include "customboard.h"

#include <stdint.h>
#include <ringbuff.h>
#include <usbd_customhid.h>
#include <hid_keyboard.h>

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

BOOL UsbHid::isReady()
{
    return (hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED);
}
// ***************************************************************************
// FUNCTION
//      UsbHid::usbInit
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL UsbHid::usbInit()
{
    BOOL            result          = true;

    ENTER(true);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      UsbHid::write
// PURPOSE
//
// PARAMETERS
//      PCVOID data --
//      INT    size --
//      UINT   TO   --
// RESULT
//      INT --
// ***************************************************************************
INT UsbHid::write(PCVOID         data,
                  INT            size,
                  UINT           TO)
{
    INT             result          = -1;
    UINT            left            = size;
    PU8             on              = (PU8)data;
    HIDKBDREPORT    report          = { 0 };

    ENTER(true);

    if (isReady())
    {
        result = 0;

        while (left--)
        {
            if (hidkbd_char_to_scancode(&report, *(on++)))
            {
                if (USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *) &report, sizeof(report)) == USBD_OK)
                {
                    result++;
                    axsleep(50);
                }
            }
        }

        report.MODIFIER = KEY_MOD_NONE;
        report.KEYCODE1 = 0;
        report.KEYCODE2 = 0;

        USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *) &report, sizeof(report));

    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      UsbHid::read
// PURPOSE
//
// PARAMETERS
//      PCVOID data --
//      INT    size --
//      UINT   TO   --
// RESULT
//      INT --
// ***************************************************************************
INT UsbHid::read(PVOID          data,
                 INT            size,
                 UINT           TO)
{
    INT             result          = 0;

    ENTER(true);

    //USBD_CUSTOM_HID_(&hUsbDeviceFS, (uint8_t *) &report, sizeof(report));

    RETURN(result);
}

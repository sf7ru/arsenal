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
#include <UsbCdc.h>
#include <axtime.h>
#include <app.h>

#include "usbd_cdc_if.h"
#include "usb_device.h"

#include "customboard.h"

#include <stdint.h>
#include <ringbuff.h>




//typedef struct __tag_RxBuffer
//{
//    uint8_t Buffer          [CDC_DATA_HS_OUT_PACKET_SIZE];
//    int     Position;
//    int     Size;
//    char    ReadDone;
//} RxBuffer;


// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

//extern  char                g_VCPInitialized;
//extern  RxBuffer            s_RxBuffer;

static  PRINGBUFF           rxRingbuff;


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

//extern "C"
//{
//    int8_t CDC_Receive_FS (uint8_t* Buf, uint32_t *Len);
//};

BOOL UsbCdc::isReady()
{
    return (hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED);
}
// ***************************************************************************
// FUNCTION
//      UsbCdc::usbInit
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL UsbCdc::usbInit()
{
    BOOL            result          = true;

    ENTER(true);

    if (!rxRingbuff && ((rxRingbuff = ringbuff1_create(USBCDC_RX_BUFF_SIZE)) == nil))
    {
        result = false;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      UsbCdc::write
// PURPOSE
//
// PARAMETERS
//      PCVOID data --
//      INT    size --
//      UINT   TO   --
// RESULT
//      INT --
// ***************************************************************************
INT UsbCdc::write(PCVOID         data,
               INT            size,
               UINT           TO)
{
    INT             result          = -1;
    PU8             pBuffer         = (PU8)data;

    ENTER(true);

    if (isReady())
    {
        if (size > CDC_DATA_FS_OUT_PACKET_SIZE)
        {
            int offset;
            int todo;
            for (offset = 0; offset < size; offset += todo)
            {
                todo = MIN(CDC_DATA_FS_OUT_PACKET_SIZE,
                               size - offset);
                int done = write(((char *)pBuffer) + offset, todo, TO);
                if (done != todo)
                    return offset + done;
            }

            return size;
        }

        if (CDC_Transmit_FS(pBuffer, size, TO) == USBD_OK)
        {
            result = size;
        }
    }

    RETURN(result);
}

extern "C"
{
    void USB_CDC_UserReceive(uint8_t *Buf,
                             uint32_t Len)
    {
        if (rxRingbuff != nil)
        {
            ringbuff1_write(rxRingbuff, Buf, Len);
        }
    }
}
// ***************************************************************************
// FUNCTION
//      UsbCdc::read
// PURPOSE
//
// PARAMETERS
//      PCVOID data --
//      INT    size --
//      UINT   TO   --
// RESULT
//      INT --
// ***************************************************************************
INT UsbCdc::read(PVOID          data,
                 INT            size,
                 UINT           TO)
{
    INT             result          = 0;
//    INT             rd;
//    uint32_t        len             = size;
    UINT            cd              = TO / 10;

    ENTER(true);

    if (isReady())
    {
        if (rxRingbuff != nil)
        {
            do
            {
                if (((result = ringbuff1_read(rxRingbuff, data, size)) == 0) && TO)
                {
                    if (cd)
                        axsleep(10);
                }

            } while (cd-- && !result);
        }
    }

    RETURN(result);
}

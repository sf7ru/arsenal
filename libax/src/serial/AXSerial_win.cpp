// ***************************************************************************
// TITLE
//      Serial Port Communications for Windows platform
// PROJECT
//
// ***************************************************************************
//
// FILE
//      $Id$
// HISTORY
//      $Log$
// ***************************************************************************

#include <stdio.h>
#include <AXSerial.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

AXSerial::AXSerial(void)
{
    dev = NULL;
}
AXSerial::~AXSerial(void)
{
    close();
}
BOOL AXSerial::close(void)
{
    if (dev)
    {
        dev = (PAXDEV)serial_close(dev);
    }

    return dev == NULL ? TRUE : FALSE;
}
BOOL AXSerial::isOpen(void)
{
    return dev == NULL ? FALSE : TRUE;
}
BOOL AXSerial::open(PSERIALPROPS  props,
                    UINT          type)
{
    return ((dev = serial_open(props, type)) != NULL) ? TRUE : FALSE;
}
BOOL AXSerial::open(PSERIALPROPS   props,
                    UINT           type,
                    PCSTR          name)
{
    return ((dev = serial_open(props, type)) != NULL) ? TRUE : FALSE;
//    return ((dev = serial_open_x(props, type, name)) != NULL) ? TRUE : FALSE;
}
INT AXSerial::read(PVOID            data,
                   INT              size,
                   UINT             TO)
{
    return _serial_read(dev, data, size, TO);
}
INT AXSerial::write(PCVOID          data,
                    INT             size,
                    UINT            TO)
{
    return _serial_write(dev, data, size, TO);
}
BOOL AXSerial::setBaudrate(UINT baudrate)
{
    BOOL                b_result    = FALSE;

    return b_result;
}

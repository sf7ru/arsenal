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
    dev = nil;
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

    return dev == nil ? true : false;
}
BOOL AXSerial::isOpen(void)
{
    return dev == nil ? false : true;
}
BOOL AXSerial::open(PSERIALPROPS  props,
                    UINT          type)
{
    return ((dev = serial_open(props, type)) != nil) ? true : false;
}
BOOL AXSerial::open(PSERIALPROPS   props,
                    UINT           type,
                    PCSTR          name)
{
    return ((dev = serial_open(props, type)) != nil) ? true : false;
//    return ((dev = serial_open_x(props, type, name)) != nil) ? true : false;
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
    BOOL                b_result    = false;

    return b_result;
}

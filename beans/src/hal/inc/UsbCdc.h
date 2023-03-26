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

#ifndef __USBCDC_H__
#define __USBCDC_H__

#include <arsenal.h>
#include <AXDevice.h>

#include <singleton_template.hpp>

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class UsbCdc: public SingletonTemplate<UsbCdc>, public AXDevice
{
public:

        BOOL            usbInit                 ();

        INT             write                   (PCVOID         data,
                                                 INT            size,
                                                 UINT           TO);

        INT             read                    (PVOID          data,
                                                 INT            size,
                                                 UINT           TO);

        BOOL            isReady                 ();
};


#endif // #ifndef __USBCDC_H__

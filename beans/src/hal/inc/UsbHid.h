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

#ifndef __USBHID_H__
#define __USBHID_H__

#include <arsenal.h>
#include <AXDevice.h>

#include <singleton_template.hpp>

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class UsbHid: public SingletonTemplate<UsbHid>, public AXDevice
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


#endif // #ifndef __USBHID_H__

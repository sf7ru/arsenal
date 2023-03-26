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

#ifndef __CONSOLEDEV_H__
#define __CONSOLEDEV_H__

#include <AXDevice.h>

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class ConsoleDev: public AXDevice
{
public:
                        ConsoleDev              ();
                        ~ConsoleDev             ();

        INT             read                    (PVOID          data,
                                                 INT            size,
                                                 UINT           TO);

        INT             write                   (PCVOID         data,
                                                 INT            size,
                                                 UINT           TO);
};

#endif // #ifndef __CONSOLEDEV_H__

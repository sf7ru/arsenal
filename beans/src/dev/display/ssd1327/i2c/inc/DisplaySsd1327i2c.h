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

#ifndef __DisplaySsd1327_H__
#define __DisplaySsd1327_H__

#include <arsenal.h>
#include <I2cDevice.h>
#include <PortPin.h>

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class DisplaySsd1327: public I2cDevice
{
protected:

        PORTPIN        ppDatCmd;
        PORTPIN        ppReset;
        PORTPIN        ppCs;

public:


                        DisplaySsd1327          ();

        void            onTransAssertSsel       ();

        void            onTransDeassertSsel     ();


        BOOL            dispInit                (int            ifaceNo,
                                                 int            address,
                                                 PORTPIN        ppReset,
                                                 PORTPIN        ppDC,
                                                 PORTPIN        ppCS,
                                                 PORTPIN        ppRES);

};

#endif // #define __DisplaySsd1327_H__


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

#ifndef __DisplaySsd130xSpi_H__
#define __DisplaySsd130xSpi_H__

#include <arsenal.h>

#include <Hal.h>

#include <customboard.h>

#include <SpiDevice.h>
#include <DisplaySsd130x.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class DisplaySsd130xSpi: public SpiDevice, public DisplaySsd130x
{
protected:
        Spi *           spi;

        void            lockDevice              () { spi->lockDevice(this); };
        void            unlockDevice            () { spi->unlockDevice(this); };
        void            longSelect              (BOOL   ena) { select(ena); };

        int             read                    (PU8            cmd,
                                                 INT            size);

        int             write                   (PU8            cmd,
                                                 INT            size);

public:
                        DisplaySsd130xSpi       ();

        void            onTransAssertSsel       ();

        void            onTransDeassertSsel     ();


        BOOL            dispInit                (int            ifaceNo,
                                                 PORTPIN        ppDC,
                                                 PORTPIN        ppCS,
                                                 PORTPIN        ppRES);

};

#endif

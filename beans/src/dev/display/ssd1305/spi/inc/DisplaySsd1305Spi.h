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

#ifndef __DisplaySsd1305Spi_H__
#define __DisplaySsd1305Spi_H__

#include <arsenal.h>

#include <Hal.h>

#include <customboard.h>

#include <SpiDevice.h>
#include <DisplaySsd1305.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class DisplaySsd1305Spi: public SpiDevice, public DisplaySsd1305
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
                        DisplaySsd1305Spi       ();

        void            onTransAssertSsel       ();

        void            onTransDeassertSsel     ();

        BOOL            dispInit                (UINT           ifaceNo     = 0);
};

#endif

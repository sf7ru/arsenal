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

#ifndef __DisplaySsd1327Spi_H__
#define __DisplaySsd1327Spi_H__

#include <arsenal.h>
#include <SpiDevice.h>
#include <PortPin.h>
#include <DisplaySsd1327.h>
#include <Display.h>


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class DisplaySsd1327Spi: public SpiDevice, public DisplaySsd1327
{
protected:

        PORTPIN         ppDatCmd;

        void            oledWrite               (PU8            buff,
                                                 UINT           size,
                                                 BOOL           isData = false) override ;


        void            setPosition             (int            x,
                                                 int            y,
                                                 int            cx,
                                                 int            cy);

        void            writeDataBlock          (U8 *           ucBuf,
                                                 int            iLen);

public:

                        DisplaySsd1327Spi       ();

//        void            onTransAssertSsel       ();

//        void            onTransDeassertSsel     ();


        BOOL            dispInit                (int            ifaceNo,
                                                 PORTPIN        ppDC,
                                                 PORTPIN        ppCS,
                                                 PORTPIN        ppRES);

};

#endif // #define __DisplaySsd1327_H__


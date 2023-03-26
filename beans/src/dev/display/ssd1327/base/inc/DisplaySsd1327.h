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
#include <SpiDevice.h>
#include <PortPin.h>
#include <Display4bit.h>

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class DisplaySsd1327: public Display4bit
{
protected:
        PORTPIN        ppReset;

        void            power                   (BOOL           bOn);

virtual void            oledWrite               (PU8            buff,
                                                 UINT           size,
                                                 BOOL           isData = false) = 0;

virtual void            writeDataBlock          (PU8            ucBuf,
                                                 int            iLen) = 0;

        void            ssd1327WriteCommand     (U8             cmd);

        void            reset                   ();

        void            setPosition             (int            x,
                                                 int            y,
                                                 int            cx,
                                                 int            cy);
public:

                        DisplaySsd1327          ();

        void            applyBuffer             (UINT           x,
                                                 UINT           y,
                                                 UINT           w,
                                                 UINT           h);


};

#endif // #define __DisplaySsd1327_H__


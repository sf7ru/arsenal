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

#ifndef __DISPLAYHD44760_H__
#define __DISPLAYHD44760_H__

#include <arsenal.h>

#include <Hal.h>

#ifdef putc
#undef putc
#endif

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class DisplayWs0010
{
        void            drawColumn              (int            x,
                                                 int            y,
                                                 unsigned char  data);

protected:
        void            sendCommand             (U8             cmd);

        void            sendData                (U8             data);

        U8              getData                 ();

        void            setData                 (UINT           value);

        U8              waitbusy                (void);

        void            delayE                  ();

        U8              read                    (U8             rs);

        void            turnRs                  (Hal &          hal,
                                                 BOOL           val);

        void            turnRw                  (Hal &          hal,
                                                 BOOL           val);

        void            turnE                   (Hal &          hal,
                                                 BOOL           val);

        void            toggleE                 (Hal &           hal);

        void            setDataOutput           (Hal &           hal,
                                                 BOOL           val);

        void            write                   (U8             data,
                                                 U8             rs);

        void            newline                 (U8             pos);

        void            initBase                ();

        void            setGfxModeHalf          (BOOL           enable);

        void            selectHalf              (int            no);

        PU8             applyHalf               (PU8            bitmap);

public:
                        DisplayWs0010          ();

        BOOL            init                    ();

        void            clear                   (void);

        void            home                    (void);

        void            putc                    (unsigned char  c);

        void            puts                    (PCSTR          string);

        void            setCharsetWin1251       (void);

        void            gotoxy                  (U8             x,
                                                 U8             y);

        void            loadUserFont            (PCU8           font,
                                                 UINT           number,
                                                 UINT           height);

        BOOL            setGfxMode              (BOOL           enable);

        void            applyBitmap             (PU8            displayBuff);
};

#endif

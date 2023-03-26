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

typedef enum
{
    CURSORMODE_none,        // none
    CURSORMODE_underline,   // underline cursor
    CURSORMODE_blink,       // blinking symbol, no cursor
    CURSORMODE_ul_blink     // underline blinking cursor and symbol
} CURSORMODE;

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class DisplayHd44780
{
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


public:
                        DisplayHd44780          ();

        BOOL            init                    ();

        void            clear                   (void);

        void            home                    (void);

        void            putc                    (unsigned char  c);

        void            puts                    (PCSTR          string);

        void            setCharsetWin1251       (void);

        void            putcRaw                 (unsigned char  c)
        { write(c, 1); }


        void            gotoxy                  (U8             x,
                                                 U8             y);

        void            loadUserFont            (PCU8           font,
                                                 UINT           number,
                                                 UINT           height);

        void            setCursorMode           (CURSORMODE     mode);
};

#endif

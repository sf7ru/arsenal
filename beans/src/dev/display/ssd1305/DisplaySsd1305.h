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

#ifndef __DisplaySsd1305_H__
#define __DisplaySsd1305_H__

#include <arsenal.h>

#include <Hal.h>

#include <customboard.h>

#include <Display1bitTilted.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

//#define MAC_PIN(a,b)        pin(CBRD_DISP_##a##_GPIO, CBRD_DISP_##a##_PIN, value ? CBRD_DISP_##a##_ONE : !CBRD_DISP_##a##_ONE);

#define SSD1305_SETLOWCOLUMN    0x00
#define SSD1305_SETHIGHCOLUMN   0x10
#define SSD1305_MEMORYMODE      0x20
#define SSD1305_SETCOLADDR      0x21
#define SSD1305_SETPAGEADDR     0x22
#define SSD1305_SETRECT         0x24
#define SSD1305_SETSTARTLINE    0x40

#define SSD1305_SETCONTRAST     0x81
#define SSD1305_SETBRIGHTNESS   0x82

#define SSD1305_SETLUT          0x91

#define SSD1305_SEGREMAP        0xA0
#define SSD1305_DISPLAYALLON_RESUME 0xA4
#define SSD1305_DISPLAYALLON    0xA5
#define SSD1305_NORMALDISPLAY   0xA6
#define SSD1305_INVERTDISPLAY   0xA7
#define SSD1305_SETMULTIPLEX    0xA8
#define SSD1305_DISPLAYDIM      0xAC
#define SSD1305_MASTERCONFIG    0xAD
#define SSD1305_DISPLAYOFF      0xAE
#define SSD1305_DISPLAYON       0xAF

#define SSD1305_SETPAGESTART    0xB0

#define SSD1305_COMSCANINC      0xC0
#define SSD1305_COMSCANDEC      0xC8
#define SSD1305_SETDISPLAYOFFSET 0xD3
#define SSD1305_SETDISPLAYCLOCKDIV 0xD5
#define SSD1305_SETAREACOLOR    0xD8
#define SSD1305_SETPRECHARGE    0xD9
#define SSD1305_SETCOMPINS      0xDA
#define SSD1305_SETVCOMLEVEL    0xDB


// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class DisplaySsd1305: public Display
{
protected:
        Hal *           hal;

        UINT            page_offset;
        UINT            column_offset;

        void            reset                   ();

        void            setModeCommand          (BOOL           value);

        void            command                 (U8             cmd);

        void            sendData                (PU8            data,
                                                 UINT           size);

        void            invert                  (BOOL           value)
        { command(value ? SSD1305_INVERTDISPLAY : SSD1305_NORMALDISPLAY); }


virtual void            lockDevice              () {};
virtual void            unlockDevice            () {};
virtual void            longSelect              (BOOL   ena) {};


virtual int             read                    (PU8            data,
                                                 INT            size) = 0;

virtual int             write                   (PU8            data,
                                                 INT            size) = 0;

        void            clearDisplay            ();


       void             setPixel                (int            x,
                                                 int            y,
                                                 U8             color);
//        {
//            int offset;
//            int bit;
//
//            int page_size = 128;
//
//            offset = (y >> 3) + (x * mPitch);
//            bit    = (y & 7);
//
//            if (color)
//                *(mBuffer + offset) |= U8(1 << bit);
//            else
//                *(mBuffer + offset) &= ~U8(1 << bit);
//        }


public:
                        DisplaySsd1305          ();

        BOOL            init                    ();


        void            applyBuffer             (UINT           x,
                                                 UINT           y,
                                                 UINT           w,
                                                 UINT           h) override;


        void            print                   (PFONTFV1       font,
                                                 int            x,
                                                 int            y,
                                                 int            w,
                                                 int            h,
                                                 int            color,
                                                 PCSTR          message) override;

        void            box                     (int            x1,
                                                 int            y1,
                                                 int            width,
                                                 int            height,
                                                 int            color) override;

        void            rect                    (int            x1,
                                                 int            y1,
                                                 int            width,
                                                 int            height,
                                                 int            color);

        void            clear                   (int            color) override;

};

#endif

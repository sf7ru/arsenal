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

#ifndef __DisplaySsd130x_H__
#define __DisplaySsd130x_H__

#include <arsenal.h>

#include <Hal.h>

#include <customboard.h>

#include <Display1bitTilted.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

//#define MAC_PIN(a,b)        pin(CBRD_DISP_##a##_GPIO, CBRD_DISP_##a##_PIN, value ? CBRD_DISP_##a##_ONE : !CBRD_DISP_##a##_ONE);

#define SSD130X_SETLOWCOLUMN    0x00
#define SSD130X_SETHIGHCOLUMN   0x10
#define SSD130X_MEMORYMODE      0x20
#define SSD130X_SETCOLADDR      0x21
#define SSD130X_SETPAGEADDR     0x22
#define SSD130X_SETRECT         0x24
#define SSD130X_SETSTARTLINE    0x40

#define SSD130X_SETCONTRAST     0x81
#define SSD130X_SETBRIGHTNESS   0x82

#define SSD130X_SETLUT          0x91

#define SSD130X_SEGREMAP        0xA0
#define SSD130X_DISPLAYALLON_RESUME 0xA4
#define SSD130X_DISPLAYALLON    0xA5
#define SSD130X_NORMALDISPLAY   0xA6
#define SSD130X_INVERTDISPLAY   0xA7
#define SSD130X_SETMULTIPLEX    0xA8
#define SSD130X_DISPLAYDIM      0xAC
#define SSD130X_MASTERCONFIG    0xAD
#define SSD130X_DISPLAYOFF      0xAE
#define SSD130X_DISPLAYON       0xAF

#define SSD130X_SETPAGESTART    0xB0

#define SSD130X_COMSCANINC      0xC0
#define SSD130X_COMSCANDEC      0xC8
#define SSD130X_SETDISPLAYOFFSET 0xD3
#define SSD130X_SETDISPLAYCLOCKDIV 0xD5
#define SSD130X_SETAREACOLOR    0xD8
#define SSD130X_SETPRECHARGE    0xD9
#define SSD130X_SETCOMPINS      0xDA
#define SSD130X_SETVCOMLEVEL    0xDB

#define SSD130X_ChargePumpSet   0x8D
#define SSD130X_DeactivateScroll 0x2E

// Режимы автоматического сдвига указателя в памяти кадра ssd130x
#define SSD130X_Adressing_Horizontal            0       // Сначала инкремент по горизонтали, затем инкремент по вертикали
#define SSD130X_Adressing_Vertical              1       // Сначала инкремент по вертикали, затем инкремент по горизонтали
#define SSD130X_Adressing_Page                  2       // Инкремент только по горизонтали


// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class DisplaySsd130x: public Display1bitTilted
{
protected:
        Hal *           hal;
        PORTPIN         ppReset;
        PORTPIN         ppDatCmd;

        UINT            page_offset;
        UINT            column_offset;

        void            reset                   ();

        void            setModeCommand          (BOOL           value);

        void            command                 (U8             cmd);

        void            command                 (PU8            cmd,
                                                 UINT           sz);

        void            sendData                (PU8            data,
                                                 UINT           size);

        void            invert                  (BOOL           value)
        { command(value ? SSD130X_INVERTDISPLAY : SSD130X_NORMALDISPLAY); }


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


        void            setDisplayClockDivider  (U8             DCLKdiv, 
                                                 U8             Fosc);

        void            setMultiplexRatio       (U8             Mux);

        void            setDisplayOffset        (U8             Offset);

        void            setDisplayStartLine     (U8             Line);

        void            chargePumpSetting       (U8             Value);

        void            setMemAdressingMode     (U8             Mode);

        void            setSegmentRemap         (U8             Value);

        void            setCOMoutScanDirection  (U8             Value);

        void            setCOMPinsConfig        (U8             AltCOMpinConfig, 
                                                 U8             LeftRightRemap);

        void            setContrast             (U8             Value);

        void            setPrechargePeriod      (U8             Phase1period, 
                                                 U8             Phase2period);

        void            setVCOMHDeselectLevel   (U8             Code);

        void            allPixRAM               (void);

        void            setInverseOn            (void);

        void            deactivateScroll        (void);

        void            setInverseOff           (void);


        void            setColumns              (U8             Start, 
                                                 U8             End);

        void            setPages                (U8             Start, 
                                                 U8             End);

public:
                        DisplaySsd130x          ();

        BOOL            init                    ();


        void            applyBuffer             (UINT           x,
                                                 UINT           y,
                                                 UINT           w,
                                                 UINT           h) override;


};

#endif

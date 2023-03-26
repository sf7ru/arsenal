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

#ifndef __Display4bit_H__
#define __Display4bit_H__

#include <Display.h>

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class Display4bit: public Display
{
protected:

        inline PU8              setPixel                (int            x,
                                                         PU8            buff,
                                                         U8             color)
        {
            if (x & 1)
            {
                *buff = (*buff & 0xF0) | (color & 0xF);
                buff++;
            }
            else
            {
                *buff = (*buff & 0x0F) | ((color << 4) & 0xF0);
            }

            return buff;
        }

public:


                        Display4bit             (PU8           buffer,
                                                 UINT          depth,
                                                 UINT          width,
                                                 UINT          height)
                        {
                            setDimensions(width, height, depth);
                            setBuffer(buffer);
                        }


        void            print                   (PFONTFV1       font,
                                                 int            x,
                                                 int            y,
                                                 int            w,
                                                 int            h,
                                                 UINT           color,
                                                 PCSTR          message) override;

        void            box                     (int            x1,
                                                 int            y1,
                                                 int            width,
                                                 int            height,
                                                 UINT           color) override;

        void            rect                    (int            x1,
                                                 int            y1,
                                                 int            width,
                                                 int            height,
                                                 UINT           color);

        void            clear                   (UINT           color) override;
};

#endif // #define __Display4bit_H__


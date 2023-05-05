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

#ifndef __Display1bit_H__
#define __Display1bit_H__

#include <Display.h>

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class Display1bit: public Display
{
protected:

        inline PU8              setPixel                (int            x,
                                                         PU8            buff,
                                                         U8             color)
        {
            int offset = x >> 3;
            int bit    = (7 - (x & 7));

            if (color)
                *(buff + offset) |= U8(1 << bit);
            else
                *(buff + offset) &= ~U8(1 << bit);

            return buff;
        }

        inline U8               getPixel                (int            x,
                                                         PU8            buff)
        {
            int offset = x >> 3;
            int bit    = (7 - (x & 7));

            return *(buff + offset) & U8(1 << bit) ? 1 : 0;
        }

public:
                        Display1bit             ()
                        {

                        }

                        Display1bit             (PU8           buffer,
                                                 UINT          width,
                                                 UINT          height)
                        {
                            setDimensions(width, height, 1);
                            setBuffer(buffer);
                        }

        void            setPixel                (UINT           x,
                                                 UINT           y,
                                                 UINT           color)
        {
            if ((x < mWidth) && (y < mHeight))
            { setPixel(x, mBuffer + (mPitch * y), color); }
        }

        UINT            getPixel                (UINT           x,
                                                 UINT           y)
        {
            return ((x < mWidth) && (y < mHeight)) ? getPixel(x, mBuffer + (mPitch * y)) : 0;
        }

        void            print                   (PFONTFV1       font,
                                                 int            x,
                                                 int            y,
                                                 int            w,
                                                 int            h,
                                                 UINT           color,
                                                 PCSTR          message) ; // override;

        void            box                     (int            x1,
                                                 int            y1,
                                                 int            width,
                                                 int            height,
                                                 UINT           color) ; // override;

        void            rect                    (int            x1,
                                                 int            y1,
                                                 int            width,
                                                 int            height,
                                                 UINT           color);

        void            clear                   (UINT           color) ; // override;

};

#endif // #define __Display1bit_H__


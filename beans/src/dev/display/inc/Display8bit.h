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

#ifndef __Display8bit_H__
#define __Display8bit_H__

#include <Display.h>

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class Display8bit: public Display
{
protected:

        inline PU8              setPixel                (int            x,
                                                         PU8            buff,
                                                         U8             color)
        {
            *(buff + x) = color;

            return buff;
        }

public:
                        Display8bit             ()
                        {

                        }


                        Display8bit             (PU8           buffer,
                                                 UINT          width,
                                                 UINT          height)
                        {
                            setDimensions(width, height, 8);
                            setBuffer(buffer);
                        }


        void            setPixel                (UINT           x,
                                                 UINT           y,
                                                 UINT           color)
        {
            if ((x < mWidth) && (y < mHeight))
            { setPixel(x, mBuffer + (mPitch * y), color); }
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

#endif // #define __Display8bit_H__


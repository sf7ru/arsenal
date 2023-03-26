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

#ifndef __Display1bitTilted_H__
#define __Display1bitTilted_H__

#include <Display.h>

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class Display1bitTilted: public Display
{
protected:

        BOOL            mTilted                 = true;

inline void             setPixel                (int            x,
                                                 int            y,
                                                 U8             color)
        {
            int offset;
            int bit;

            if (mTilted)
            {
                offset = (y >> 3) + (x * mPitch);
                bit    = (y & 7);
            }
            else
            {
                offset = (x >> 3) + (y * mPitch);
                bit    = (x & 7);
            }

            if (color)
                *(mBuffer + offset) |= U8(1 << bit);
            else
                *(mBuffer + offset) &= ~U8(1 << bit);
        }

        BOOL            bufferInit              ()
        {
            if (mTilted)
            {
                setPitch(mHeight);
            }
            else
                setPitch(mWidth);

            return true;
        }

public:

                        Display1bitTilted       (PU8           buffer, 
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

#endif // #define __Display1bitTilted_H__


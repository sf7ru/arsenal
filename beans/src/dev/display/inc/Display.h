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

#ifndef __Display_H__
#define __Display_H__

#include <arsenal.h>
#include <customboard.h>
#include <limits.h>
#include <stdio.h>
#include <WienImage.h>
#include <axmath.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

//#define DISP_WIDTH_MUTLTIPLIER                  ((double)mDepth / 8)


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef AXPACKED(struct) __tag_FONTFV1
{
    U8          width;
    U8          height;
    U8          bnk1offset;
    U8          bnk1size;
    U8          bnk2offset;
    U8          bnk2size;
} FONTFV1, * PFONTFV1;

typedef AXPACKED(struct) __tag_DISPPOINT
{
    int         x;
    int         y;
} DISPPOINT, * PDISPPOINT;


// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

#ifdef __cplusplus

class Display
{
protected:
        UINT            mWidth;
        UINT            mHeight;
        UINT            mDepth;
        UINT            mPitch;

        INT             window_x1;
        INT             window_y1;
        INT             window_x2;
        INT             window_y2;

        void            (Display::*selectedLineFunc)     (int            x1,
                                                          int            y1,
                                                 int            x2,
                                                 int            y2,
                                                 UINT           color);


        UINT            mBufferSize;
        PU8             mBuffer; //                   [ (UINT)(DISP_WIDTH_MUTLTIPLIER * CBRD_DISP_WIDTH * CBRD_DISP_HEIGHT) ];


        void            setPitch                (UINT           width);

        BOOL            drawNonT                (PU8 			buff,
                                                 int            x,
                                                 int            y,
                                                 UINT           width,
                                                 UINT           height);

        BOOL            drawT                   (PU8 			buff,
                                                 int            x,
                                                 int            y,
                                                 UINT           width,
                                                 UINT           height,
                                                 int            color);

public:
                        Display                 ()
                        { 
                        }

        void            setDimensions           (UINT          width, 
                                                 UINT          height, 
                                                 UINT          depth)
                        {
                            mWidth      = width;
                            mHeight     = height;
                            mDepth      = depth;
                            
                            setPitch(width);

                            mBufferSize = mPitch * height;

                            if ((width > 256) || (height > 256)) 
                                selectedLineFunc = &Display::lineForAllDisplays; //  : &lineForSmallDisplays;

                            validate();
                        }

        void            setBuffer               (PVOID          buffer)
        {
            mBuffer = (PU8)buffer;
        }


        UINT            getWidth                ()
        { return mWidth; }                        

        UINT            getHeight               ()
        { return mHeight; }                        

        UINT            getBufferSize           ()
        { return mBufferSize; }

        void            validate                ()
        {
            window_x1               = INT_MAX;
            window_y1               = INT_MAX;
            window_x2               = -1;
            window_y2               = -1;
        }


        void            lineForAllDisplays      (int            x1,
                                                 int            y1,
                                                 int            x2,
                                                 int            y2,
                                                 UINT           color);


        void            lineForSmallDisplays    (int            x1,
                                                 int            y1,
                                                 int            x2,
                                                 int            y2,
                                                 UINT           color);

virtual void            line                    (int            x1,
                                                 int            y1,
                                                 int            x2,
                                                 int            y2,
                                                 UINT           color)
                        { (this->*selectedLineFunc)(x1, y1, x2, y2, color); }

virtual void            print                   (PFONTFV1       font,
                                                 int            x,
                                                 int            y,
                                                 int            w,
                                                 int            h,
                                                 UINT           color,
                                                 PCSTR          message) {};

virtual void            box                     (int            x1,
                                                 int            y1,
                                                 int            x2,
                                                 int            y2,
                                                 UINT           color) {};

virtual void            rect                    (int            x1,
                                                 int            y1,
                                                 int            x2,
                                                 int            y2,
                                                 UINT           color) {};


virtual void            applyBuffer             (UINT           x,
                                                 UINT           y,
                                                 UINT           w,
                                                 UINT           h) = 0;

virtual void            clear                   (UINT           color) = 0;


virtual void            setPixel                (UINT           x,
                                                 UINT           y,
                                                 UINT           color) = 0;

virtual void            tiltedRect              (int            x,
                                                 int            y,
                                                 UINT           width,
                                                 UINT           height,
                                                 int            angle,
                                                 UINT           color);

virtual BOOL            draw                    (int            x,
                                                 int            y,
                                                 UINT           width,
                                                 UINT           height,
                                                 PWIENIMAGE     image);

virtual BOOL            tiltedDraw              (int            x,
                                                 int            y,
                                                 PWIENIMAGE     image,
                                                 int            cx,
                                                 int            cy,
                                                 double         angle);

        void            update                  ()
        { applyBuffer(0, 0, mWidth, mHeight); validate(); }
};

#endif // #ifdef __cplusplus

#endif // #define __Display_H__

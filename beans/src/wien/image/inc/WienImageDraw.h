// ***************************************************************************
// TITLE
//
// PROJECT
//     
// ***************************************************************************

#ifndef __WIENIMAGEDRAW_H__
#define __WIENIMAGEDRAW_H__

#include <WienImage.h>
#include <Display.h>

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------


// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class WienImageDraw
{
protected:
        Display *       mDisp;
        PWIENIMAGE      mImage;
        PU8             mPushedBitmap;
        
public:
        BOOL            init                    (PWIENIMAGE     image,
                                                 UINT           colorDepth = 0);

        BOOL            draw                    (PWIENIMAGE     image,
                                                 int            x       = 0,
                                                 int            y       = 0,
                                                 UINT           width   = 0,
                                                 UINT           height  = 0);

        PWIENIMAGE      getImage                ()
        {
                return mImage;
        }

        BOOL            pushBitmap              ();

        void            popBitmap               ();

        void            popBitmapCopy           ();

        void            tiltedRect              (int            x,
                                                 int            y,
                                                 UINT           width,
                                                 UINT           height,
                                                 int            angle,
                                                 UINT           color)
        {
            mDisp->tiltedRect(x, y, width, height, angle, color);
        }
};


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#endif // #define __WIENIMAGEDRAW_H__

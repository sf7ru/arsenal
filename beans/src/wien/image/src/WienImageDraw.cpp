// ***************************************************************************
// TITLE
//
// PROJECT
//     
// ***************************************************************************

#include <WienImageDraw.h>
#include <Display1bit.h>
#include <Display4bit.h>
#include <Display8bit.h>
#include <axstring.h>
#include <string.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

class Display1BitOnImage: public Display1bit
{
public:    
                    Display1BitOnImage(PVOID    bitmap,
                                       UINT     width,
                                       UINT     height) : Display1bit((PU8)bitmap, width, height)
                    {
                    }

    void            applyBuffer             (UINT           x,
                                             UINT           y,
                                             UINT           w,
                                             UINT           h)
    {

    }
};

class Display4BitOnImage: public Display4bit
{
public:    
                    Display4BitOnImage(PVOID    bitmap,
                                       UINT     width,
                                       UINT     height) : Display4bit((PU8)bitmap, width, height)
                    {
                    }

    void            applyBuffer             (UINT           x,
                                             UINT           y,
                                             UINT           w,
                                             UINT           h)
    {
    }
};

class Display8BitOnImage: public Display8bit
{
public:        
                    Display8BitOnImage(PVOID      bitmap,
                                       UINT     width,
                                       UINT     height) : Display8bit((PU8)bitmap, width, height)
                    {
                    }

    void            applyBuffer             (UINT           x,
                                             UINT           y,
                                             UINT           w,
                                             UINT           h)
    {
    }
};

BOOL WienImageDraw::init(PWIENIMAGE     image,  
                         UINT           colorDepth)
{
    BOOL            result = false;
    
    ENTER(true);

    if (colorDepth)
    {
        mImage = wienimage_dither(image, colorDepth);
    }
    else
        mImage = wienimage_copy(image);

    if (mImage != nil)
    {
        switch (mImage->depth)
        {
            case 1: mDisp = new Display1BitOnImage(mImage->bitmap, mImage->width, mImage->height); break;
            case 4: mDisp = new Display4BitOnImage(mImage->bitmap, mImage->width, mImage->height); break;
            case 8: mDisp = new Display8BitOnImage(mImage->bitmap, mImage->width, mImage->height); break;
            
            default:
                break;
        }

        if (mDisp != nil)
        {
            //mDisp->setBuffer();
            result = true;
        }
    }
    
    RETURN(result);
}
BOOL WienImageDraw::pushBitmap()
{
    BOOL        result      = false;

    SAFEFREE(mPushedBitmap);

    if ((mPushedBitmap = (PU8)strz_memdup(mImage->bitmap, wienimage_get_bitmap_size(mImage))) != nil)
    {
        result = true;
    }

    return result;
}
void WienImageDraw::popBitmapCopy()
{
    if (mPushedBitmap && mImage->bitmap)
    {
        memcpy(mImage->bitmap, mPushedBitmap, wienimage_get_bitmap_size(mImage));
    }
}
void WienImageDraw::popBitmap()
{
    if (mPushedBitmap)
    {
        if (mImage->bitmap)
            free(mImage->bitmap);

        mImage->bitmap  = mPushedBitmap;
        mPushedBitmap   = nil;
    }
}
BOOL WienImageDraw::draw(PWIENIMAGE     image,
                         int            x,
                         int            y,
                         UINT           width,
                         UINT           height)
{
    BOOL            result          = false;
    PWIENIMAGE      img;

    if (image->depth != mImage->depth)
    {
        if ((img = wienimage_dither(image, mImage->depth)) != nil)
        {
            result = mDisp->draw(x, y, width, height, img);

            wienimage_destroy(img);
        }
    }
    else
        result = mDisp->draw(x, y, width, height, image);

    return result;
}

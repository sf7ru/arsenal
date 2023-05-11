// ***************************************************************************
// TITLE
//
// PROJECT
//     conductor
// ***************************************************************************

#include <WienImage.h>


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

static int __read8(PWIENIMAGEFASTREAD   fr,
                   int                  x,
                   int                  y)
{
    return *(((PU8)fr->bitmap) + x + (y * fr->pitch));
}

int wienimage_fastread(PWIENIMAGEFASTREAD fr, 
                       int                x, 
                       int                y)
{
    return fr->func(fr, x,y);
}
BOOL wienimage_fastread_prepare(PWIENIMAGEFASTREAD fr, 
                                PWIENIMAGE         image)
{
    BOOL        result          = false;

    switch (image->depth)
    {
        case 8:
            fr->func    = (PWIENFRFUNC)__read8;
            break;
    
        default:
            fr->func    = nil;
            break;
    }

    if (fr->func)
    {
        fr->bitmap  = image->bitmap;
        fr->width   = image->width;
        fr->height  = image->height;
        fr->pitch   = image->pitch;

        result      = true;
    }

    return result;
}

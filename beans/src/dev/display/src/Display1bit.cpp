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

#include <Display1bit.h>
#include <string.h>

// FIXME unimplemented



void Display1bit::print(PFONTFV1       font,
                        int            x,
                        int            y,
                        int            w,
                        int            h,
                        UINT           color,
                        PCSTR          message)
{
    int             i;
    int             iFontOff;
    int             tx;
    int             ty;
    unsigned char   ucMask;
    unsigned char   c;
    unsigned char*  src;
    unsigned char*  dest;

    U8      cx      = font->width;
    U8*     pFont   = (PU8)(font + 1);

    int     dy      = font->height / 8;

    if (dy % 8)
        dy++;

    i = 0;
    if (y > mHeight-(font->height-1)) return; // will write past the bottom

    while (x < mWidth-(font->width-1) && message[i] != 0)
    {
        c               = (unsigned char)message[i];

        if (  (c >= font->bnk1offset)                   &&
              (c < (font->bnk1offset + font->bnk1size)) )
        {
            iFontOff = (int)(c - font->bnk1offset) * cx * dy;
        }
        else if (  font->bnk2offset                       &&
                   (c >= font->bnk2offset)                &&
                   (c < (font->bnk2offset + font->bnk2size)) )
        {
            iFontOff = (int)((c - font->bnk2offset) + font->bnk1size) * cx * dy;
        }
        else
            iFontOff = -1;

        if (iFontOff >= 0)
        {
            src    = &pFont[iFontOff];
            dest   = &mBuffer[(y * mPitch) + (x / 2)];
            ucMask = 1;

            for (ty = 0; ty < font->height; ty++)
            {
                if (ty && !(ty % 8))
                {
                    src += font->width;
                    ucMask = 1;
                }
                for (tx=0; tx<cx; tx+=2)
                {
                    if (src[tx] & ucMask) // foreground
                    {
                        dest[0] &= 0xf;
                        dest[0] |= (color << 4);
                    }
                    if (src[tx+1] & ucMask)
                    {
                        dest[0] &= 0xf0;
                        dest[0] |= color;
                    }
                    dest++;
                }
                ucMask <<= 1;

                dest -= cx / 2;
                if (cx & 1)
                    dest--;

                dest += mPitch; // move to next line
            } // for ty
            x += cx;
        }

        i++;
    }
}
void Display1bit::rect(int            x1,
                       int            y1,
                       int            width,
                       int            height,
                       UINT           ucColor)
{
    // U8*     pBuffer;
    // int     tx;
    // int     ty;
    // int     x2  = x1 + width - 1;
    // int     y2  = y1 + height - 1;


//    pBuffer = &mBuffer[(y1 * mPitch)];
//    for (tx=x1; tx<x2; tx++)
//    {
//        pBuffer = setPixel(tx, pBuffer, ucColor);
//    }

//    pBuffer = &mBuffer[(y2*mPitch)+(x1/2)]; // starting point also
//    for (tx=x1; tx<x2; tx++)
//    {
//        pBuffer = setPixel(tx, pBuffer, ucColor);
//    }
//
//    pBuffer = &mBuffer[(y1*mPitch)+(x1/2)]; // starting point also
//    for (ty=y1; ty<y2; ty++)
//    {
//        setPixel(x1, pBuffer, ucColor);
//
//        pBuffer += mPitch;
//    }
//
//    pBuffer = &mBuffer[(y1*mPitch)+(x2/2)]; // starting point also
//    for (ty=y1; ty<y2; ty++)
//    {
//        setPixel(x2, pBuffer, ucColor);
//
//        pBuffer += mPitch;
//    }
}
void Display1bit::box(int            x1,
                      int            y1,
                      int            width,
                      int            height,
                      UINT           ucColor)
{
    U8*     pBuffer;
    int     x2  = x1 + width;
    int     y2  = y1 + height;

    for (int y = y1; y < y2; y++)
    {
        pBuffer = &mBuffer[(y * mPitch) + (x1 / 2)]; // starting point also
        for (int x = x1; x < x2; x++)
        {
            pBuffer = setPixel(x, pBuffer, ucColor);
        }
    }
}
void Display1bit::clear(UINT           color)
{
    memset(mBuffer, color ? 0xFF : 0x00, mBufferSize);
}

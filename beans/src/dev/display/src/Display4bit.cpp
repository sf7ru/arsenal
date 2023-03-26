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

#include <Display4bit.h>
#include <string.h>


void Display4bit::print(PFONTFV1       font,
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
    if (y > (int)(mHeight-(font->height-1))) return; // will write past the bottom

    while (x < (int)(mWidth-(font->width-1)) && message[i] != 0)
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
//void DisplaySsd1327Spi::drawLine(int            x1,
//                                 int            y1,
//                                 int            x2,
//                                 int            y2,
//                                 U8             ucColor)
//{
//    int temp;
//    int dx = x2 - x1;
//    int dy = y2 - y1;
//    int error;
//    U8 *p;
//    int xinc, yinc, shift;
//    int y, x;
//
//    if (x1 < 0 || x2 < 0 || y1 < 0 || y2 < 0 || x1 >= iMaxX || x2 >= iMaxX || y1 >= iMaxY || y2 >= iMaxY)
//        return;
//
//    if(abs(dx) > abs(dy)) {
//        // X major case
//        if(x2 < x1) {
//            dx = -dx;
//            temp = x1;
//            x1 = x2;
//            x2 = temp;
//            temp = y1;
//            y1 = y2;
//            y2 = temp;
//        }
//
//        y = y1;
//        dy = (y2 - y1);
//        error = dx >> 1;
//        yinc = 1;
//        if (dy < 0)
//        {
//            dy = -dy;
//            yinc = -1;
//        }
//        p = &ucBackbuffer[(x1/2) + (y1 * iPitch)]; // point to current spot in back buffer
//        shift = (x1 & 1) ? 0:4; // current bit offset
//        for(x=x1; x1 <= x2; x1++) {
//            *p &= (0xf0 >> shift);
//            *p |= (ucColor << shift);
//            shift = 4-shift;
//            if (shift == 4) // time to increment pointer
//                p++;
//            error -= dy;
//            if (error < 0)
//            {
//                error += dx;
//                if (yinc > 0)
//                    p += iPitch;
//                else
//                    p -= iPitch;
//                y += yinc;
//            }
//        } // for x1
//    }
//    else {
//        // Y major case
//        if(y1 > y2) {
//            dy = -dy;
//            temp = x1;
//            x1 = x2;
//            x2 = temp;
//            temp = y1;
//            y1 = y2;
//            y2 = temp;
//        }
//
//        p = &ucBackbuffer[(x1/2) + (y1 * iPitch)]; // point to current spot in back buffer
//        shift = (x1 & 1) ? 0:4; // current bit offset
//        dx = (x2 - x1);
//        error = dy >> 1;
//        xinc = 1;
//        if (dx < 0)
//        {
//            dx = -dx;
//            xinc = -1;
//        }
//        for(x = x1; y1 <= y2; y1++) {
//            *p &= (0xf0 >> shift); // set the pixel
//            *p |= (ucColor << shift);
//            error -= dx;
//            p += iPitch; // y1++
//            if (error < 0)
//            {
//                error += dy;
//                x += xinc;
//                shift = 4-shift;
//                if (xinc == 1)
//                {
//                    if (shift == 4) // time to increment pointer
//                        p++;
//                }
//                else
//                {
//                    if (shift == 0)
//                        p--;
//                }
//            }
//        } // for y
//    } // y major case
//}
void Display4bit::rect(int            x1,
                       int            y1,
                       int            width,
                       int            height,
                       UINT           ucColor)
{
    U8*     pBuffer;
    int     tx;
    int     ty;
    int     x2  = x1 + width - 1;
    int     y2  = y1 + height - 1;


    pBuffer = &mBuffer[(y1*mPitch)+(x1/2)]; // starting point also
    for (tx=x1; tx<x2; tx++)
    {
        pBuffer = setPixel(tx, pBuffer, ucColor);
    }

    pBuffer = &mBuffer[(y2*mPitch)+(x1/2)]; // starting point also
    for (tx=x1; tx<x2; tx++)
    {
        pBuffer = setPixel(tx, pBuffer, ucColor);
    }

    pBuffer = &mBuffer[(y1*mPitch)+(x1/2)]; // starting point also
    for (ty=y1; ty<y2; ty++)
    {
        setPixel(x1, pBuffer, ucColor);

        pBuffer += mPitch;
    }

    pBuffer = &mBuffer[(y1*mPitch)+(x2/2)]; // starting point also
    for (ty=y1; ty<y2; ty++)
    {
        setPixel(x2, pBuffer, ucColor);

        pBuffer += mPitch;
    }
}
void Display4bit::box(int            x1,
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
void Display4bit::clear(UINT           color)
{
    U8   ucColor = color | ((U8)color << 4); // set pixel pair color

    memset(mBuffer, ucColor, mBufferSize);
}

// ***************************************************************************
// TITLE
//
// PROJECT
//     conductor
// ***************************************************************************

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<arsenal.h>

#include <WienImage.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------
//-------------------------------- Hi 555 ------------------------------------

#define RAWRGB2HI555(rgbq) (U16)(( ((rgbq&0xf8L)>>3) | ((rgbq&0xf800L)>>6) | ((rgbq&0xf80000L)>>9)))
#define HI5552RAWRGB(rgbq) (U32)(( ((rgbq&0x1fL)<<3) | ((rgbq&0x3e0L)<<6) | ((rgbq&0x7c00L)<<9)))

//-------------------------------- Hi 565 ------------------------------------
    
#define RAWRGB2HI565(rgbq) (U16)(((rgbq&0xf8L)>>3) | ((rgbq&0xfc00L)>>5) | ((rgbq&0xf80000L)>>8))
#define HI5652RAWRGB(rgbq) (U32)(((rgbq&0x1fL)<<3) | ((rgbq&0x7e0L)<<5) | ((rgbq&0xf800L)<<8))
#define INVERTHI565(w) (U16)(((w&0x1f)<<11) | (w&0x7e0) | ((w&0xf800)>>11))

//-------------------------------- Hi 444A -----------------------------------

#define HI444A2RAWRGB(rgbq) (U32)(((rgbq&0x0fL)<<4) | ((rgbq&0xf0L)<<8) | ((rgbq&0xf00L)<<12) | ((rgbq&0xf000L)<<16))
#define RAWRGB2HI444A(rgbq) (U16)(((rgbq&0xf0L)>>4) | ((rgbq&0xf000L)>>8) | ((rgbq&0xf00000L)>>12) | ((rgbq&0xf0000000L)>>16))

//----------------------------- Hi (various) ---------------------------------

#define HI4542RAWRGB(rgbq) (U32)(((rgbq&0x0fL)<<4) | ((rgbq&0x1f0L)<<7) | ((rgbq&0x1e00L)<<11))
#define HI3432RAWRGB(rgbq) (U32)(((rgbq&0x07L)<<5) | ((rgbq&0x78L)<<9) | ((rgbq&0x380L)<<14))
#define RAWRGB2HI646(rgbq) (U16)(((rgbq&0xfcL)>>2) | ((rgbq&0xf000L)>>6) | ((rgbq&0xfc0000L)>>8))

//---------------------------------- RGB -------------------------------------

#define PALRGB2RAWRGB(rgbq) (U32)(((rgbq&0xffL)<<16) | (rgbq&0xff00L) | ((rgbq&0xff0000L)>>16))
#define RAWRGB2PALRGB   PALRGB2RAWRGB

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

extern unsigned char vga_default_palette[ 256 * 3 ];


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

static BOOL _dither_1_to_8(PWIENIMAGE tgt,
                           PWIENIMAGE src)
{
    BOOL     result = true;
    UINT    i;
    UINT    j;
    INT     b;
    PU8     onS;
    UINT    w;
    PU8     onT;
    int     color1  = -1;
    int     color0  = -1;

    onT = tgt->bitmap;

    if (src->transparentColor != -1)
    {
        if (src->transparentColor)
        {
            color0 = (src->defaultColor != -1) ? src->defaultColor : DEFAULTVGAPAL_BLACK;
            tgt->defaultColor       = color0;
        }
        else
        {
            color1 = (src->defaultColor != -1) ? src->defaultColor : DEFAULTVGAPAL_WHITE;
            tgt->defaultColor       = color1;
        }

        tgt->transparentColor   = src->transparentColor;
    }
    else
    {
        color0 = DEFAULTVGAPAL_BLACK;
        color1 = src->defaultColor != -1 ? src->defaultColor : DEFAULTVGAPAL_WHITE;
        tgt->defaultColor           = src->defaultColor;
    }

    for (j = 0; j < src->height; j++)
    {
        onS = ((PU8)src->bitmap) + (j * src->pitch);
        w   = src->width;

        for (i = 0; w && (i < src->pitch); i++)
        {
            for (b = 7; w && (b >= 0); b--)
            {
                //*(onT++) = ((*onS) >> b) & 1 ? VGARGB_WHITE : VGARGB_BLACK;

                if (((*onS) >> b) & 1)
                {
                    if (color1 != -1)
                        *(onT) = color1;
                }
                else
                {
                    if (color0 != -1)
                        *(onT) = color0;
                }
                
                onT++;
                w--;
            }

            onS++;
        }
    }

    return result;
}
static BOOL _dither_8_to_16(PWIENIMAGE tgt,
                            PWIENIMAGE src)
{
    BOOL     result = true;
    
    UINT    i;
    PU8     onS = src->bitmap;
    PU8     onT = tgt->bitmap;

    for(i = 0; i < (src->width * src->height); i++)
    {
        *((PU16)onT) = RAWRGB2HI555(PALRGB2RAWRGB((*(PU32)(((PU8)src->palette)+((*(PU8)onS<<1)+*(PU8)onS)))&0xffffff));
        onS++;
        onT+=2;
    }

    return result;
}
static BOOL _dither_8_to_24(PWIENIMAGE tgt,
                            PWIENIMAGE src)
{
    BOOL     result = true;
    
    UINT    i;
    PU8     onS = src->bitmap;
    PU8     onT = tgt->bitmap;

    for(i = 0; i < (src->width * src->height); i++)
    {
        *((PU32)onT)=PALRGB2RAWRGB((*(PU32)(((PU8)src->palette)+((*(PU8)onS<<1)+*(PU8)onS)))&0xffffff);
        onS++;
        onT+=3;
    }

    return result;
}
static BOOL _dither_24_to_16(PWIENIMAGE tgt,
                             PWIENIMAGE src)
{
    BOOL     result = true;
    
    UINT    i;
    PU8     onS = src->bitmap;
    PU8     onT = tgt->bitmap;

    for (i = 0; i < (src->width * src->height); i++)
    {
        *((PU16)onT) = RAWRGB2HI555(*((PU32)onS));
        onS += 3;
        onT += 2;
    }

    return result;
}

static BOOL _dither_32_to_16(PWIENIMAGE tgt,
                             PWIENIMAGE src)
{
    BOOL     result = true;
    
    UINT    i;
    PU8     onS = src->bitmap;
    PU8     onT = tgt->bitmap;

    for (i = 0; i < (src->width * src->height); i++)
    {
        *((PU16)onT) = RAWRGB2HI565(*((PU32)onS));
        onS += 4;
        onT += 2;
    }

    return result;
}
PWIENIMAGE wienimage_dither(PWIENIMAGE img,
                            UINT       newDepth)
{
#define MAC_DITHER(was,shouldBe)   case shouldBe: done = _dither_##was##_to_##shouldBe(result, img); break

    PWIENIMAGE  result              = nil;
    BOOL        done                = false;

    if (img)
    {
        if (img->depth != newDepth)
        {
            if ((result = wienimage_create(img->width, img->height, newDepth)) != nil)
            {
                switch(img->depth)
                {
                    case 1:
                        switch(newDepth)
                        {
                            MAC_DITHER(1, 8);
                            MAC_DITHER(8, 24);

                            default:
                                break;
                        } 
                        break;

                    case 8:
                        switch(newDepth)
                        {
                            MAC_DITHER(8, 16);
                            MAC_DITHER(8, 24);

                            default:
                                break;
                        } 
                        break;

                    case 16:
                        break;

                    case 24:
                        switch(newDepth)
                        {
                            MAC_DITHER(24, 16);

                            default:
                                break;
                        } 
                        break;

                    case 32:
                        switch(newDepth)
                        {
                            MAC_DITHER(32, 16);

                            default:
                                break;
                        } 
                        break;

                    default:
                        break;                    
                }

                if (!done)
                {
                    result = wienimage_destroy(result);
                }
            }
        }
        else 
            result = wienimage_copy(img);
    }

    return result;
}

void wienimage_palette_DIB2VGA(PU8      palette, 
                               UINT     entries)
{
    U32   i;

    for(i = 0; i < entries; i++)
      *(PU32)(palette + (i * 3)) = PALRGB2RAWRGB(*(PU32)(palette + (i * 4)));

    for(i = 0; i < (entries * 3); i++)
      *(palette++) >>= 2;
}
PVOID wienimage_palette_invert(PVOID      palette)
{
    PVOID   result      = nil;
    PU8     src         = (PU8)palette;
    PU8     tgt;
    int     i;
    U32     entry;

    if ((result = (PU8)MALLOC(256 * 3 + 1)) != nil)
    {
        tgt = (PU8)result;

        for (i = 0; i < 256; i++, tgt += 3, src += 3)
        {
            entry = *(PU32)src;
            *(PU32)tgt = ((entry & 0xFF) << 16) | (entry & 0xFF00) | (((entry & 0xFF0000) >> 16));
        }
    }

    return result;
}
BOOL wienimage_palette_copy(PWIENIMAGE    tgt, 
                            PWIENIMAGE    src)
{
    BOOL    result      = false;

    if (tgt && src && tgt->palette && src->palette)
    {
        memcpy(tgt->palette, src->palette, WIENIMAGE_PALETTE_SZ);

        result  = true;
    }

    return result;
}
void wienimage_set_default_palette(PWIENIMAGE    img)
{
    if (img->palette && !img->flags.staticPalette)
    {
        FREE(img->palette);
    }

    img->palette                = &vga_default_palette[0];
    img->flags.staticPalette    = true;
}
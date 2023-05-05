// ***************************************************************************
// TITLE
//
// PROJECT
//     conductor
// ***************************************************************************


#ifndef __WIENIMAGE_H__
#define __WIENIMAGE_H__

#include <arsenal.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define DEFAULTVGAPAL_BLACK 0 
#define DEFAULTVGAPAL_WHITE 15
#define DEFAULTVGAPAL_RED   32
#define DEFAULTVGAPAL_BLUE  40
#define DEFAULTVGAPAL_GREEN 48

#define WIENIMAGE_PALETTE_SZ    (256 * 3)

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct _tag_WIENIMAGE 
{
    UINT                    width;
    UINT                    height;
    UINT                    depth;
    UINT                    pitch;
    PVOID                   bitmap;

    PVOID                   palette;
    U32                     transparentColor;
    U32                     defaultColor;

    struct
    {
        UINT                staticInfo      : 1;
        UINT                staticBitmap    : 1;
        UINT                staticPalette   : 1;
    } flags;
} WIENIMAGE, * PWIENIMAGE;


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif 

void                wienimage_flip_bitmap       (PU8                p_bitmap, 
                                                 UINT               width, 
                                                 UINT               height);

PWIENIMAGE          wienimage_destroy           (PWIENIMAGE         image);

BOOL                wienimage_save_targa        (PCSTR              filename,
                                                 PWIENIMAGE         image);

BOOL                wienimage_save_png          (PCSTR              filename,
                                                 PWIENIMAGE         image);


BOOL                wienimage_palette_copy      (PWIENIMAGE         tgt, 
                                                 PWIENIMAGE         src);

void                wienimage_palette_DIB2VGA   (PU8                palette, 
                                                 UINT               entries);

void                wienimage_set_default_palette(PWIENIMAGE        img);

PWIENIMAGE          wienimage_copy              (PWIENIMAGE         img);


PWIENIMAGE          wienimage_dither            (PWIENIMAGE         img, 
                                                 UINT               newDepth);


PWIENIMAGE          wienimage_create            (UINT               width, 
                                                 UINT               height, 
                                                 UINT               depth);

UINT                wienimage_get_bitmap_size   (PWIENIMAGE         img);

#ifdef __cplusplus

}
#endif 





#endif // #define __WIENIMAGE_H__
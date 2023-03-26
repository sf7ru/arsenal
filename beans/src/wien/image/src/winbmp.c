// ***************************************************************************
// TITLE
//      Windows Bitmap picture format
// PROJECT
//      Arsenal
// ***************************************************************************

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <arsenal.h>

#include <WienImage.h>


// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define BMP_SIGNATURE               0x4D42
#define PALRGB2RAWRGB(rgbq) (U32    )(((rgbq&0xffL)<<16) | (rgbq&0xff00L) | ((rgbq&0xff0000L)>>16))


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------


#pragma pack(1)

//-------------------------------- BMP_FH ------------------------------------

typedef AXPACKED(struct) _tag_BMP_FH
{
    U16      bfType;                         // "BM" or 0x4D42
    U32      bfSize;                         // Size of file in bytes
    U16      bfReserved1;                    // Set to 0
    U16      bfReserved2;                    // Set to 0
    U32      bfOffBits;                      // Offset in file where
                                            // the bits begin
} BMP_FH;

//-------------------------------- BMP_IH ------------------------------------

typedef AXPACKED(struct) _tag_BMP_IH
{
    U32      biSize;                         // Size of the structure
    U32      biWidth;                        // Width in pixels
    U32      biHeight;                       // Height in pixels
    U16      biPlanes;                       // # of color Planes: Set to 1
    U16      biBitCount;                     // Color bits per pixel
    U32      biCompression;                  // Compression Scheme
    U32      biSizeImage;                    // Number of bitmap bytes
    U32      biXPelsPerMeter;                // Horizontal Resolution
    U32      biYPelsPerMeter;                // Vertical Resolution
    U32      biClrUsed;                      // Number of colors usedt
    U32      biClrImportant;                 // Important colors
} BMP_IH;

#pragma pack()


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------


PWIENIMAGE wienimage_load_winbmp(PSTR p_filename)
{
    FILE        *p_in_file;
    PWIENIMAGE  image;
    U32         d_size;
    U32         d_cnt;
    U32         d_bitmap_width;
    U32         d_bitmap_size;
    PU8         p_tmp_ptr1;
    PU8         p_tmp_ptr2;
    BMP_FH      st_BMP_FH;
    BMP_IH      st_BMP_IH;

    if ((p_in_file = fopen(p_filename, "rb")) != NULL)
    {
        if ((image = malloc(sizeof(WIENIMAGE))) != NULL)
        {
            memset(image, 0, sizeof(WIENIMAGE));

            if ( fread(&st_BMP_FH, sizeof(BMP_FH), 1, p_in_file) &&
                 (st_BMP_FH.bfType == BMP_SIGNATURE)             &&
                 fread(&st_BMP_IH, sizeof(BMP_IH), 1, p_in_file) )
            {

//-------------------- Procesing palette if neccessary -----------------------

                if (st_BMP_IH.biBitCount < 16)
                {
                    if ( ((image->palette = malloc((1 << st_BMP_IH.biBitCount) << 2)) != NULL)    &&
                         fread(image->palette, ((1 << st_BMP_IH.biBitCount) << 2), 1, p_in_file) )
                    {
                        // Resolving palette from WinBMP format
                        for (d_cnt = 0; d_cnt < (1 << st_BMP_IH.biBitCount); d_cnt++)
                            *(PU32    )(image->palette + (d_cnt * 3)) =
                                PALRGB2RAWRGB(*(PU32    )(image->palette + (d_cnt << 2)));

                        for (d_cnt = 0; d_cnt < (1 << st_BMP_IH.biBitCount) * 3; d_cnt++)
                            *(((PU8)image->palette) + d_cnt) >>= 2;
                    }
                    else
                        image = wienimage_destroy(image);
                }

//--------------------------- Processing bitmap ------------------------------

                if (image)
                {
                    // Calculating real bitmap width
                    if (st_BMP_IH.biWidth != ((st_BMP_IH.biWidth) >> 2) << 2)
                        d_bitmap_width = (((st_BMP_IH.biWidth) >> 2) + 1) <<2 *(st_BMP_IH.biBitCount >> 3);
                    else
                        d_bitmap_width = st_BMP_IH.biWidth * (st_BMP_IH.biBitCount >> 3);

                    // Calculating real bitmap size
                    d_bitmap_size = d_bitmap_width * st_BMP_IH.biHeight;

                    // Loading Bitmap
                    if ( ((image->bitmap = MALLOC(d_bitmap_size)) != NULL) &&
                         fread(image->bitmap, d_bitmap_size, 1, p_in_file) )
                    {
                        wienimage_flip_bitmap(image->bitmap, d_bitmap_width, st_BMP_IH.biHeight);

                        // Removing align space if neccessary
                        if (d_bitmap_width != st_BMP_IH.biWidth * (st_BMP_IH.biBitCount >> 3))
                        {
                            p_tmp_ptr1 = image->bitmap;
                            p_tmp_ptr2 = image->bitmap;
                            d_size     = st_BMP_IH.biWidth * (st_BMP_IH.biBitCount >> 3);

                            for(d_cnt = 0; d_cnt < st_BMP_IH.biHeight; d_cnt++)
                            {
                                memcpy(p_tmp_ptr1, p_tmp_ptr2, d_size);
                                p_tmp_ptr1 += d_size;
                                p_tmp_ptr2 += d_bitmap_width;
                            }

                            // Storing parameters
                            image->width       = st_BMP_IH.biWidth;
                            image->height      = st_BMP_IH.biHeight;
                            image->depth       = st_BMP_IH.biBitCount;
                            image->transparent = -1;
                        }
                    }
                    else
                        image = wienimage_destroy(image);
                }
            }
        }

        fclose(p_in_file);
    }

    return image;
}


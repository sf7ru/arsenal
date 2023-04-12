// ***************************************************************************
// TITLE
//
// PROJECT
//     conductor
// ***************************************************************************

#include <WienImage.h>
#include <string.h>
#include <math.h>
#include <axstring.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

PWIENIMAGE wienimage_destroy(PWIENIMAGE         image)
{
    if (image)
    {
        if (!image->flags.staticBitmap && image->bitmap)
        {
            FREE(image->bitmap);
        }

        if (!image->flags.staticPalette && image->palette)
        {
            FREE(image->palette);
        }

        if (!image->flags.staticInfo)
        {
            FREE(image);
        }
    }

    return nil;
}
PWIENIMAGE wienimage_create(UINT               width, 
                            UINT               height, 
                            UINT               depth)
{
    PWIENIMAGE      result      = nil;

    if ((result = CREATE(WIENIMAGE)) != nil)
    {
        result->pitch = (UINT)ceil((float)depth / 8 * width);
        // printf("width = %d, height = %d, depth = %d, pitch = %d, size = %d\n", 
        //     width, height, depth, result->pitch, result->pitch * height);

        if ((result->bitmap = MALLOC(result->pitch * height)) != nil)
        {
            result->width   = width;
            result->height  = height;
            result->depth   = depth;

            if ((depth > 1) && (depth <= 8))
            {
                wienimage_set_default_palette(result);
            }
        }
        else
            result = wienimage_destroy(result);
    }

    return result;    
}
PWIENIMAGE wienimage_copy(PWIENIMAGE         img)
{
    PWIENIMAGE      result      = nil;

    if ((result = CREATE(WIENIMAGE)) != nil)
    {
        memcpy(result, img, sizeof(WIENIMAGE));

        result->bitmap  = nil;
        result->palette = nil;

        if (img->flags.staticBitmap)
        {
            result->flags.staticBitmap = true;
            result->bitmap             = img->bitmap;
        }
        else if ((result->bitmap = strz_memdup(img->bitmap, img->pitch * img->height)) == nil)
        {
            result = wienimage_destroy(result);
        }

        if (result)
        {
            if (img->flags.staticPalette)
            {
                result->flags.staticPalette = true;
                result->palette             = img->palette;
            }
            else if ((result->palette = strz_memdup(img->palette, WIENIMAGE_PALETTE_SZ)) == nil)
            {
                result = wienimage_destroy(result);
            }
        }
    }

    return result;    
}

void wienimage_flip_bitmap(PU8 p_bitmap, UINT width, UINT height)
{
    PU8   p_tmp_line;
    PU8   p_bitmap_start;
    PU8   p_bitmap_end;
    UINT    cnt;

    if ((p_tmp_line = (PU8)MALLOC(width)) != NULL)
    {
        p_bitmap_start = p_bitmap;
        p_bitmap_end   = p_bitmap + (width * (height - 1));
        for(cnt = 0; cnt < (height >> 1); cnt++)
        {
            memcpy(p_tmp_line,     p_bitmap_start, width);
            memcpy(p_bitmap_start, p_bitmap_end,   width);
            memcpy(p_bitmap_end,   p_tmp_line,     width);
            p_bitmap_start += width;
            p_bitmap_end   -= width;
        }
        free(p_tmp_line);
    }
}
UINT  wienimage_get_bitmap_size(PWIENIMAGE         img)
{
    return img->pitch * img->height;
}

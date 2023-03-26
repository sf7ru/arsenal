// ***************************************************************************
// TITLE
//      Targa picture format
// PROJECT
//      conductor
// ***************************************************************************

#include<stdlib.h>
#include <stdio.h>
#include<string.h>

#include<WienImage.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define TGAATTRIB_INVERTED          0x20


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef AXPACKED(struct) _tag_TARGAIMAGEHEADER
{
    U8      IDFieldPresence;                // =0
    U8      ColorMapPresence;               // =0
    U8      ImageType;                      // =1 for mapped, =2 for unmapped
    U16     ColorMapOrigin;
    U16     ColorMapLenght;
    U8      ColorMapEntrySize;
    U16     OriginX;
    U16     OriginY;
    U16     Width;
    U16     Height;
    U8      Depth;
    U8      ImageAttributes;                // see TGA format description
} TARGAIMAGEHEADER, * PTARGAIMAGEHEADER;


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

/*
PWIENIMAGE kds_Targa_load_picture(PSTR psz_filename, PDU16  pd_ret_code)
{
    NTATALKCODE         ret_code                = NTATC_out;
    FILE              * p_targa_file;
    DU16                d_size;
    DU16                d_offset;
    PNTAPICTURE         fps_pict                = NULL;
    TARGAIMAGEHEADER    st_my_TARGAIMAGEHEADER;

    if ((p_targa_file = fopen(psz_filename, "rb")) != NULL)
    {
        if((fps_pict = (PNTAPICTURE)malloc(sizeof(NTAPICTURE))) != NULL)
        {
            memset(fps_pict, 0, sizeof(NTAPICTURE));

            if(fread(&st_my_TARGAIMAGEHEADER, sizeof(TARGAIMAGEHEADER), 1, p_targa_file))
            {
                if (st_my_TARGAIMAGEHEADER.ColorMapPresence)
                {
                    if ((fps_pict->p_palette = malloc(0x400)) != NULL)
                    {
                        d_size   =  st_my_TARGAIMAGEHEADER.ColorMapLenght *
                                    (st_my_TARGAIMAGEHEADER.ColorMapEntrySize >> 3);
                        d_offset =  st_my_TARGAIMAGEHEADER.ColorMapOrigin *
                                    (st_my_TARGAIMAGEHEADER.ColorMapEntrySize >> 3);

                        if (!fread(fps_pict->p_palette + d_offset, d_size, 1, p_targa_file))
                            ret_code = NTATC_file | NTATC_read | 2;
                    }
                    else ret_code = NTATC_memory | NTATC_request | 2;
                }

                if (ret_code == NTATC_out)
                {
                    d_size = st_my_TARGAIMAGEHEADER.Width  *
                                st_my_TARGAIMAGEHEADER.Height *
                            (st_my_TARGAIMAGEHEADER.Depth >> 3);

                    if ((fps_pict->p_bitmap = malloc(d_size)) != NULL)
                    {
                        if(fread(fps_pict->p_bitmap, d_size, 1, p_targa_file))
                        {
                            fps_pict->d_width       = st_my_TARGAIMAGEHEADER.Width;
                            fps_pict->d_height      = st_my_TARGAIMAGEHEADER.Height;
                            fps_pict->b_depth       = st_my_TARGAIMAGEHEADER.Depth;
                            fps_pict->d_transparent = MAXLONG;

                            if (!(st_my_TARGAIMAGEHEADER.ImageAttributes & TGAATTRIB_INVERTED))
                                kds_invert_picture(fps_pict);
                        }
                        else ret_code = NTATC_file | NTATC_read | 3;
                    }
                    else ret_code = NTATC_memory | NTATC_request | 3;
                }
            }
            else ret_code = NTATC_file | NTATC_read | 1;

            if (fps_pict->p_bitmap == NULL)
            {
                kds_destroy_picture(fps_pict);
                fps_pict = NULL;
            }
        }
        else ret_code = NTATC_memory | NTATC_request | 1;

        fclose(p_targa_file);
    }
    else ret_code = NTATC_file | NTATC_open;

    if (pd_ret_code)
        *pd_ret_code = ret_code;

    return fps_pict;
}
*/
BOOL wienimage_save_targa(PCSTR              filename,
                          PWIENIMAGE         image)
{
    BOOL                result      = false;
    FILE *              fout;
    TARGAIMAGEHEADER    st_TARGAIMAGEHEADER;
    UINT                d_image_width;

    if (filename && image)
    {
        if ((fout = fopen(filename, "w+b")) != nil)
        {
//----------------------- Initializing Targa header --------------------------

            st_TARGAIMAGEHEADER.IDFieldPresence = 0;
            st_TARGAIMAGEHEADER.OriginX         = 0;
            st_TARGAIMAGEHEADER.OriginY         = 0;
            st_TARGAIMAGEHEADER.Width           = image->width;
            st_TARGAIMAGEHEADER.Height          = image->height;
            st_TARGAIMAGEHEADER.Depth           = image->depth;

            if (image->depth > 24)
                st_TARGAIMAGEHEADER.ImageAttributes = 8 | TGAATTRIB_INVERTED;
            else
                st_TARGAIMAGEHEADER.ImageAttributes = 0 | TGAATTRIB_INVERTED;

            if(image->depth > 8)
            {
                st_TARGAIMAGEHEADER.ColorMapPresence    = 0;
                st_TARGAIMAGEHEADER.ImageType           = 2;
                st_TARGAIMAGEHEADER.ColorMapOrigin      = 0;
                st_TARGAIMAGEHEADER.ColorMapLenght      = 0;
                st_TARGAIMAGEHEADER.ColorMapEntrySize   = 0;
            }
            else if(image->depth == 1)
            {
                st_TARGAIMAGEHEADER.ColorMapPresence    = 0;
                st_TARGAIMAGEHEADER.ImageType           = 3;
                st_TARGAIMAGEHEADER.ColorMapOrigin      = 0;
                st_TARGAIMAGEHEADER.ColorMapLenght      = 0;
                st_TARGAIMAGEHEADER.ColorMapEntrySize   = 0;
            }
            else
            {
                st_TARGAIMAGEHEADER.ColorMapPresence    = 1;
                st_TARGAIMAGEHEADER.ImageType           = 1;
                st_TARGAIMAGEHEADER.ColorMapOrigin      = 0;
                st_TARGAIMAGEHEADER.ColorMapLenght      = 256;
                st_TARGAIMAGEHEADER.ColorMapEntrySize   = 24;
            }

//----------------------------- Writing Header -------------------------------

            if (fwrite((PU8  )&st_TARGAIMAGEHEADER, sizeof(TARGAIMAGEHEADER), 1, fout))
            {

//--------------------- Writing palette if neccesary -------------------------

                if (!st_TARGAIMAGEHEADER.ColorMapPresence ||
                    (fwrite(image->palette, 0x300, 1, fout)))
                {
                    d_image_width = (float)image->depth / 8 * image->width;

//----------------------------- Writing image --------------------------------

                    if (fwrite(image->bitmap,d_image_width*image->height, 1, fout) > 0)
                    {
                        result = true;
                    }
                }
            }

            fclose(fout);
        }
    }

    return result;
}

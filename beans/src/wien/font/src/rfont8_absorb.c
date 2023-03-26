/* Font manipulation                                           Ver.0.01.007 */
/* ======================================================================== */
/*                                                         ROBOCODE PROJECT */
/* $ Various font manipulation routinues and functions                      */
/* $ development                                                            */
/*                                                                          */
/* $ NTASRCFLG: ANSI-C-OK                                                   */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<arsenal.h>
#include<datastorm.h>

#define DEFAULT_MAX_SYMBS       256
#define DEFAULT_MAP_SIZE        DEFAULT_MAX_SYMBS * sizeof(NTAFONTRASTERMAPENTRY)
#define DEFAULT_PALETTE_SIZE    256 * 3
#define OFFSET_TO_DATA          (DEFAULT_MAP_SIZE + DEFAULT_PALETTE_SIZE)
#define NTA_ASCIISPACESYMBOL    32


BOOL __is_column_empty(PNTAPICTURE pst_pict, DWORD d_column, BYTE b_transparent)
{
    BOOL    b_retcode = TRUE;
    DWORD   d_cnt;
    PBYTE   p_curr;

    p_curr = pst_pict->p_bitmap + d_column;

    for (d_cnt = 0; (d_cnt < pst_pict->d_height) && (*p_curr == b_transparent); d_cnt++)
        p_curr += pst_pict->d_width;

    if (d_cnt < pst_pict->d_height)
        b_retcode = FALSE;

    return b_retcode;
}

BOOL __is_row_empty(PNTAPICTURE pst_pict, DWORD d_column, DWORD d_row, DWORD d_width, BYTE b_transparent)
{
    BOOL    b_retcode = TRUE;
    DWORD   d_cnt;
    PBYTE   p_curr;

    p_curr = pst_pict->p_bitmap + d_column + (d_row * pst_pict->d_width);

    for (d_cnt = 0; (d_cnt < d_width) && (*p_curr == b_transparent); d_cnt++)
        p_curr++;

    if (d_cnt < d_width)
        b_retcode = FALSE;

    return b_retcode;
}

/* Function absorbs raster font from 8bit picture
------------------------------------------- krc_absorb_font_from_pict ----- */
PNTAFONT krc_absorb_font_from_pict(PNTAPICTURE pst_pict, PSTR psz_name, WORD w_start_symb, WORD w_max_space, BYTE b_transparent)
{
    PNTAFONT                pst_font            = NULL;
    DWORD                   d_column            = 0;
    DWORD                   d_char_start_x;
    DWORD                   d_char_start_y;
    DWORD                   d_char_width;
    DWORD                   d_char_height;
    DWORD                   d_space_width;
    DWORD                   d_max_width         = 0;
    DWORD                   d_max_height        = 0;
    DWORD                   d_char_start_y_corr = 0;
    DWORD                   d_cnt;
    PBYTE                   p_tmp;
    DWORD                   d_offset_on_data    = OFFSET_TO_DATA;
    PNTAFONTRASTERMAPENTRY  pst_map;
    PBYTE                   p_data;
    PBYTE                   p_data_curr;

    if (pst_pict && (pst_pict->b_depth == 8))
    {
        if ( ((pst_map = malloc(DEFAULT_MAP_SIZE)) != NULL)                       &&
             ((p_data  = malloc(pst_pict->d_width * pst_pict->d_height)) != NULL) )
        {
            memset(pst_map, 0, DEFAULT_MAP_SIZE);
            p_data_curr = p_data;

//----------------- Determining START Y correction amoumt --------------------

            while(__is_row_empty(pst_pict, 0, d_char_start_y_corr, pst_pict->d_width, b_transparent) &&
                d_char_start_y_corr < pst_pict->d_height)
                d_char_start_y_corr++;

//------------------------------- Main loop ----------------------------------

            while (d_column < pst_pict->d_width && w_start_symb < DEFAULT_MAX_SYMBS)
                if (!__is_column_empty(pst_pict, d_column, b_transparent))
                {

//----------------------- Determining symbol width ---------------------------

                    d_char_start_x = d_column;

                    do
                    {
                        while (!__is_column_empty(pst_pict, d_column , b_transparent) &&
                            d_column < pst_pict->d_width)
                            d_column++;

                        d_space_width  = d_column;
                        while (__is_column_empty(pst_pict, d_column , b_transparent) &&
                            d_column < pst_pict->d_width)
                            d_column++;
                        d_space_width = d_column - d_space_width;


                    } while (d_space_width <= w_max_space && d_column < pst_pict->d_width);

                    d_char_width = d_column - d_space_width - d_char_start_x;

                    if (d_max_width < d_char_width)
                        d_max_width = d_char_width;

//----------------------- Determining symbol height --------------------------

                    d_char_start_y = 0;
                    while(__is_row_empty(pst_pict, d_char_start_x, d_char_start_y, d_char_width, b_transparent) &&
                        d_char_start_y < pst_pict->d_height)
                        d_char_start_y++;

                    d_char_height = d_char_start_y;
                    d_space_width = 0;

                    do
                    {
                        d_char_height += d_space_width;

                        while(!__is_row_empty(pst_pict, d_char_start_x, d_char_height, d_char_width, b_transparent) &&
                            d_char_height < pst_pict->d_height)
                            d_char_height++;

                        d_space_width  = d_char_height;
                        while(__is_row_empty(pst_pict, d_char_start_x, d_space_width, d_char_width, b_transparent) &&
                            d_space_width < pst_pict->d_height)
                            d_space_width++;
                        d_space_width -= d_char_height;

                    } while (d_space_width <= d_max_height && d_char_height < pst_pict->d_height);

                    d_char_height -= d_char_start_y;

                    if (d_max_height < d_char_height)
                        d_max_height = d_char_height;

//---------------------------- Storing results -------------------------------

                    pst_map[w_start_symb].b_width   = (BYTE)d_char_width;
                    pst_map[w_start_symb].b_height  = (BYTE)d_char_height;
                    pst_map[w_start_symb].b_shift_x = 0;
                    pst_map[w_start_symb].b_shift_y = d_char_start_y - d_char_start_y_corr;
                    pst_map[w_start_symb].d_offset  = d_offset_on_data;

                    p_tmp = pst_pict->p_bitmap +
                            d_char_start_x     +
                            (pst_pict->d_width * d_char_start_y);

                    for (d_cnt = 0; d_cnt < d_char_height; d_cnt++)
                    {
                        memcpy(p_data_curr, p_tmp, d_char_width);
                        p_tmp       += pst_pict->d_width;
                        p_data_curr += d_char_width;
                    }
                    d_offset_on_data += (d_char_width * d_char_height);

                    //printf("CHAR %d: %d, y: %d, width: %d, height: %d, shift y: %d\n", w_start_symb, d_char_start_x, d_char_start_y, d_char_width, d_char_height, pst_map[w_start_symb].b_shift_y);
                    w_start_symb++;
                }
                else
                    d_column++;

            if ( ((pst_font         = malloc(sizeof(NTAFONT)))  != NULL) &&
                 ((pst_font->p_data = malloc(d_offset_on_data)) != NULL) )
            {
                pst_font->w_charset     = 0;
                pst_font->v_attrib      = NTAFA_RASTER;
                pst_font->b_width       = d_max_width;
                pst_font->b_height      = d_max_height;
                pst_font->b_hspace      = 1;
                pst_font->b_vspace      = 1;
                pst_font->w_symb_number = DEFAULT_MAX_SYMBS;
                pst_font->d_data_size   = d_offset_on_data;

                if (!pst_map[NTA_ASCIISPACESYMBOL].d_offset)
                    pst_map[NTA_ASCIISPACESYMBOL].b_shift_x = pst_font->b_hspace << 2;

                memcpy((PBYTE)pst_font->p_data,
                       pst_map,
                       DEFAULT_MAP_SIZE);

                memcpy((PBYTE)pst_font->p_data + DEFAULT_MAP_SIZE,
                       pst_pict->p_palette,
                       DEFAULT_PALETTE_SIZE);

                memcpy((PBYTE)pst_font->p_data + DEFAULT_MAP_SIZE + DEFAULT_PALETTE_SIZE,
                       p_data,
                       d_offset_on_data - OFFSET_TO_DATA);

                strcpy(pst_font->sz_name, psz_name);
            }

            free(pst_map);
            free(p_data);
        }
    }

    return pst_font;
}

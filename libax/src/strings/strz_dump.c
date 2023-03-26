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

#include <stdio.h>
#include <string.h>
#include <axstring.h>
#include <limits.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      strz_dump
// PURPOSE
//
// PARAMETERS
//      PCSTR psz_message --
//      PVOID p_data      --
//      UINT  u_size      --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL strz_dump(PCSTR        psz_message,
               PVOID        p_data,
               UINT         u_size)
{
    BOOL            b_result        = false;
    PSTR            psz_dump;

    ENTER(p_data && u_size);

    if ((psz_dump = MALLOC((u_size << 1) + sizeof(CHAR))) != NULL)
    {
        strz_bin2hex(psz_dump, p_data, u_size);

#ifndef USE_NO_PRINTF
        printf("%s%s\n", psz_message ? psz_message : "", psz_dump);
#endif

        b_result = true;

        FREE(psz_dump);
    }
    
    RETURN(b_result);
}
BOOL strz_dump_w_txt(PCSTR        psz_message,
                     PVOID        p_data,
                     UINT         u_size)
{
    BOOL            b_result        = false;
    PSTR            psz_dump;
    PSTR            on              = (PSTR)p_data;
    PSTR            end             = on + u_size;

    ENTER(p_data && u_size);

    if ((psz_dump = MALLOC((u_size << 1) + sizeof(CHAR))) != NULL)
    {
        strz_bin2hex(psz_dump, p_data, u_size);

#ifndef USE_NO_PRINTF
        printf("%s%s | ", psz_message ? psz_message : "", psz_dump);

        while (on < end)
        {
            printf("%c", ((*on >= ' ') && ((*on <= '~'))) ? *on: '.');
            on++;
        }
        printf("\n");
#endif

        b_result = true;

        FREE(psz_dump);
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      la6_bin2hex
// PURPOSE
//      Render hexadecimal digits to string without delimeters
//      and U8 ordering
// PARAMETERS
//      PSTR  psz_string -- Pointer to target string
//      PU8    p_value    -- Pointer to value buffer
//      UINT d_size     -- Size of buffer
// RESULT
//      PSTR -- Pointer to end of rendered buffer
// ***************************************************************************
PSTR strz_bin2hex(PSTR      string,
                  PVOID     data,
                  UINT      size)
{
    PU8     onData      = data;
    PSTR    onStr       = string;
    UINT    left        = size;

    if (onStr && onData && left)
    {
        *(onStr + (left << 1)) = 0;

        for (; left; onData++, left--)
        {
            *(onStr++) = (CHAR)(MAC_BIN2HEX((*onData >> 4)));
            *(onStr++) = (CHAR)(MAC_BIN2HEX(*onData));
        }
    }

    return onStr;
}
// ***************************************************************************
// FUNCTION
//      la6_bin2BEhex
// PURPOSE
//      Render data buffer to hex string in Big Endian byte order
// PARAMETERS
//      PSTR  psz_string -- Pointer to target string
//      PVOID p_data     -- Pointer to value buffer
//      DWORD d_size     -- Size of buffer
// RESULT
//      PSTR -- Pointer to end of rendered buffer
// ***************************************************************************
PSTR strz_bin2BEhex(PSTR        psz_string,
                    PVOID       p_data,
                    UINT        d_size)
{
    PSTR    psz_ret         = psz_string;
    PU8     p_on_data       = p_data;
    PSTR    psz_on_string;


    if (psz_string && p_data && d_size)
    {
        psz_ret += (d_size << 1);

        for (psz_on_string = psz_ret; d_size--; p_on_data++)
        {
            *(--psz_on_string) = MAC_BIN2HEX(*p_on_data);
            *(--psz_on_string) = MAC_BIN2HEX(*p_on_data >> 4);
        }
    }

    return psz_ret;
}
PSTR strz_BEhex2bin(PVOID   p_data,
                    PSTR    psz_string,
                    UINT    d_size)
{
    PSTR    psz_ret         = psz_string;
    PU8     p_on_data       = p_data;
    PSTR    psz_on_string;


    if (psz_string && p_data && d_size)
    {
        psz_ret += (d_size << 1);

        for ( psz_on_string = psz_ret - DEF_OCTET_CHARS;
              d_size--;
              p_on_data++, psz_on_string -= DEF_OCTET_CHARS)
        {
            *p_on_data = (MAC_HEX2BIN(*(psz_on_string + 0)) << 4) |
                          MAC_HEX2BIN(*(psz_on_string + 1));
        }
    }

    return psz_ret;
}

// ***************************************************************************
// FUNCTION
//      la6_val2hex
// PURPOSE
//      Render binary value to hex string in Big Endian byte order
// PARAMETERS
//      PSTR  psz_string -- Pointer to target string
//      PVOID p_value    -- Pointer to value buffer
//      DWORD d_size     -- Size of buffer
// RESULT
//      PSTR -- Pointer to end of rendered buffer
// ***************************************************************************
PSTR strz_val2hex(PSTR psz_string, U32 d_value, UINT d_size)
{
    PSTR    psz_on_string   = psz_string;

    if (d_size <= sizeof(U32))
        psz_on_string = strz_bin2BEhex(psz_string, &d_value, d_size);

    return psz_on_string;
}


// ***************************************************************************
// FUNCTION
//      strz_hex2bin
// PURPOSE
//
// PARAMETERS
//      PVOID data   --
//      UINT  size   --
//      PCSTR string --
// RESULT
//      INT --
// ***************************************************************************
INT strz_hex2bin(PVOID        data,
                 UINT         size,
                 PCSTR        string)
{
#define DIG(a) dig -= (a); if (stage) *(ontgt++) |= dig; else *ontgt = (dig << 4); stage = !stage

    INT     result      = -1;
    PU8      ontgt       = data;
    PCSTR   onsrc       = string;
    PU8      end         = ontgt + size;
    BOOL    stage       = false;
    CHAR    dig;

    ENTER(ontgt && onsrc);

    for (; (ontgt < end)  && *onsrc; onsrc++)
    {
        dig = *onsrc;

        if ((dig >= '0') && (dig <= '9'))
        {
            DIG(48);
        }
        else
        {
            dig |= 0x20;

            if ((dig >= 'a') && (dig <= 'f'))
            {
                DIG(87);
            }
        }
    }

    if (!stage)
        result = ontgt - ((PU8)data);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      strz_val2bits
// PURPOSE
//
// PARAMETERS
//      PSTR targetString --
//      UINT value        --
//      int  bitsNum      --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL strz_val2bits(PSTR     targetString,
                   UINT     value,
                   UINT     bitsNum)
{
    BOOL    b_result        = false;
    UINT    d_tmp           = value;
    UINT    i_cnt           = bitsNum;
    PSTR    psz_on_string;


    if (targetString && bitsNum && (bitsNum <= (sizeof(UINT) * CHAR_BIT)))
    {
        psz_on_string       = targetString + bitsNum;
        *(psz_on_string--)  = 0;

        while (i_cnt--)
        {
            *(psz_on_string--) = ('0' + ((CHAR)d_tmp & 1));
            d_tmp >>= 1;
        }

        b_result = true;
    }

    return b_result;
}
// ***************************************************************************
// FUNCTION
//      strz_bits2val
// PURPOSE
//
// PARAMETERS
//      PCSTR psz_string --
// RESULT
//      UINT --
// ***************************************************************************
UINT strz_bits2val(PCSTR psz_string)
{
    UINT        d_value             = 0;
    PCSTR       on                  = psz_string;

    if (psz_string)
    {
        while (*on)
        {
            d_value <<= 1;
            d_value |= ((*(on++) - '0') & 1);
        }
    }

    return d_value;
}
// ***************************************************************************
// FUNCTION
//      strz_sdump
// PURPOSE
//      Dump memory region to string
// PARAMETERS
//      PSTR  psz_string     -- Pointer to target string
//      UINT  d_string_width -- Width of string in octets
//      U32   d_separators   -- 4 combined separator symbols
//                              (see definitions in header)
//      PVOID p_data         -- Pointer to data
//      UINT  d_size         -- Size of data
// RESULT
//      PSTR -- Pointer to end of buffer
// NOTE
//      Target string must be about 5 times bigger, than size of dumping data
// ***************************************************************************
PSTR strz_sdump(PSTR               psz_string,
                UINT               d_string_width,
                U32                d_separators,
                PVOID              p_data,
                UINT               d_size)
{
    UINT    d_string_size       = (d_string_width << 2);
    UINT    d_cnt;
    PU8     p_on_data           = p_data;
    PSTR    p_ascii_string;
    PSTR    p_hex_string        = psz_string;
    U8      b_dd_ileave         = 1;
    U8      b_string_ileave     = 1;


    if (psz_string && d_string_width && p_data && d_size)
    {
        if (!d_separators)
        {
            d_separators = AXDUMP_SEP_DEFAULT;
        }

// ---------------------- Calculating string width ---------------------------

        // For each U8: 2 chars + U8 sep. + U8 char in string = 4 U8s

        if ((d_separators & AXDUMP_ADD_SEP_SPACE) && (((d_separators >> 8) & 0xff) > 32))
        {
                                            // + UINT separators addition
                                            // space
            d_string_size   += (((d_string_width - 1) >> 2) << 1);
            b_dd_ileave      = 3;
        }

        if ((d_separators & AXDUMP_ADD_SEP_SPACE) && (((d_separators >> 16) & 0xff) > 32))
        {
                                            // + string separator addition
                                            // space
            d_string_size   += 2;
            b_string_ileave  = 2;
        }

// ----------------------- Prepare string and variables ----------------------

        p_ascii_string = psz_string + d_string_size - d_string_width;

        memset(psz_string, 32, d_string_size);
        *(psz_string + d_string_size) = 0;
                                            // Cleaning string

        for (   d_cnt =  (11 + (b_dd_ileave >> 1))               ;
                d_cnt <  (d_string_size - d_string_width - b_dd_ileave) ;
                d_cnt += (11 + b_dd_ileave))
            *(p_hex_string + d_cnt) = (U8)(d_separators >> 8);
                                            // UINT separators

        *(p_ascii_string - b_string_ileave) = (U8)(d_separators >> 16);
                                            // String separator

// ------------------------------- HEX values --------------------------------
        if (d_size > d_string_width)
            d_size = d_string_width;

        for (d_cnt = 1; d_cnt <= d_size; d_cnt++)
        {
            // Creating hex value
            *(p_hex_string++) = MAC_BIN2HEX(*p_on_data >> 4);
            *(p_hex_string++) = MAC_BIN2HEX(*p_on_data);

            if (d_cnt & 3)
                *(p_hex_string++) = (U8)(d_separators);
            else
                p_hex_string += b_dd_ileave;

            // Adding char to ASCII string
            if ((d_separators & AXDUMP_SHOW_UNPRINT) || (*(p_on_data) > 31))
            {
                *(p_ascii_string++) = *(p_on_data++);
            }
            else
            {
                *(p_ascii_string++) = '.';
                p_on_data++;
            }
        }
    }

    return p_hex_string;
}
// ***************************************************************************
// FUNCTION
//      strz_sdump_to_dev
// PURPOSE
//
// PARAMETERS
//      PAXDEV dev         --
//      UINT   shownOffset --
//      UINT   stringWidth --
//      U32    separators  --
//      PVOID  data        --
//      UINT   size        --
// RESULT
//      void --
// ***************************************************************************
void strz_sdump_to_dev(PAXDEV             dev,
                       UINT               shownOffset,
                       UINT               stringWidth,
                       U32                separators,
                       PVOID              data,
                       UINT               size)
{
    PU8         p_on;
    UINT        u_left;
    UINT        u_to;
    UINT        width       = stringWidth ? stringWidth : 16;
    UINT        offset      = shownOffset;
    PSTR        dump;
    PSTR        on;

    ENTER(true);

    if ((dump = MALLOC(width * 5)) != nil)
    {
        for (  p_on = data, u_left = size;
               u_left;
               u_left -= u_to, p_on += u_to)
        {
            u_to = (u_left > width) ? width : u_left;

            sprintf(dump, "%.8X  ", offset);

            axdev_write(dev, dump, 10, -1);

            on = strz_sdump(dump, width, separators, p_on, u_to);

            axdev_write(dev, dump, on - dump, -1);
            axdev_write(dev, "\n", 1, -1);

            offset += u_to;
        }

        FREE(dump);
    }

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      strz_sdump_print
// PURPOSE
//
// PARAMETERS
//      UINT  shownOffset --
//      UINT  stringWidth --
//      U32   separators  --
//      PVOID data        --
//      UINT  size        --
// RESULT
//      void --
// ***************************************************************************
void strz_sdump_print(UINT               shownOffset,
                      UINT               stringWidth,
                      U32                separators,
                      PVOID              data,
                      UINT               size)
{
//    PAXDEV             con =
//    strz_sdump_to_dev(con, shownOffset, stringWidth, separators, data, size);

    PU8         p_on;
    UINT        u_left;
    UINT        u_to;
    UINT        width       = stringWidth ? stringWidth : 16;
    UINT        offtset     = shownOffset;
    PSTR        dump;

    ENTER(true);

    if ((dump = MALLOC(width * 5)) != nil)
    {
        for (  p_on = data, u_left = size;
               u_left;
               u_left -= u_to, p_on += u_to)
        {
            u_to = (u_left > width) ? width : u_left;

            strz_sdump(dump, width, separators, p_on, u_to);

#ifndef USE_NO_PRINTF
            printf("%.8X  %s\n", offtset, dump);
#endif
            offtset += u_to;
        }

        FREE(dump);
    }

    QUIT;
}

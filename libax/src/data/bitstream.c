// ***************************************************************************
// TITLE
//      Bitstream Manipulation
// PROJECT
//      Arsenal Library
// ***************************************************************************
//
// FILE
//      $Id: bits.c,v 1.3 2003/08/05 08:48:05 A.Urakhchinskiy Exp $
// AUTHOR
//      $Author: A.Urakhchinskiy $
// HISTORY
//      $Log: bits.c,v $
//      Revision 1.3  2003/08/05 08:48:05  A.Urakhchinskiy
//      no message
//
//      Revision 1.2  2003/04/24 10:07:28  A.Kozhukhar
//      *** empty log message ***
//
//      Revision 1.1.1.1  2003/02/14 13:17:30  A.Kozhukhar
//      Initial release
//
// ***************************************************************************


#include <stdlib.h>
#include <string.h>

#include <arsenal.h>

#include <axdata.h>

// ***************************************************************************
// STRUCTURE
//      AXBITSTREAM
// PURPOSE
//
// ***************************************************************************
typedef struct _tag_AXBITSTREAM
{
    PU8             p_data;
    PU8             p_curr_data;
    PU8             p_EOD;                  // End OF Data
    U32             d_buffer;
    U8              b_in_buffer;
    U8              b_stream_format;
} AXBITSTREAM, * PAXBITSTREAM;



// ---------------------------------------------------------------------------
// -------------------------- LOCAL DEFINITIONS ------------------------------
// ---------------------------------------------------------------------------

#define ADD_HI_BITS(a, b, c, d)         ((a << (d - b)) | (((U32)c) << d)) >> (d - b)



// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      la6_get_hex_octets
// PURPOSE
//      Convert up to 4 text hexadercimal octets to DWORD
// PARAMETERS
//      PBYTE   * psz_string -- Source string
//      DWORD     d_octets   -- Number of octets
// RESULT
//      DWORD -- Converted value
// ***************************************************************************
U32 la6_get_hex_octets(PU8 * psz_string, UINT d_octets)
{
    U32 d_res = 0;

    if (psz_string && *psz_string && d_octets)
    {
        d_octets <<= 1;                     // Octets to nibbles

        for (; d_octets--; (*psz_string)++)
        {
            d_res <<= 4;

            if ((*(*psz_string) > 47) && (*(*psz_string) < 58))
                d_res |= (*(*psz_string) - 48);
            else
                d_res |= ((*(*psz_string) | 0x20) - 87);
        }
    }

    return d_res;
}

// ***************************************************************************
// FUNCTION
//      la6_put_hex_octets
// PURPOSE
//      Convert up to 4 octets to hexadecimal string
// PARAMETERS
//      PBYTE   * psz_string -- Target string
//      DWORD     d_octets   -- Number of octets
//      DWORD     d_value    -- Value to convert
// RESULT
//      none
// ***************************************************************************
void la6_put_hex_octets(PU8 * psz_string, UINT d_octets, U32 d_value)
{
    U8          b_value;

    if (psz_string && *psz_string && d_octets)
    {
        d_value  <<= ((4 - d_octets) << 3);
        d_octets <<= 1;                     // Octets to nibbles

        for (; d_octets--; (*psz_string)++)
        {
            b_value   = (U8)((d_value >> 28) & 0xF);
            d_value <<= 4;

            if (b_value < 10)
                *(*psz_string) = (b_value + 48);
            else
                *(*psz_string) = (b_value + 55);
        }
    }
}

// ***************************************************************************
// FUNCTION
//      bitstream_get_actual_size
// PURPOSE
//      Return atual size of the Bitstream's data in bytes
// PARAMETERS
//      PAXBITSTREAM pst_bits -- Pointer to Bits Stream control structure
// RESULT
//      DWORD -- Size in bytes of actual Bitstream data size
// ***************************************************************************
UINT bitstream_get_actual_size(HBITSTREAM h_bits)
{
    PAXBITSTREAM pst_bits = (PAXBITSTREAM)h_bits;

    UINT d_size = 0;

    if (pst_bits)
        d_size = (UINT)(pst_bits->p_curr_data - pst_bits->p_data);

    return d_size;
}

// ***************************************************************************
// FUNCTION
//      bitstream_destroy
// PURPOSE
//      Destroy Bitstream control structure
// PARAMETERS
//      PAXBITSTREAM pst_bits -- Pointer to Bitstream control structure
// RESULT
//      PAXBITSTREAM -- Always NULL
// ***************************************************************************
HBITSTREAM bitstream_destroy(HBITSTREAM h_bits)
{
    if (h_bits)
    {
        FREE(h_bits);
    }

    return nil;
}

// ***************************************************************************
// FUNCTION
//      bitstream_create
// PURPOSE
//      Create Bitstream control structure
// PARAMETERS
//      PU8         p_data          -- Bitstream data buffer
//      DWORD         d_size          -- Size of buffer
//      BYTE          b_stream_format -- Bitstream data buffer format
//                                       LA6BS_BINARY: Binary data
//                                       LA6BS_HEX: Hexadecimal string
// RESULT
//      PAXBITSTREAM -- Pointer to the just created Bitstream control
//                       structure if all is ok, NULL if not
// ***************************************************************************
HBITSTREAM bitstream_create(PVOID p_data, UINT d_size, U8 b_stream_format)
{
    PAXBITSTREAM   pst_bits = nil;

    if (p_data && d_size)
    {
        if ((pst_bits = MALLOC_D(sizeof(AXBITSTREAM), "BITSTREAM")) != NULL)
        {
            memset(pst_bits, 0, sizeof(AXBITSTREAM));
            pst_bits->p_data            = p_data;
            pst_bits->p_curr_data       = p_data;
            pst_bits->p_EOD             = (char*)(p_data) + d_size;
            pst_bits->b_stream_format   = b_stream_format;
        }
    }

    return (HBITSTREAM)pst_bits;
}

// ***************************************************************************
// FUNCTION
//      bitstream_get
// PURPOSE
//      Get up to 24 bits from Bitstream
// PARAMETERS
//      PAXBITSTREAM pst_bits         -- Pointer to Bitstream structure
//      BYTE          b_number_of_bits -- Number of bits to get
// RESULT
//      DWORD -- Bits value
// ***************************************************************************
U32 bitstream_get(HBITSTREAM h_bits, INT b_number_of_bits)
{
    PAXBITSTREAM    pst_bits        = (PAXBITSTREAM)h_bits;
    U32             d_ret_buffer    = -1;
    U8              b_byte          = 0;

    if (pst_bits && (b_number_of_bits < 25))
    {
        // Filling buffer
        while ((pst_bits->b_in_buffer < b_number_of_bits) &&
               (pst_bits->p_curr_data < pst_bits->p_EOD)  )
        {
            switch (pst_bits->b_stream_format)
            {
                case AXBSF_BINARY: b_byte = *(pst_bits->p_curr_data++);                             break;
                case AXBSF_HEX:    b_byte = (U8)la6_get_hex_octets(&pst_bits->p_curr_data, 1);    break;
            }

            pst_bits->d_buffer     = ADD_HI_BITS(pst_bits->d_buffer, pst_bits->b_in_buffer, b_byte, 24);
            pst_bits->b_in_buffer += 8;
        }

        // If no error occured
        if (pst_bits->b_in_buffer >= b_number_of_bits)
        {
            d_ret_buffer            = pst_bits->d_buffer & ((1 <<  b_number_of_bits) - 1);
            pst_bits->d_buffer    >>= b_number_of_bits;
            pst_bits->b_in_buffer  -= b_number_of_bits;
        }
    }

    return d_ret_buffer;
}

// ***************************************************************************
// FUNCTION
//      bitstream_put
// PURPOSE
//      Put up to 24 bits to Bitstream
// PARAMETERS
//      PAXBITSTREAM pst_bits         -- Pointer to Bitstream structure
//      BYTE          b_number_of_bits -- Number of bits to put
//      DWORD         d_value          -- Bits value
// RESULT
//      LA6TALKCODE -- LA6TC_ok if ok or error code otherwise
// ***************************************************************************
INT bitstream_put(HBITSTREAM h_bits, INT b_number_of_bits, U32 d_value)
{
    PAXBITSTREAM    pst_bits        = (PAXBITSTREAM)h_bits;
    INT             result          = -1; // LA6TC_fatal;

    if (pst_bits && (b_number_of_bits < 25))
    {
        // Forcing buffer flushing if b_number_of_bits = 0
        if ((!b_number_of_bits) && pst_bits->b_in_buffer)
            pst_bits->b_in_buffer += 8;

        // Flushing buffer
        while ((pst_bits->b_in_buffer > 8) &&
               (pst_bits->p_curr_data < pst_bits->p_EOD) )
        {
            switch (pst_bits->b_stream_format)
            {
                case AXBSF_BINARY: *(pst_bits->p_curr_data++) = (U8)pst_bits->d_buffer;               break;
                case AXBSF_HEX:    la6_put_hex_octets(&pst_bits->p_curr_data, 1, pst_bits->d_buffer);   break;
            }
            pst_bits->d_buffer    >>= 8;
            pst_bits->b_in_buffer  -= 8;
        }

        if (b_number_of_bits && (pst_bits->p_curr_data < pst_bits->p_EOD))
        {
            pst_bits->d_buffer       = ADD_HI_BITS(pst_bits->d_buffer, pst_bits->b_in_buffer, d_value, 8);
            pst_bits->b_in_buffer   += b_number_of_bits;
            result                   = b_number_of_bits;
        }
    }

    return result;
}

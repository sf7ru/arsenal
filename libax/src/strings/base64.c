// ***************************************************************************
// TITLE
//      Base64 encoding
// PROJECT
//      Arsenal Library
// ***************************************************************************
//
// FILE
//      $Id:$
// HISTORY
//      $Log:$
// ***************************************************************************


#include <string.h>

#include <arsenal.h>
#include <axstring.h>





// ---------------------------------------------------------------------------
// ----------------------------- DEFINITIONS ---------------------------------
// ---------------------------------------------------------------------------

#define DEF_PAD             '='


// ---------------------------------------------------------------------------
// ------------------------------ CONSTANTS ----------------------------------
// ---------------------------------------------------------------------------

static PU8        g_ab_table = (PU8)
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";



// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _char2code
// PURPOSE
//
// PARAMETERS
//      CHAR c --
// RESULT
//      int --
// ***************************************************************************
static int _char2code(CHAR c)
{
    int     code    = -1;

    if (c >= '0')
    {
        if (c >= 'A')
        {
            if (c >= 'a')
            {
                if (c <= 'z')
                {
                    code = c - 71;
                }
            }
            else if (c <= 'Z')
            {
                code = c - 'A';
            }
        }
        else if (c <= '9')
        {
            code = c + 4;
        }
        else if (c == '=')
        {
            code = -3;
        }
    }
    else switch(c)
    {
        case '+' : code = c + 19; break;
        case '/' : code = c + 16; break;
        case ' ' :
        case '\t':
        case '\r':
        case '\n': code = -2; break;
    }

//    printf("char '%c' = code %d\n", c, code);

    return code;
}
// ***************************************************************************
// FUNCTION
//      strz_base64_decode
// PURPOSE
//
// PARAMETERS
//      PVOID p_data     --
//      UINT  u_size     --
//      PSTR  psz_source --
// RESULT
//      UINT --
// ***************************************************************************
UINT strz_base64_decode(PVOID        p_data,
                   UINT         u_size,
                   PSTR         psz_source)
{
    UINT            u_decoded       = 0;
    PSTR            psz_on          = psz_source;
    PU8           p_to            = p_data;
    UINT            u_left          = u_size;
    UINT            u_bits;
    PU8           p_found;
    UINT           d_buff;


    ENTER(psz_on && p_to && u_left);

    while (*psz_on && u_left)
    {
// --------------------------- Filling buffer --------------------------------

        for (d_buff = 0, u_bits = 0; *psz_on && (u_bits < 24); psz_on++)
        {
            if (*psz_on != DEF_PAD)
            {
                if ((p_found = (PU8)strchr((PSTR)g_ab_table, *psz_on)) != NULL)
                {
                    d_buff  = (d_buff << 6) | (p_found - g_ab_table);
                    u_bits += 6;
                }
            }
        }

// --------------------------- Flushing buffer -------------------------------

        for (; u_left && (u_bits >= 8); p_to++, u_left--)
        {
            u_bits  -= 8;
            *p_to    = (U8)(d_buff >> u_bits);
            u_decoded++;
        }
    }

    if (*psz_on)
        u_decoded = 0;

    RETURN(u_decoded);
}
// ***************************************************************************
// FUNCTION
//      strz_base64_encode
// PURPOSE
//
// PARAMETERS
//      PSTR  psz_target --
//      UINT  u_tgt_size --
//      PVOID p_data     --
//      UINT  u_src_size --
// RESULT
//      UINT --
// ***************************************************************************
UINT strz_base64_encode(PSTR         psz_target,
                        UINT         u_tgt_size,
                        PVOID        p_data,
                        UINT         u_src_size)
{
    UINT            u_encoded       = 0;
    PU8             p_on            = p_data;
    UINT            u_src_left      = u_src_size;
    PSTR            psz_to          = psz_target;
    UINT            u_tgt_left      = u_tgt_size;
    UINT            u_bits;
    UINT            u_bits_pad;
    UINT           d_buff;


    ENTER(p_on && psz_to && u_src_left && u_tgt_left);


    while (u_src_left)
    {
// --------------------------- Filling buffer --------------------------------

        for (d_buff = 0, u_bits = 0; u_src_left && (u_bits < 24); u_src_left--, p_on++)
        {
            d_buff    <<= 8;
            u_bits     += 8;
            d_buff     |= *p_on;
        }

// --------------------------- Flushing buffer -------------------------------

        u_bits_pad      = 24 - u_bits;
        d_buff        <<= u_bits_pad;

        for (; u_tgt_left && u_bits; u_tgt_left--)
        {
            d_buff    <<= 6;
            u_bits     -= MAC_MIN(u_bits, 6);

            *(psz_to++) = g_ab_table[(d_buff >> 24) & 0x3F];
            u_encoded++;
        }

// ------------------------------- Padding -----------------------------------

        for (; u_tgt_left && (u_bits_pad >= 6); u_tgt_left--)
        {
            *(psz_to++)  = '=';
            u_bits_pad  -= 6;
            u_encoded++;
        }

    }

    if (u_src_left)
        u_encoded = 0;
    else
        *psz_to = 0;

    RETURN(u_encoded);
}
// ***************************************************************************
// FUNCTION
//      strz_base64_dec
// PURPOSE
//
// PARAMETERS
//      PVOID tgt     --
//      UINT  tgtsize --
//      PCSTR src     --
// RESULT
//      UINT --
// ***************************************************************************
UINT strz_base64_dec(PVOID      tgt,
                     UINT       tgtsize,
                     PCSTR      src)
{
    UINT        size    = 0;
    PCSTR       on;
    PU8       to;
    UINT        left;
    U32      buff    = 0;
    int         tail    = 0;
    int         count;
    int         code;

    on  = src;
    to  = tgt;
    left = tgtsize;
    count = 0;

    while (left && *on)
    {
        if (count < 4)
        {
            switch (code = _char2code(*(on++)))
            {
                case -1: // error
                    left = 0;
                    size = 0;
                    break;

                case -2: // white space
                    break;

                case -3: // tail symbol
                    count++;
                    tail++;
                    buff <<= 6;
                    break;

                default:
                    if (!tail)
                    {
                        count++;
                        buff <<= 6;
                        buff  |= code;
                    }
                    break;
            }
        }

        if (count == 4)
        {
            for (count--; left && (count > tail); count--)
            {
                *(to++) = (U8)((buff >> ((count - 1) * 8)) & 0xff);
                left--;
                size++;
            }

            count -= tail;
        }
    }

    return size;
}

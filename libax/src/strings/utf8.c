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

#include <stdlib.h>

#include <arsenal.h>

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// ---------------------------------------------------------------------------

static const U32 _cp1251_table   [ 128 ] =
{
	0x00000802, 0x00000803, 0x0000801A, 0x00000893,
	0x0000801E, 0x00008026, 0x00008020, 0x00008021,
	0x0000812C, 0x00008030, 0x00000809, 0x00008039,
	0x0000080A, 0x0000080C, 0x0000080B, 0x0000080F,
	0x00000892, 0x00008018, 0x00008019, 0x0000801C,
	0x0000801D, 0x00008022, 0x00008013, 0x00008014,
	0x00000000, 0x00008222, 0x00000899, 0x0000803A,
	0x0000089A, 0x0000089C, 0x0000089B, 0x0000089F,
	0x00000120, 0x0000080E, 0x0000089E, 0x00000808,
	0x00000124, 0x00000910, 0x00000126, 0x00000127,
	0x00000801, 0x00000129, 0x00000804, 0x0000012B,
	0x0000012C, 0x0000012D, 0x0000012E, 0x00000807,
	0x00000130, 0x00000131, 0x00000806, 0x00000896,
	0x00000911, 0x00000135, 0x00000136, 0x00000137,
	0x00000891, 0x00008216, 0x00000894, 0x0000013B,
	0x00000898, 0x00000805, 0x00000895, 0x00000897,
	0x00000810, 0x00000811, 0x00000812, 0x00000813,
	0x00000814, 0x00000815, 0x00000816, 0x00000817,
	0x00000818, 0x00000819, 0x0000081A, 0x0000081B,
	0x0000081C, 0x0000081D, 0x0000081E, 0x0000081F,
	0x00000820, 0x00000821, 0x00000822, 0x00000823,
	0x00000824, 0x00000825, 0x00000826, 0x00000827,
	0x00000828, 0x00000829, 0x0000082A, 0x0000082B,
	0x0000082C, 0x0000082D, 0x0000082E, 0x0000082F,
	0x00000830, 0x00000831, 0x00000832, 0x00000833,
	0x00000834, 0x00000835, 0x00000836, 0x00000837,
	0x00000838, 0x00000839, 0x0000083A, 0x0000083B,
	0x0000083C, 0x0000083D, 0x0000083E, 0x0000083F,
	0x00000880, 0x00000881, 0x00000882, 0x00000883,
	0x00000884, 0x00000885, 0x00000886, 0x00000887,
	0x00000888, 0x00000889, 0x0000088A, 0x0000088B,
	0x0000088C, 0x0000088D, 0x0000088E, 0x0000088F
};

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

static const U32 cp1251_table    [128]   =
{
        0x82D0,0x83D0,0x9A80E2,0x93D1,0x9E80E2,0xA680E2,0xA080E2,0xA180E2,
        0xAC82E2,0xB080E2,0x89D0,0xB980E2,0x8AD0,0x8CD0,0x8BD0,0x8FD0,
        0x92D1,0x9880E2,0x9980E2,0x9C80E2,0x9D80E2,0xA280E2,0x9380E2,0x9480E2,
        0,0xA284E2,0x99D1,0xBA80E2,0x9AD1,0x9CD1,0x9BD1,0x9FD1,
        0xA0C2,0x8ED0,0x9ED1,0x88D0,0xA4C2,0x90D2,0xA6C2,0xA7C2,
        0x81D0,0xA9C2,0x84D0,0xABC2,0xACC2,0xADC2,0xAEC2,0x87D0,
        0xB0C2,0xB1C2,0x86D0,0x96D1,0x91D2,0xB5C2,0xB6C2,0xB7C2,
        0x91D1,0x9684E2,0x94D1,0xBBC2,0x98D1,0x85D0,0x95D1,0x97D1,
        0x90D0,0x91D0,0x92D0,0x93D0,0x94D0,0x95D0,0x96D0,0x97D0,
        0x98D0,0x99D0,0x9AD0,0x9BD0,0x9CD0,0x9DD0,0x9ED0,0x9FD0,
        0xA0D0,0xA1D0,0xA2D0,0xA3D0,0xA4D0,0xA5D0,0xA6D0,0xA7D0,
        0xA8D0,0xA9D0,0xAAD0,0xABD0,0xACD0,0xADD0,0xAED0,0xAFD0,
        0xB0D0,0xB1D0,0xB2D0,0xB3D0,0xB4D0,0xB5D0,0xB6D0,0xB7D0,
        0xB8D0,0xB9D0,0xBAD0,0xBBD0,0xBCD0,0xBDD0,0xBED0,0xBFD0,
        0x80D1,0x81D1,0x82D1,0x83D1,0x84D1,0x85D1,0x86D1,0x87D1,
        0x88D1,0x89D1,0x8AD1,0x8BD1,0x8CD1,0x8DD1,0x8ED1,0x8FD1
};

/*
// ***************************************************************************
// FUNCTION
//      _recode
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//      void --
// ***************************************************************************
static void _recode(void)
{
    UINT            u_cnt;
    U32          value;
    PU8           p_on;

    ENTER(true);

    for (u_cnt = 0; u_cnt < 128; u_cnt++)
    {
        p_on    = (PU8)&cp1251_table[u_cnt];
        value   = _read_utf8(&p_on, p_on + 4);

        A7TRACE("0x%.8X, ", (int)value);
    }

    QUIT;
}
*/
// ***************************************************************************
// FUNCTION
//      psz_end)
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
// RESULT
//      UINT3_read_utf8PU8 * ppsz_src,PU8 --
// ***************************************************************************
static U32 _read_utf8(PU8 *    ppsz_src,
                         PU8      psz_end)
{
    U32          u_char      = 0;
    UINT            u_num       = 0;
    U8            c_tmp;

    ENTER(true);

    if (*ppsz_src < psz_end)
    {
        c_tmp = *(*ppsz_src)++;

        if (c_tmp & 0x80)
        {
            while (c_tmp & (1 << (6 - u_num)))
            {
                u_num++;
            }

            u_char = c_tmp & ((1 << (6 - u_num)) - 1);

            for (; u_num && (*ppsz_src < psz_end); u_num--)
            {
                c_tmp = *(*ppsz_src)++;
                u_char = (u_char << 7) | (c_tmp & 0x7F);
            }
        }
        else
            u_char = c_tmp;
    }

    RETURN(u_char);
}
// ***************************************************************************
// FUNCTION
//      _find_cp1251
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
// RESULT
//      CHAR --
// ***************************************************************************
static U8 _find_cp1251(U32 u_char)
{
    U8        c_result    = '?';
    UINT        u_cnt;

    ENTER(true);

    for (u_cnt = 0; u_cnt < 128; u_cnt++)
    {
        if (_cp1251_table[u_cnt] == u_char)
        {
            c_result = 128 + u_cnt;

            break;
        }
    }

    RETURN(c_result);
}
void strz_cp1251_to_utf8_s(PSTR               outBuff,
                           PVOID              p_in,
                           UINT               u_size)
{
    PU8         inBuff = (PU8)p_in;

    while (*inBuff)
    {
        if (*inBuff & 0x80)
        {
            int v = cp1251_table[(int)(0x7f & *inBuff++)];

            if (!v)
                continue;

            *outBuff++ = (char)v;
            *outBuff++ = (char)(v >> 8);

            if (v >>= 16)
                *outBuff++ = (char)v;
        }
        else
            *outBuff++ = *inBuff++;
    }

    *outBuff = 0;
}
// ***************************************************************************
// FUNCTION
//      u_size)
// PURPOSE
//
// PARAMETERS
// RESULT
//      PSTR strz_utf8_to_cp1PVOID psz_in,UINT --
// ***************************************************************************
PSTR strz_utf8_to_cp1251_s(PSTR       psz_out,
                           PVOID      p_in,
                           UINT       u_size)
{
    U32             u_char;
    PU8             psz_on_e;
    PU8             psz_on_s;
    PU8             psz_on_t;

    ENTER(p_in && u_size);

//    _recode();

        psz_on_s    = (PU8)p_in;
        psz_on_e    = psz_on_s + u_size;
        psz_on_t    = (PU8)psz_out;

        while (psz_on_s < psz_on_e)
        {
            u_char = _read_utf8(&psz_on_s, psz_on_e);

            if (u_char > 127)
            {
                *(psz_on_t++) = _find_cp1251(u_char);
            }
            else
                *(psz_on_t++) = (U8)u_char;
        }

        *(psz_on_t++) = 0;

    RETURN(psz_out);
}

// ***************************************************************************
// FUNCTION
//      u_size)
// PURPOSE
//
// PARAMETERS
// RESULT
//      PSTR strz_utf8_to_cp1PVOID psz_in,UINT --
// ***************************************************************************
PSTR strz_utf8_to_cp1251(PVOID      p_in,
                         UINT       u_size)
{
    PSTR            psz_out         = NULL;
//    U32             u_char;
//    PU8           psz_on_e;
//    PU8           psz_on_s;
//    PU8           psz_on_t;

    ENTER(p_in && u_size);

//    _recode();

    if ((psz_out = MALLOC(u_size + 1)) != NULL)
    {
        strz_utf8_to_cp1251_s(psz_out, p_in, u_size);
    }

    RETURN(psz_out);
}


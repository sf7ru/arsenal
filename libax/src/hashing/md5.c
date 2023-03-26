// ***************************************************************************
// TITLE
//      MD5 Message-digest Algorithm
// PROJECT
//      Arsenal Library
// REMARKS
//      Copyright (C) 1990, RSA Data Security, Inc. All rights reserved.
//
//      License to copy and use this software is granted provided that
//      it is identified as the "RSA Data Security, Inc. MD5 Message
//      Digest Algorithm" in all material mentioning or referencing this
//      software or this function.
//
//      License is also granted to make and use derivative works
//      provided that such works are identified as "derived from the RSA
//      Data Security, Inc. MD5 Message Digest Algorithm" in all
//      material mentioning or referencing the derived work.
//
//      RSA Data Security, Inc. makes no represela6tions concerning
//      either the merchala6bility of this software or the suitability
//      of this software for any particular purpose.  It is provided "as
//      is" without express or implied warranty of any kind.
//
//      These notices must be retained in any copies of any part of this
//      documela6tion and/or software.
// ***************************************************************************
//
// FILE
//      $Id: md5.c,v 1.2 2003/09/10 06:55:35 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// HISTORY
//      $Log: md5.c,v $
//      Revision 1.2  2003/09/10 06:55:35  A.Kozhukhar
//      *** empty log message ***
//
//      Revision 1.1.1.1  2003/02/14 13:17:30  A.Kozhukhar
//      Initial release
//
// ***************************************************************************


#include <stdlib.h>

#include <arsenal.h>
#include <axhashing.h>


// ---------------------------------------------------------------------------
// -------------------------- LOCAL DEFINITIONS ------------------------------
// ---------------------------------------------------------------------------

// F, G and H are basic MD5 functions: selection, majority, parity
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

// ROTATE_LEFT rotates x left n bits
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

// FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4
// Rotation is separate from addition to prevent recomputation
#define FF(a, b, c, d, x, s, ac) {(a) += F ((b), (c), (d)) + (x) + (U32)(ac); (a) = ROTATE_LEFT ((a), (s)); (a) += (b); }
#define GG(a, b, c, d, x, s, ac) {(a) += G ((b), (c), (d)) + (x) + (U32)(ac); (a) = ROTATE_LEFT ((a), (s)); (a) += (b); }
#define HH(a, b, c, d, x, s, ac) {(a) += H ((b), (c), (d)) + (x) + (U32)(ac); (a) = ROTATE_LEFT ((a), (s)); (a) += (b); }
#define II(a, b, c, d, x, s, ac) {(a) += I ((b), (c), (d)) + (x) + (U32)(ac); (a) = ROTATE_LEFT ((a), (s)); (a) += (b); }


// ---------------------------------------------------------------------------
// ----------------------------- STATIC DATA ---------------------------------
// ---------------------------------------------------------------------------

static unsigned char PADDING[64] = {
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------


// ***************************************************************************
// STATIC FUNCTION
//      _transform
// PURPOSE
//      Basic MD5 step. Transform buf based on in.
// PARAMETERS
//      U32  * buf -- Pointer to digets buffer
//      U32  * in  -- Pointer to in buffer
// RESULT
//      none
// ***************************************************************************
static void _transform(U32 *buf, U32 *in)
{
  U32 a = buf[0], b = buf[1], c = buf[2], d = buf[3];

  // Round 1
#define S11 7
#define S12 12
#define S13 17
#define S14 22
  FF ( a, b, c, d, in[ 0], S11, (U32) 3614090360u); // 1
  FF ( d, a, b, c, in[ 1], S12, (U32) 3905402710u); // 2
  FF ( c, d, a, b, in[ 2], S13, (U32)  606105819u); // 3
  FF ( b, c, d, a, in[ 3], S14, (U32) 3250441966u); // 4
  FF ( a, b, c, d, in[ 4], S11, (U32) 4118548399u); // 5
  FF ( d, a, b, c, in[ 5], S12, (U32) 1200080426u); // 6
  FF ( c, d, a, b, in[ 6], S13, (U32) 2821735955u); // 7
  FF ( b, c, d, a, in[ 7], S14, (U32) 4249261313u); // 8
  FF ( a, b, c, d, in[ 8], S11, (U32) 1770035416u); // 9
  FF ( d, a, b, c, in[ 9], S12, (U32) 2336552879u); // 10
  FF ( c, d, a, b, in[10], S13, (U32) 4294925233u); // 11
  FF ( b, c, d, a, in[11], S14, (U32) 2304563134u); // 12
  FF ( a, b, c, d, in[12], S11, (U32) 1804603682u); // 13
  FF ( d, a, b, c, in[13], S12, (U32) 4254626195u); // 14
  FF ( c, d, a, b, in[14], S13, (U32) 2792965006u); // 15
  FF ( b, c, d, a, in[15], S14, (U32) 1236535329u); // 16

  // Round 2
#define S21 5
#define S22 9
#define S23 14
#define S24 20
  GG ( a, b, c, d, in[ 1], S21, (U32) 4129170786u); // 17
  GG ( d, a, b, c, in[ 6], S22, (U32) 3225465664u); // 18
  GG ( c, d, a, b, in[11], S23, (U32)  643717713u); // 19
  GG ( b, c, d, a, in[ 0], S24, (U32) 3921069994u); // 20
  GG ( a, b, c, d, in[ 5], S21, (U32) 3593408605u); // 21
  GG ( d, a, b, c, in[10], S22, (U32)   38016083u); // 22
  GG ( c, d, a, b, in[15], S23, (U32) 3634488961u); // 23
  GG ( b, c, d, a, in[ 4], S24, (U32) 3889429448u); // 24
  GG ( a, b, c, d, in[ 9], S21, (U32)  568446438u); // 25
  GG ( d, a, b, c, in[14], S22, (U32) 3275163606u); // 26
  GG ( c, d, a, b, in[ 3], S23, (U32) 4107603335u); // 27
  GG ( b, c, d, a, in[ 8], S24, (U32) 1163531501u); // 28
  GG ( a, b, c, d, in[13], S21, (U32) 2850285829u); // 29
  GG ( d, a, b, c, in[ 2], S22, (U32) 4243563512u); // 30
  GG ( c, d, a, b, in[ 7], S23, (U32) 1735328473u); // 31
  GG ( b, c, d, a, in[12], S24, (U32) 2368359562u); // 32

  // Round 3
#define S31 4
#define S32 11
#define S33 16
#define S34 23
  HH ( a, b, c, d, in[ 5], S31, (U32) 4294588738u); // 33
  HH ( d, a, b, c, in[ 8], S32, (U32) 2272392833u); // 34
  HH ( c, d, a, b, in[11], S33, (U32) 1839030562u); // 35
  HH ( b, c, d, a, in[14], S34, (U32) 4259657740u); // 36
  HH ( a, b, c, d, in[ 1], S31, (U32) 2763975236u); // 37
  HH ( d, a, b, c, in[ 4], S32, (U32) 1272893353u); // 38
  HH ( c, d, a, b, in[ 7], S33, (U32) 4139469664u); // 39
  HH ( b, c, d, a, in[10], S34, (U32) 3200236656u); // 40
  HH ( a, b, c, d, in[13], S31, (U32)  681279174u); // 41
  HH ( d, a, b, c, in[ 0], S32, (U32) 3936430074u); // 42
  HH ( c, d, a, b, in[ 3], S33, (U32) 3572445317u); // 43
  HH ( b, c, d, a, in[ 6], S34, (U32)   76029189u); // 44
  HH ( a, b, c, d, in[ 9], S31, (U32) 3654602809u); // 45
  HH ( d, a, b, c, in[12], S32, (U32) 3873151461u); // 46
  HH ( c, d, a, b, in[15], S33, (U32)  530742520u); // 47
  HH ( b, c, d, a, in[ 2], S34, (U32) 3299628645u); // 48

  // Round 4
#define S41 6
#define S42 10
#define S43 15
#define S44 21
  II ( a, b, c, d, in[ 0], S41, (U32) 4096336452u); // 49
  II ( d, a, b, c, in[ 7], S42, (U32) 1126891415u); // 50
  II ( c, d, a, b, in[14], S43, (U32) 2878612391u); // 51
  II ( b, c, d, a, in[ 5], S44, (U32) 4237533241u); // 52
  II ( a, b, c, d, in[12], S41, (U32) 1700485571u); // 53
  II ( d, a, b, c, in[ 3], S42, (U32) 2399980690u); // 54
  II ( c, d, a, b, in[10], S43, (U32) 4293915773u); // 55
  II ( b, c, d, a, in[ 1], S44, (U32) 2240044497u); // 56
  II ( a, b, c, d, in[ 8], S41, (U32) 1873313359u); // 57
  II ( d, a, b, c, in[15], S42, (U32) 4264355552u); // 58
  II ( c, d, a, b, in[ 6], S43, (U32) 2734768916u); // 59
  II ( b, c, d, a, in[13], S44, (U32) 1309151649u); // 60
  II ( a, b, c, d, in[ 4], S41, (U32) 4149444226u); // 61
  II ( d, a, b, c, in[11], S42, (U32) 3174756917u); // 62
  II ( c, d, a, b, in[ 2], S43, (U32)  718787259u); // 63
  II ( b, c, d, a, in[ 9], S44, (U32) 3951481745u); // 64

  buf[0] += a;
  buf[1] += b;
  buf[2] += c;
  buf[3] += d;
}

// ***************************************************************************
// FUNCTION
//      md5_init
// PURPOSE
//      Initialize digest
// PARAMETERS
//      PMD5CTX pst_context          -- Pointer to MD5 Context structure
//      U32      d_pseudo_rand_number -- Pseudo-random number
// RESULT
//      none
// ***************************************************************************
void md5_init (PMD5CTX      pst_context,
               U32          d_pseudo_rand_number)
{
    pst_context->i[0] = pst_context->i[1] = (U32)0;

    // Load magic initialization constants.
    pst_context->buf[0] = (U32)0x67452301 + d_pseudo_rand_number * 11 ;
    pst_context->buf[1] = (U32)0xefcdab89 + d_pseudo_rand_number * 71 ;
    pst_context->buf[2] = (U32)0x98badcfe + d_pseudo_rand_number * 37 ;
    pst_context->buf[3] = (U32)0x10325476 + d_pseudo_rand_number * 97 ;
}

// ***************************************************************************
// FUNCTION
//      md5_update
// PURPOSE
//      Update MD5 digest
// PARAMETERS
//      PMD5CTX pst_context -- Pointer to MD5 Context structure
//      PU8      p_in_buff   -- Pointer to source buffer
//      U32      d_in_len    -- Size of source buffer
// RESULT
//      none
// ***************************************************************************
void md5_update (PMD5CTX    pst_context,
                 PU8        p_in_buff,
                 U32        d_in_len)
{
    U32         in  [ DEFMD5_HASH_SIZE ];
    int           mdi;
    unsigned int  i;
    unsigned int  ii;

    // compute number of bytes mod 64
    mdi = (int)((pst_context->i[0] >> 3) & 0x3F);

    // update number of bits
    if ((pst_context->i[0] + ((U32)d_in_len << 3)) < pst_context->i[0])
        pst_context->i[1]++;
    pst_context->i[0] += ((U32)d_in_len << 3);
    pst_context->i[1] += ((U32)d_in_len >> 29);

    while (d_in_len--)
    {
        // add new character to buffer, increment mdi
        pst_context->in[mdi++] = *p_in_buff++;

        // transform if necessary
        if (mdi == 0x40)
        {
            for (i = 0, ii = 0; i < DEFMD5_HASH_SIZE; i++, ii += 4)
                in[i] = (((U32)pst_context->in[ii+3]) << 24) |
                        (((U32)pst_context->in[ii+2]) << 16) |
                        (((U32)pst_context->in[ii+1]) << 8) |
                        ((U32)pst_context->in[ii]);
            _transform(pst_context->buf, in);
            mdi = 0;
        }
    }
}

// ***************************************************************************
// FUNCTION
//      md5_final
// PURPOSE
//      Finalize MD5 calculation
// PARAMETERS
//      PMD5CTX pst_context -- Pointer to MD5 Context structure
// RESULT
//      none
// ***************************************************************************
void md5_final(PMD5CTX pst_context)
{
    U32           in      [ DEFMD5_HASH_SIZE ];
    int             mdi;
    unsigned int    i, ii;
    unsigned int    padLen;

    // Save number of bits
    in[14] = pst_context->i[0];
    in[15] = pst_context->i[1];

    // Compute number of bytes mod 64
    mdi = (int)((pst_context->i[0] >> 3) & 0x3F);

    // Pad out to 56 mod 64
    padLen = (mdi < 56) ? (56 - mdi) : (120 - mdi);
    md5_update (pst_context, PADDING, padLen);

    // Append length in bits and transform
    for (i = 0, ii = 0; i < 14; i++, ii += 4)
        in[i] = (((U32)pst_context->in[ii+3]) << 24) |
                (((U32)pst_context->in[ii+2]) << 16) |
                (((U32)pst_context->in[ii+1]) << 8) |
                ((U32)pst_context->in[ii]);
    _transform (pst_context->buf, in);

    // Store buffer in digest
    for (i = 0, ii = 0; i < 4; i++, ii += 4)
    {
        pst_context->ab_digest[ii] = (unsigned char)(pst_context->buf[i] & 0xFF);
        pst_context->ab_digest[ii+1] =
        (unsigned char)((pst_context->buf[i] >> 8) & 0xFF);
        pst_context->ab_digest[ii+2] =
        (unsigned char)((pst_context->buf[i] >> 16) & 0xFF);
        pst_context->ab_digest[ii+3] =
        (unsigned char)((pst_context->buf[i] >> 24) & 0xFF);
    }
}

// ***************************************************************************
// FUNCTION
//      md5_calc
// PURPOSE
//      Calculate MD5 digest
// PARAMETERS
//      PU8      p_source             -- Pointer to source buffer
//      U32      d_size               -- Size of source buffer
//      U32      d_pseudo_rand_number -- Pseudo-random number
// RESULT
//      PMD5CTX -- Pointer to the just created MD5 Context structure
// NOTE
//      MD5 Context structure should be just FREEd after use
// ***************************************************************************
PMD5CTX md5_calc(PU8 p_source, U32 d_size, U32 d_pseudo_rand_number)
{
    PMD5CTX pst_context = NULL;

    if (p_source && d_size)
    {
        if ((pst_context = MALLOC_D(sizeof(MD5CTX), "MD5CTX")) != NULL)
        {
            md5_init(pst_context, d_pseudo_rand_number);
            md5_update(pst_context, p_source, d_size);
            md5_final(pst_context);
        }
    }

    return pst_context;
}

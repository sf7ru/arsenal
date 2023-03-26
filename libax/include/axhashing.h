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

#ifndef __AXHASHING_H__
#define __AXHASHING_H__

#include <arsenal.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define ADLER32_INITIAL         ((U32)1)
#define CRC16_INITIAL           ((U16)0)
#define CRC32_INITIAL           ((U32)-1)
#define CRC64_INITIAL           ((U64)0)
#define FCS16_INITIAL           ((U16)-1)

#define DEFMD5_HASH_SIZE    16

#define SHA256_HASH_SIZE    32

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

// ***************************************************************************
// STRUCTURE
//      MD5CTX
// PURPOSE
//
// ***************************************************************************
#pragma pack(1)
typedef AXPACKED(struct) __tag_MD5CTX
{
    U32             i           [ 2 ];      // number of _bits_ handled mod 2^
                                            // 64
    U32             buf         [ 4 ];      // scratch buffer
    U8              in          [ 64 ];     // input buffer
    U8              ab_digest [ DEFMD5_HASH_SIZE ];
                                            // actual digest after _final call
} MD5CTX, * PMD5CTX;
#pragma pack()

#pragma pack(1)
typedef AXPACKED(struct) __tag_SHA256CTX
{
    U8              data        [64];
    I64             bitlen;
    U32             datalen;
    U32             state       [8];
} SHA256CTX, * PSHA256CTX;
#pragma pack()



// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif                                      //  #ifdef __cplusplus

// ---------------------------- hash/crc8.c ----------------------------------

U8                  crc8_update                 (PVOID          p_data,
                                                 UINT           u_size,
                                                 U8             w_start_val);

U8                  crc8_calc                   (PVOID          p_data,
                                                 UINT           u_size);


// ---------------------------- hash/crc16.c ---------------------------------

U16                 crc16_update                (U16            w_start_val,
                                                 PVOID          p_data,
                                                 UINT           u_size);

U16                 crc16_calc                  (PVOID          p_data,
                                                 UINT           u_size);

// ----

U32                 crc32_update                (U32            d_start_value,
                                                 PVOID          p_data,
                                                 UINT           u_size);

U32                 crc32_calc                  (PVOID          p_data,
                                                 UINT           u_size);

U32                 crc32_calc_str              (PSTR           psz_string);

U32                 crc32_ccitt                 (PCU8           buf,
                                                 UINT           len,
                                                 U32            seed);

U32                 crc32_ccitt2                (PCU8           buf,
                                                 UINT           len,
                                                 U32            seed);

// ----

U32                 fletcher32_calc             (PVOID          p_source,
                                                 UINT           size);

U16                 fletcher16_calc             (PVOID          data,
                                                 INT            count);

// ----

U32                 adler32_update              (U32            d_adler,
                                                 PCVOID         p_buf,
                                                 UINT           d_len);

U32                 adler32_calc                (PCVOID         p_buf,
                                                 UINT           d_len);
// ----

U64                 crc64ecma_update            (U64            initial,
                                                 PVOID          data,
                                                 INT            size);

U64                 crc64ecma_calc              (PVOID          data,
                                                 INT            size);

// ----

void                md5_init                    (PMD5CTX        pst_context,
                                                 U32            d_pseudo_rand_number);

void                md5_update                  (PMD5CTX        pst_context,
                                                 PU8            p_in_buff,
                                                 U32            d_in_len);

PMD5CTX             md5_calc                    (PU8            p_source,
                                                 U32            d_size,
                                                 U32            d_pseudo_rand_number);

void                md5_final                   (PMD5CTX        pst_context);

// ----

U8                  simplesum_calc              (PVOID          p_data,
                                                 UINT           u_size);
// ----

U8                  xorsum_calc                 (PVOID          p_data,
                                                 UINT           u_size);

U8                  xorsum_str                  (PSTR           psz_string);

// ----

void                sha256_init                 (PSHA256CTX     ctx);

void                sha256_update               (PSHA256CTX     ctx,
                                                 PCVOID         data,
                                                 UINT           len);

void                sha256_final                (PSHA256CTX     ctx,
                                                 PVOID          buffForHash);


#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus

#endif //  #ifndef __AXHASHING_H__

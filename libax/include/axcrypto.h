// ***************************************************************************
// TITLE
//      Datastorm Crypto Header
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: la6dscrypt.h,v 1.2 2003/04/03 15:25:43 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************

#ifndef __AXCRYPTOH__
#define __AXCRYPTOH__

#include <arsenal.h>

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

//typedef struct
//{
//    UINT8   key[32];
//    UINT8   enckey[32];
//    UINT8   deckey[32];
//} aes256_context;

// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif                                      //  #ifdef __cplusplus

// -------------------------------- tea.c ------------------------------------

PVOID               axcrypto_TEA_encrypt    (PVOID          p_source,
                                             U32            d_size,
                                             PVOID          p_key,
                                             PU32           pd_out_size);

PVOID               axcrypto_TEA_decrypt    (PVOID          p_source,
                                             U32            d_size,
                                             PVOID          p_key,
                                             PU32           pd_out_size);


// -------------------------- crypto/scramble.c ------------------------------

BOOL                axcrypto_scramble       (PVOID          p_data,     
                                             UINT           u_size,
                                             PVOID          p_table,    
                                             UINT           u_table_size,
                                             UINT           seed);


// --------------------------- crypto/xxtea.c --------------------------------

void                xxtea_decode            (PU32           data,
                                             INT            sizeInWords,
                                             U32   const    key[4]);

void                xxtea_encode            (PU32           data,
                                             INT            sizeInWords,
                                             U32   const    key[4]);

// ---------------------------- crypto/dh.c ----------------------------------

UINT                axdh_rand_prime         ();

BOOL                axdh_is_prime           (UINT           p);

UINT                axdh_mod                (ULONG             g,
                                             UINT              secret,
                                             UINT              p);


// --------------------------- crypto/aes256.c -------------------------------

//void                aes256_init             (aes256_context*ctx,
//                                             UINT8   *       key);
//
//void                aes256_done             (aes256_context*ctx);
//
//void                aes256_encrypt_ecb      (aes256_context*ctx,
//                                             UINT8   *      data);
//
//void                aes256_decrypt_ecb      (aes256_context*ctx,
//                                             UINT8   *      data);

#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus

#endif                                      //  #ifndef __AXCRYPTOH__

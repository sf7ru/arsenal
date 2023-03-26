// ***************************************************************************
// TITLE
//      The Tiny Encryption Algorithm
// PROJECT
//      Arsenal Library
// ***************************************************************************
//
// FILE
//      $Id: tea.c,v 1.2 2003/04/24 10:07:28 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// HISTORY
//      $Log: tea.c,v $
//      Revision 1.2  2003/04/24 10:07:28  A.Kozhukhar
//      *** empty log message ***
//
//      Revision 1.1.1.1  2003/02/14 13:17:29  A.Kozhukhar
//      Initial release
//
// ***************************************************************************


#include <stdlib.h>
#include <string.h>

#include <arsenal.h>

#include <axcrypto.h>


// ---------------------------------------------------------------------------
// -------------------------- LOCAL DEFINITIONS ------------------------------
// ---------------------------------------------------------------------------

#define DEF_SIZE_MAGIC          0xDF
#define DEF_BLOCK_SIZE          8
#define DEF_DELTA               0x9E3779B9
#define DEF_SUM_INIT            0xC6EF3720

#define MAC_LENDELTA(a)         ((DEF_BLOCK_SIZE - (a % DEF_BLOCK_SIZE)) & 7)



// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------


// ***************************************************************************
// STATIC FUNCTION
//      _encipher
// PURPOSE
//      TEA encipher block routine
// PARAMETERS
//      PU32   p_target_block -- Pointer to target block of U32s
//      PU32   p_source_block -- Pointer to source block of U32s
//      PU32   p_key          -- Pointer to Key
// RESULT
//      none
// ***************************************************************************
static void _encipher(PU32   p_target_block,
                      PU32   p_source_block,
                      PU32   p_key)
{
    U32     d_y         = p_source_block    [ 0 ];
    U32     d_z         = p_source_block    [ 1 ];
    U32     d_sum       = 0;
    U32     d_delta     = DEF_DELTA;
    U32     d_n         = 32;

    while (d_n-- > 0)
    {
        d_y   += (((d_z << 4) ^ (d_z >> 5)) + d_z) ^ (d_sum + p_key[d_sum & 3]);
        d_sum += d_delta;
        d_z   += (((d_y << 4) ^ (d_y >> 5)) + d_y) ^ (d_sum + p_key[d_sum >> 11 & 3]);
    }

   p_target_block[0] = d_y; p_target_block[1] = d_z;
}

// ***************************************************************************
// STATIC FUNCTION
//      _decipher
// PURPOSE
//      TEA decipher block routine
// PARAMETERS
//      PU32   p_target_block -- Pointer to target block of U32s
//      PU32   p_source_block -- Pointer to source block of U32s
//      PU32   p_key          -- Pointer to Key
// RESULT
//      none
// ***************************************************************************
static void _decipher(PU32   p_target_block,
                      PU32   p_source_block,
                      PU32   p_key)
{
    U32     d_y         = p_source_block    [ 0 ];
    U32     d_z         = p_source_block    [ 1 ];
    U32     d_sum       = DEF_SUM_INIT;
    U32     d_delta     = DEF_DELTA;
    U32     d_n         = 32;

    while(d_n-- > 0)
    {
        d_z   -= (((d_y << 4) ^ (d_y >> 5)) + d_y) ^ (d_sum + p_key[d_sum >> 11 & 3]);
        d_sum -= d_delta;
        d_y   -= (((d_z << 4) ^ (d_z >> 5)) + d_z) ^ (d_sum + p_key[d_sum & 3]);
    }

   p_target_block[0] = d_y; p_target_block[1] = d_z;
}

// ***************************************************************************
// FUNCTION
//      axcrypto_TEA_encrypt
// PURPOSE
//      Encrypt data with TEA algorithm
// PARAMETERS
//      PVOID  p_source    -- Pointer to source data
//      U32    d_size      -- Size of source data
//      PVOID  p_key       -- Pointer to Key
//      PU32   pd_out_size -- Pointer to U32 to which will be stored
//                            size of encrypted data
// RESULT
//      PVOID  -- Pointer to the allocated buffer with encrypted data if
//                all is ok or NULL if not
// ***************************************************************************
PVOID axcrypto_TEA_encrypt(PVOID  p_source,
                           U32    d_size,
                           PVOID  p_key,
                           PU32   pd_out_size)
{
    PVOID   p_return        = NULL;
    U32     d_buff_size     = d_size + MAC_LENDELTA(d_size) + sizeof(U8);
    PU8  	p_buffer;
    CHAR    b_corr_buff     [ DEF_BLOCK_SIZE ];


    if ( p_source && d_size && p_key &&
         ((p_return = MALLOC_D(d_buff_size, "ENCRYPTED")) != NULL) )
    {
        p_buffer        = p_return;
        *(p_buffer++)	= (U8)MAC_LENDELTA(d_size) ^ DEF_SIZE_MAGIC;
		
        if (pd_out_size)
            *pd_out_size = d_buff_size;

        while (d_size)
        {
            if (d_size < DEF_BLOCK_SIZE)
            {
                memcpy(b_corr_buff, p_source, d_size);
                _encipher((PU32  )p_buffer, (PU32  )b_corr_buff, p_key);
                break;
            }
            else
                _encipher((PU32  )p_buffer, p_source, p_key);

            p_buffer += ((DEF_BLOCK_SIZE / sizeof(U32  )) << 2);
            p_source  = ((PU8)p_source) + DEF_BLOCK_SIZE;
            d_size   -= DEF_BLOCK_SIZE;
        }
    }

    return p_return;
}

// ***************************************************************************
// FUNCTION
//      axcrypto_TEA_decrypt
// PURPOSE
//      Decrypt data with TEA algorithm
// PARAMETERS
//      PVOID  p_source    -- Pointer to source data
//      U32    d_size      -- Size of source data
//      PVOID  p_key       -- Pointer to Key
//      PU32   pd_out_size -- Pointer to U32 to which will be stored
//                            size of decrypted data
// RESULT
//      PVOID  -- Pointer to the allocated buffer with decrypted data if
//                all is ok or NULL if not
// ***************************************************************************
PVOID axcrypto_TEA_decrypt(PVOID p_source,
                           U32   d_size,
                           PVOID p_key,
                           PU32   pd_out_size)
{
    PVOID   p_return        = NULL;
    PU32    p_buffer;
    U32     d_buff_size;
    U32     d_sub;

    if ( p_source && d_size && p_key &&
         ((d_sub       = (*(PU8)p_source) ^ DEF_SIZE_MAGIC) < DEF_BLOCK_SIZE)             &&
         ((d_buff_size = (d_size - d_sub - sizeof(U8))) != 0)                             &&
         ((p_return    = MALLOC_D(d_buff_size + DEF_BLOCK_SIZE, "DECRYPTED")) != NULL) )
    {
        p_buffer  = p_return;
        // Bypassing real size value
        p_source  = ((PU8)p_source) + sizeof(U8);
        d_size   -= sizeof(U8);

        if (pd_out_size)
            *pd_out_size = d_buff_size;

        while (d_size)
        {
            _decipher(p_buffer, p_source, p_key);
            p_buffer += (DEF_BLOCK_SIZE / sizeof(U32  ));
            p_source  = ((PU8)p_source) + DEF_BLOCK_SIZE;
            d_size   -= ((d_size < DEF_BLOCK_SIZE) ? d_size : DEF_BLOCK_SIZE);
        }
    }

    return p_return;
}

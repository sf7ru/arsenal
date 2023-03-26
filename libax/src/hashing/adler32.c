// ***************************************************************************
// TITLE
//      32-bit Adler Checksums Calculation
// PROJECT
//      Arsenal Library/Library/Hashing
// ***************************************************************************
//
// FILE
//      $Id:$
// HISTORY
//      $Log:$
// ***************************************************************************

#include<arsenal.h>

#include<axhashing.h>

// ---------------------------------------------------------------------------
// -------------------------- LOCAL DEFINITIONS ------------------------------
// ---------------------------------------------------------------------------

#define BASE        65521  //  largest prime smaller than 65536

// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      adler32_update
// PURPOSE
//      Update the Adler Checksum
// PARAMETERS
//      U32   d_adler -- Start checksum value
//      PVOID p_buf   -- Pointer to data buffer
//      U32   d_len   -- Size of data
// RESULT
//      U32   -- Updated Adler Checksum
// NOTE
//      The Adler32 checksum should be initialized to 1
// ***************************************************************************
U32 adler32_update(U32      d_adler,
                   PCVOID   p_buf,
                   UINT     d_len)
{
    U32      s1 = d_adler & 0xffff;
    U32      s2 = (d_adler >> 16) & 0xffff;
    unsigned    n;


    for (n = 0; n < d_len; n++)
    {
        s1 = (s1 + ((PCU8)p_buf)[n]) % BASE;
        s2 = (s2 + s1)               % BASE;
    }

    return (s2 << 16) + s1;
}
// ***************************************************************************
// FUNCTION
//      adler32_calc
// PURPOSE
//      Calculate the Adler Checksum of data block
// PARAMETERS
//      PVOID p_buf -- Pointer to data buffer
//      U32   d_len -- Size of data
// RESULT
//      U32   -- Adler Checksum of data block
// ***************************************************************************
U32 adler32_calc(PCVOID      p_buf,
                 UINT        d_len)
{
    return adler32_update(ADLER32_INITIAL, p_buf, d_len);
}

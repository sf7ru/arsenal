// ***************************************************************************
// TITLE
//      32-bit Fletcher Checksums Calculation
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


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      fletcher32_calc
// PURPOSE
//      Calculate the Fletcher Checksum of data block
// PARAMETERS
//      PVOID p_source  -- Pointer to data buffer
//      U32   d_size    -- Size of data
// RESULT
//      U32    -- Fletcher32 checksum of data block
// ***************************************************************************
U32 fletcher32_calc(PVOID p_source, UINT size)
{
    U32         d_s1          = 0;
    U32         d_s2          = 0;
    U16         w_x;
    PU8         pb_source;
    PU16        pw_source;
    unsigned    n_cnt;
    UINT        half_size;


    half_size = size >> 1;
    pb_source   = (PU8  )p_source;
    pw_source   = (PU16  )p_source;

    for (n_cnt = 0; n_cnt < half_size; n_cnt++, pw_source++)
    {
        d_s1 = ((d_s1 + *pw_source) & 0x0000ffff);
        d_s2 = ((d_s2 + d_s1)       & 0x0000ffff);
    }

    if(size > (half_size << 1))
    {
        // do the odd byte, we don't expect this code to execute.
        w_x = pb_source[(size-1)] << 8 | 0;
        d_s1 = ((d_s1 + w_x)  & 0x0000ffff);
        d_s2 = ((d_s2 + d_s1) & 0x0000ffff);
    }

    return ((U32  )(d_s2 << 16) + d_s1);
}

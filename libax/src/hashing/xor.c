// ***************************************************************************
// TITLE
//      Simple XOR checksum
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id$
// AUTHOR
//      $Author$
// ***************************************************************************

#include<arsenal.h>



// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      xorsum_calc
// PURPOSE
//
// PARAMETERS
//      PVOID p_data --
//      UINT  u_size --
// RESULT
//      U8    --
// ***************************************************************************
U8 xorsum_calc(PVOID p_data, UINT u_size)
{
    U8        b_checksum      = 0;
    UINT        u_left;
    PU8       p_on;

    ENTER(p_data && u_size);

    p_on    = p_data;
    u_left  = u_size;

    while (u_left--)
    {
        b_checksum ^= *(p_on++);
    }

    RETURN(b_checksum);
}
// ***************************************************************************
// FUNCTION
//      xorsum_str
// PURPOSE
//
// PARAMETERS
//      PSTR psz_string --
// RESULT
//      U8    --
// ***************************************************************************
U8 xorsum_str(PSTR psz_string)
{
    U8        b_checksum      = 0;
    PSTR        psz_on;

    ENTER(psz_string);

    psz_on  = psz_string;

    while (*psz_on)
    {
        b_checksum ^= *(psz_on++);
    }

    RETURN(b_checksum);
}

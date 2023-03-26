// ***************************************************************************
// TITLE
//      Simple Data Scrambling
// PROJECT
//      Arsenal Library
// ***************************************************************************
//
// FILE
//      $Id: scramble.c,v 1.1.1.1 2003/02/14 13:17:29 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// HISTORY
//      $Log: scramble.c,v $
//      Revision 1.1.1.1  2003/02/14 13:17:29  A.Kozhukhar
//      Initial release
//
// ***************************************************************************


#include <stdlib.h>
#include <string.h>

#include <arsenal.h>

#include <axcrypto.h>



// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------


// ***************************************************************************
// FUNCTION
//      axcrypto_scramble
// PURPOSE
//      Scramble data with Key table
// PARAMETERS
//      PVOID p_data       -- Pointer to Source/Target data
//      UINT  u_size       -- Size of data to scramble
//      PVOID p_table      -- Pointer to Key table
//      UINT  u_table_size -- Size of Key table
//      UINT  seed         -- Random Seed
// RESULT
//      none
// ***************************************************************************
BOOL axcrypto_scramble(PVOID     p_data,     
                       UINT      u_size,
                       PVOID     p_table,    
                       UINT      u_table_size,
                       UINT      seed)
{
    BOOL            b_result        = false;
    PU8             p_on_data       = p_data;
    PU8             p_on_table      = p_table;

    ENTER(p_data && u_size && p_table && u_table_size);

    while (u_size--)
    {
        *(p_on_data++) ^= *(p_on_table + ((u_size + seed) % u_table_size));
    }

    b_result = true;

    RETURN(b_result);
}

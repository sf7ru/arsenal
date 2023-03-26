// ***************************************************************************
// TITLE
//      Text Processing Routinues
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: text.c,v 1.14 2003/12/10 12:57:38 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arsenal.h>
#include <axmath.h>




// ***************************************************************************
// FUNCTION
//      la6_parts
// PURPOSE
//
// PARAMETERS
//      UINT u_dividend --
//      UINT u_divisor  --
// RESULT
//      UINT    --
// ***************************************************************************
UINT ax_parts(UINT u_dividend, UINT u_divisor)
{
    div_t       st_div      = { 0, 0 };

    ENTER(u_divisor);

    st_div  = div(u_dividend, u_divisor);

    if (st_div.rem)
        st_div.quot++;

    RETURN(st_div.quot);
}
// ***************************************************************************
// FUNCTION
//      la6_parts
// PURPOSE
//
// PARAMETERS
//      UINT u_dividend --
//      UINT u_divisor  --
// RESULT
//      UINT    --
// ***************************************************************************
UINT ax_round_up(UINT u_dividend, UINT u_divisor)
{
    div_t       st_div      = { 0, 0 };

    ENTER(u_divisor);

    st_div  = div(u_dividend, u_divisor);

    if (st_div.rem)
        st_div.quot++;

    RETURN(st_div.quot * u_divisor);
}
// ***************************************************************************
// FUNCTION
//      ax_parts_l
// PURPOSE
//
// PARAMETERS
//      UINT   u_dividend --
//      UINT   u_divisor       --
// RESULT
//      ULONG  --
// ***************************************************************************
ULONG ax_parts_l(ULONG u_dividend, UINT u_divisor)
{
    ULONG      u_result        = 0;

    ENTER(u_divisor);

    u_result = u_dividend / u_divisor;

    if (u_dividend % u_divisor)
    {
        u_result++;
    }

    RETURN(u_result);
}
// ***************************************************************************
// FUNCTION
//      ax_twoscomplement_to_int
// PURPOSE
//
// PARAMETERS
//      int    smallInt        -- number in two's complement form
//      int    bits            -- number of meaning bits
// RESULT
//      int  -- result in signed magniture form (traditional)
// ***************************************************************************
INT ax_twoscomplement_to_int(INT smallInt, INT bits)

{
  const int negative = (smallInt & (1 << (bits - 1))) != 0;
  int nativeInt;

  if (negative)
    nativeInt = smallInt | ~((1 << (bits)) - 1);
  else
    nativeInt = smallInt;


  return nativeInt;
}

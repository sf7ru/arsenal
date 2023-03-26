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
#include <string.h>

#include <axtime.h>

#include <math.h>

#include <arsenal.h>

#include <axcrypto.h>

// Diffie-Hellman


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------


// ***************************************************************************
// FUNCTION
//      axdh_is_prime
// PURPOSE
//
// PARAMETERS
//      UINT p --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL axdh_is_prime(UINT     p)
{
    BOOL            result          = true;
    UINT            i               = 0;
    UINT            r               = (UINT)sqrt(p);

    ENTER(true);

    for (i = 2; result && (i <= r); i++)
    {
        if (p%i == 0)
        {
            result = false;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      axdh_mod
// PURPOSE
//
// PARAMETERS
//      ULONG g --
//      UINT  a --
//      UINT  p --
// RESULT
//      UINT --
// ***************************************************************************
UINT axdh_mod(ULONG             g,
              UINT              secret,
              UINT              p)
{
    UINT            result          = 0;
    ULONG           z;
    ULONG           tmp;
    UINT            idx;

    ENTER(true);

    if (secret != 0)
    {
        z = g % p;

        if (secret != 1)
        {
            tmp = z;

            for (idx = 2; idx <= secret; idx++)
            {
                tmp = (tmp * z) % p;
            }

            result = (UINT)tmp;
        }
        else
            result = (UINT)z;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      axdh_rand_prime
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      UINT --
// ***************************************************************************
UINT axdh_rand_prime()
{
    UINT            result          = 0;

    ENTER(true);

    srand(axtime_get_monotonic());

    result = rand();

    while (!axdh_is_prime(result))
    {
        result++;
    }

    RETURN(result);
}

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

#include <arsenal.h>
#include <stdint.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define DELTA 0x9e3779b9
#define MX (((z>>5^y<<2) + (y>>3^z<<4)) ^ ((sum^y) + (key[(p&3)^e] ^ z)))


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      xxtea_encode
// PURPOSE
//
// PARAMETERS
//      PU32        data        --
//      INT         sizeInWords --
//      U32   const key[4]      --
// RESULT
//      void --
// ***************************************************************************
void xxtea_encode(PU32  data,
                  INT   sizeInWords,
                  U32   const key[4])
{
    U32 y, z, sum;
    unsigned p, rounds, e;

    rounds  = 6 + 52/sizeInWords;
    sum     = 0;
    z       = data[sizeInWords-1];

    do
    {
        sum += DELTA;
        e    = (sum >> 2) & 3;

        for (p = 0; p < (sizeInWords-1); p++)
        {
            y = data[p+1];
            z = data[p] += MX;
        }

        y = data[0];
        z = data[sizeInWords-1] += MX;

    } while (--rounds);
}
// ***************************************************************************
// FUNCTION
//      xxtea_decode
// PURPOSE
//
// PARAMETERS
//      PU32        data        --
//      INT         sizeInWords --
//      U32   const key[4]      --
// RESULT
//      void --
// ***************************************************************************
void xxtea_decode(PU32  data,
                  INT   sizeInWords,
                  U32   const key[4])
{
    U32 y, z, sum;
    unsigned p, rounds, e;

    rounds  = 6 + 52/sizeInWords;
    sum     = rounds * DELTA;
    y       = data[0];

    do
    {
        e = (sum >> 2) & 3;

        for (p=sizeInWords-1; p>0; p--)
        {
            z = data[p-1];
            y = data[p] -= MX;
        }

        z = data[sizeInWords-1];
        y = data[0] -= MX;
        sum -= DELTA;

    } while (--rounds);
}

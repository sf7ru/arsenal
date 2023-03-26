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

#include <string.h>
#include <stdio.h>

#include <arsenal.h>
#include <axcrypto.h>
#include <axstring.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define DATA_SIZE           10

U32 key [ 4 ] = { 0x12345678L, 0xABCDEFABL, 0x87654321L, 0x01010110L };

U32 originData [ DATA_SIZE ];
U32 testData [ DATA_SIZE ];

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------


// ***************************************************************************
// FUNCTION
//      main
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      int --
// ***************************************************************************
int main(void)
{
    int     result      = 1;
    int     i;

    for (i = 0; i < DATA_SIZE; i++)
    {
        originData  [ i ]   = i;
        testData    [ i ]   = i;
    }

    strz_dump("origin:  ", originData, sizeof(U32) * DATA_SIZE);

    xxtea_encode(testData, DATA_SIZE, key);

    strz_dump("encoded: ", testData, sizeof(U32) * DATA_SIZE);

    xxtea_decode(testData, DATA_SIZE, key);

    strz_dump("decoded: ", testData, sizeof(U32) * DATA_SIZE);

    if (!memcmp(originData, testData, sizeof(U32) * DATA_SIZE))
    {
        printf("OK\n");
        result = 0;
    }
    else
        printf("FAILED!");

    return result;
}


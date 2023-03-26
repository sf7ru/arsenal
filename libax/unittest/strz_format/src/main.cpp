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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <axtime.h>
#include <axstring.h>
#include <stdarg.h>
#include <string.h>


// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

static  INT    lenreal;



// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------


static INT myputchar(PVOID prt,
                     CHAR  c)
{
    putchar(c);
    lenreal++;

    return 1;
}

// ***************************************************************************
// FUNCTION
//      main
// PURPOSE
//
// PARAMETERS
//      int      argc   --
//      char *   argv[] --
// RESULT
//      int  --
// ***************************************************************************
int main(int        argc,
         char *     argv[])
{
    int             result      = 0;

    int             value       = 0x00efcdab;

    INT             len;
    CHAR            buff        [ 10 ];

    ENTER(TRUE);

// -----

    lenreal = 0;
    len = strz_format((PFNPUTCHAR)myputchar, NULL,
                      "*: %% %s %d %s %u %s %u %s %l\n",
                      "| -411 with d =", -411,
                      "| -100 with u =", -100,
                      "| 0 with u =", 0,
                      "| 300L with l =", 300L);

    printf("R: "
           "%% %s %d %s %u %s %u %s %lu\n",
           "| -411 with d =", -411,
           "| -100 with u =", -100,
           "| 0 with u =", 0,
           "| 300L with l =", 300L);

    if (lenreal != len) printf("@@@@@@@@@@@@ LEN MISMATCH!!! lenreal = %d, len = %d\n", lenreal, len);

// -----

    lenreal = 0;
    len = strz_format((PFNPUTCHAR)myputchar, NULL,
                      "*: %s %x %s %X %s %p\n",
                      "| -100 with x =", -100,
                      "| -100 with X =", -100,
                      "| &result with p =", &result);

    printf("R: "
           "%s %x %s %X %s %p\n",
           "| -100 with x =", -100,
           "| -100 with X =", -100,
           "| &result with p =", &result);

    if (lenreal != len) printf("@@@@@@@@@@@@ LEN MISMATCH!!! lenreal = %d, len = %d\n", lenreal, len);

// -----

    lenreal = 0;
    len = strz_format((PFNPUTCHAR)myputchar, NULL,
                      "*: %s %h %s %H\n",
                      "| dump 3 with h =", &value, 3,
                      "| dump 4 with H =", &value, 4);

    printf("R: \n");

    if (lenreal != len) printf("@@@@@@@@@@@@ LEN MISMATCH!!! lenreal = %d, len = %d\n", lenreal, len);

// -----

    lenreal = 0;
    len = strz_format((PFNPUTCHAR)myputchar, NULL,
                      "*: %s %c %s %8b\n",
                      "| 'c' with c =", 'c',
                      "| AA with 8b =", 0xFF0F00AA,
                      "| &result with p =", &result);

    printf("R: \n");

    if (lenreal != len) printf("@@@@@@@@@@@@ LEN MISMATCH!!! lenreal = %d, len = %d\n", lenreal, len);

// -----

    lenreal = 0;
    len = strz_format((PFNPUTCHAR)myputchar, NULL,
                      "*: ---- %.4d-%1.2d-%2.2d_%3.2d:%4.2d:%5.2d ---- '%7.4d' '%6.4d' '%5.4d' '%4.4d' '%3.4d'\n",
                      2010, 12, 28, 9, 8, 7, -100, -100, -100, -100, -100);

    printf("R: ---- %.4d-%1.2d-%2.2d_%3.2d:%4.2d:%5.2d ---- '%7.4d' '%6.4d' '%5.4d' '%4.4d' '%3.4d'\n",
                      2010, 12, 28, 9, 8, 7, -100, -100, -100, -100, -100);

    if (lenreal != len) printf("@@@@@@@@@@@@ LEN MISMATCH!!! lenreal = %d, len = %d\n", lenreal, len);

// -----

    lenreal = 0;
    len = strz_format((PFNPUTCHAR)myputchar, NULL,
                      "*: %8.4d\n",
                      123);

    printf("R: %8.4d\n", 123);

    if (lenreal != len) printf("@@@@@@@@@@@@ LEN MISMATCH!!! lenreal = %d, len = %d\n", lenreal, len);

    len = strz_sformat(buff, 10, "der {%d}", 1000);

    printf("R: sformat = '%s' len %d\n", buff, len);


    lenreal = 0;
    len = strz_format((PFNPUTCHAR)myputchar, NULL,
                      "*: ---- '%1.2f', '%2.2f' '%3.2f' '%4.2f' '%4.5f' '%4.8f' '%12.8f' '%2.8f' '%20.16f'\n",
           55.12345678, 55.12345678, 55.12345678, 55.12345678, 55.12345678, 55.12345678, 55.12345678, -55.12345678, -55.12345678);

    printf("R: ---- '%1.2f', '%2.2f' '%3.2f' '%4.2f' '%4.5f' '%4.8f' '%12.8f' '%2.8f' '%20.16f'\n",
           55.12345678, 55.12345678, 55.12345678, 55.12345678, 55.12345678, 55.12345678, 55.12345678, -55.12345678, -55.12345678);


    lenreal = 0;
    len = strz_format((PFNPUTCHAR)myputchar, NULL,
                      "*: ---- '%1.2f', '%2.2f' '%3.2f' '%4.2f' '%4.5f' '%4.8f' '%12.8f' '%2.8f' '%16.8f'\n",
           55.87654321, 55.87654321, 55.87654321, 55.87654321, 55.87654321, 55.87654321, 55.87654321, -55.87654321, -55.87654321);

    printf("R: ---- '%1.2f', '%2.2f' '%3.2f' '%4.2f' '%4.5f' '%4.8f' '%12.8f' '%2.8f' '%16.8f'\n",
           55.87654321, 55.87654321, 55.87654321, 55.87654321, 55.87654321, 55.87654321, 55.87654321, -55.87654321, -55.87654321);

    lenreal = 0;
    len = strz_format((PFNPUTCHAR)myputchar, NULL,
                      "*: ---- '%1.2f', '%2.2f' '%3.2f' '%4.2f' '%4.5f' '%4.8f' '%12.8f' '%2.8f' '%16.8f'\n",
           55.99999989, 55.99999989, 55.99999989, 55.99999989, 55.99999989, 55.99999989, 55.99999989, -55.99999989, -55.99999989);

    printf("R: ---- '%1.2f', '%2.2f' '%3.2f' '%4.2f' '%4.5f' '%4.8f' '%12.8f' '%2.8f' '%16.8f'\n",
           55.99999989, 55.99999989, 55.99999989, 55.99999989, 55.99999989, 55.99999989, 55.99999989, -55.99999989, -55.99999989);




    lenreal = 0;
    len = strz_format((PFNPUTCHAR)myputchar, NULL,
                      "*: ---- '\\\\.\\\\%s\\'\n", "pipe");

    printf("R: ---- '\\\\.\\\\%s\\'\n", "pipe");


    lenreal = 0;
    len = strz_format((PFNPUTCHAR)myputchar, NULL,
                      "*: ---- '%6.2f%% %3.1f Kbit/sec (%3.1f Kbyte/sec)'\n",
                                  (double)2000 / 300,
                                  (double)1024 / 128,
                                  (double)1020 / 1024);

    printf("R: ---- '%6.2f%% %3.1f Kbit/sec (%3.1f Kbyte/sec)'\n",
                                  (double)2000 / 300,
                                  (double)1024 / 128,
                                  (double)1020 / 1024);

    lenreal = 0;
    len = strz_format((PFNPUTCHAR)myputchar, NULL,
                      "*: ---- '%.2x' '%2.2x' '%4.2X' '%2.4x' '%4.2X'\n",
                                  0xab, 0xab, 0xab, 0xab, 0xabc);

    printf("R: ---- '%.2x' '%2.2x' '%4.2X' '%2.4x' '%4.2X'\n",
                                  0xab, 0xab, 0xab, 0xab, 0xabc);


    lenreal = 0;
    len = strz_format((PFNPUTCHAR)myputchar, NULL,
                      "*: ---- '%llu' '%lu' '%u'\n",
                                  100LL, 100L, 100);

    printf("R: ---- '%llu' '%lu' '%u'\n",
                                  100LL, 100L, 100);

    len = strz_format((PFNPUTCHAR)myputchar, NULL,
                      "HEX = 0x%x\n", 0x00);

    len = strz_format((PFNPUTCHAR)myputchar, NULL,
                      "HEX = 0x%x\n", 0x01);

    len = strz_format((PFNPUTCHAR)myputchar, NULL,
                      "HEX = 0x%x\n", 0x168);

    len = strz_format((PFNPUTCHAR)myputchar, NULL,
                      "HEX = 0x%x\n", 0x10203068);

    RETURN(result);
}

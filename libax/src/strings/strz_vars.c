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
#include <string.h>
#include <axstring.h>
#include <limits.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

double strz_atof(PCSTR s)
{
    double  rez = 0;
    double  fact = 1;
    int     point_seen;
    int     d;

    if (*s == '-')
    {
        s++;
        fact = -1;
    };

    for (point_seen = 0; *s; s++)
    {
        if (*s == '.')
        {
          point_seen = 1;
          continue;
        };

        d = *s - '0';

        if (d >= 0 && d <= 9)
        {
            if (point_seen)
                fact /= 10.0f;

            rez = rez * 10.0f + (double)d;
        }
    }

    return rez * fact;
};

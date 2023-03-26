
#include <stdio.h>
#include <stdarg.h>

// ----------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      tracef
// PURPOSE
//
// PARAMETERS
//      const char * format --
//                   ...    --
// RESULT
//      int --
// ***************************************************************************
int axdbg_trace(const char * place,
                const char * format,
                ...)
{
    int ret;
    va_list ap;

    va_start (ap, format);

    ret = vprintf (format, ap);

    va_end (ap);

    printf("\n");

    return ret;
}


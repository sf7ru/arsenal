
#include <ptv_debug_lpc.h>
#include <stdio.h>
#include <stdarg.h>

// ----------------------------------------------------------------------------

//#define ISDEBUGACTIVE()         (CoreDebug->DHCSR&CoreDebug_DHCSR_C_DEBUGEN_Msk)

#ifndef AX_INTEGER_TRACE_PRINTF_TMP_ARRAY_SIZE
#define AX_INTEGER_TRACE_PRINTF_TMP_ARRAY_SIZE (128)
#endif


// Defining this symbol enables semihosting in the Code Red debugger
//extern void __CR_SEMIHOST () __attribute__ ((weak, alias
//("axdbg_trace")));

extern int _write(int zero, const char *s, int len);
extern int __read(int zero, char *s, int len);
extern int __readc(void);

static int trace_write(char * buf, int size)
{
    int written;

    // The following if() disables semihosted writes when there is no hosted debugger
    // Otherwise, the target will halt when the semihost __write is called
//    if(ISDEBUGACTIVE())
        written = _write(0, buf, size);
//    else
//        written = 0;

    return written;
}

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

    // TODO: rewrite it to no longer use newlib, it is way too heavy

    static char buf[AX_INTEGER_TRACE_PRINTF_TMP_ARRAY_SIZE];

    // Print to the local buffer
    ret = vsnprintf (buf, sizeof(buf), format, ap);
    if (ret > 0)
      {
        // Transfer the buffer to the device
        ret = trace_write (buf, (size_t)ret);
      }

    va_end (ap);

    trace_write("\n", 1);

    return ret;
}


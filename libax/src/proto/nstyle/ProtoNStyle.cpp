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

#include <ProtoNStyle.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mathlib.h>
#include <axnavi.h>
#include <app.h>
#include <axdata.h>

#include <axstring.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define END_SIZE            6  // "*00\r\n"

#ifdef  NOSTDINOUT
    #define SFORMAT             strz_sformat
    #define VSFORMAT            strz_vsformat
#else
    #if (TARGET_ARCH == __AX_stm32__)
        #define SFORMAT(a,b,...)    sprintf(a, __VA_ARGS__)
        #define VSFORMAT(a,b,...)   vsprintf(a, __VA_ARGS__)
    #else
        #define SFORMAT             snprintf
        #define VSFORMAT            vsnprintf
    #endif
#endif


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      ProtoNStyle::ProtoNStyle
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
ProtoNStyle::ProtoNStyle()
{
    outBuff         = nil;
    outBuffSize     = 0;
    delimeter       = ",";
}
// ***************************************************************************
// TYPE
//      Destructor
// FUNCTION
//      ProtoNStyle::~ProtoNStyle
// PURPOSE
//      Class destruction
// PARAMETERS
//          --
// ***************************************************************************
ProtoNStyle::~ProtoNStyle()
{

}
// ***************************************************************************
// FUNCTION
//      ProtoNStyle::vput
// PURPOSE
//
// PARAMETERS
//      PCSTR   type --
//      PCSTR   msg  --
//      va_list list --
// RESULT
//      INT --
// ***************************************************************************
INT ProtoNStyle::vput(PCSTR          type,
                      PCSTR          msg,
                      va_list        list)
{
    int     result = 0;
    int     typeLen;
    PSTR    on;
    UINT    left;
    va_list copy;

    if (outBuff && outBuffSize)
    {
        if (lock())
        {
            on          = outBuff;
            left        = outBuffSize;
            left       -= END_SIZE;
            typeLen     = strz_len(type, -1);

            *(on++)     = '$';
            left       -= 1;

            memcpy(on, type, typeLen);

            left       -= typeLen;
            on         += typeLen;

            *(on++)     = ',';
            left       -= 1;

            va_copy(copy, list);
            result      = VSFORMAT(on, left, msg, copy); // strz_vsformat(on, left, msg, copy);
            va_end(copy);

            if ((result = addTailToBuff()) > 0)
            {
                result = (write(outBuff, result) > 0);
            }

            unlock();
        }
    }

    return result;
}
// ***************************************************************************
// FUNCTION
//      ProtoNStyle::put
// PURPOSE
//
// PARAMETERS
//      PCSTR type --
//      PCSTR msg  --
//            ...  --
// RESULT
//      INT --
// ***************************************************************************
INT ProtoNStyle::put(PCSTR          type,
                     PCSTR          msg,
                     ...            )
{
    int     result = 0;
    va_list args;

    va_start(args, msg);
    result = vput(type, msg, args);
    va_end(args);

    return result;
}
// ***************************************************************************
// FUNCTION
//      ProtoNStyle::newSentence
// PURPOSE
//
// PARAMETERS
//      PCSTR type --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL ProtoNStyle::newSentence(PCSTR          type)
{
    BOOL            result          = false;

    ENTER(true);

    if (outBuff && outBuffSize)
    {
        if (lock())
        {
            SFORMAT(outBuff, outBuffSize, "$%s", type);

            sentenceLeft    = outBuffSize - strlen(outBuff);
            result          = true;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      ProtoNStyle::addParam
// PURPOSE
//
// PARAMETERS
//      INT   paramType --
//      PVOID param     --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL ProtoNStyle::addParam(INT            paramType,
                           PCVOID         param,
                           INT            limit1,
                           INT            limit2)
{
    BOOL            result          = true;
    PAXDATE         date;
    PSTR            on              = outBuff + (outBuffSize - sentenceLeft);

    ENTER(true);

    if (sentenceLeft > 0)
    {
        switch (paramType)
        {
            case PARAM_BIN:
                if (sentenceLeft >= (UINT)((limit1 << 1) + 1))
                {
                    strz_cpy(on, delimeter, sentenceLeft);
                    strz_cpy(on, "x", sentenceLeft);
                    strz_bin2hex(on + strlen(delimeter) + 1, (PVOID)param, limit1);
                }
                else
                    result = false;
                break;

            case PARAM_DOUBLE:
                if (!isnan(*((double*)param)))
                {
                    strz_cpy(on, delimeter, sentenceLeft);
                    modp_dtoa(*((double*)param), on + 1, limit2 ? limit2 : 4);

//                    SFORMAT(on, sentenceLeft, ",%*.*f",
//                             limit1 ? limit1 : 1,
//                             limit2 ? limit2 : 4,
//                             *((double*)param));
                }
                else
                    SFORMAT(on, sentenceLeft, delimeter);
                break;

            case PARAM_FLOAT:
                if (!isnan(*((float*)param)))
                {
                    strz_cpy(on, delimeter, sentenceLeft);
                    modp_dtoa(*((float*)param), on + 1, limit2 ? limit2 : 4);

//                    SFORMAT(on, sentenceLeft, ",%*.*f",
//                             limit1 ? limit1 : 1,
//                             limit2 ? limit2 : 4,
//                             *((float*)param));
                }
                else
                    SFORMAT(on, sentenceLeft, delimeter);
                break;

            case PARAM_INT_NZ:
                if (!*((signed*)param))
                {
                    SFORMAT(on, sentenceLeft, delimeter);
                    break;
                }
                // no break;

            case PARAM_INT:
                SFORMAT(on, sentenceLeft, "%s%d", delimeter, *((int*)param));
                break;

            case PARAM_UINT_NZ:
                if (!*((unsigned*)param))
                {
                    SFORMAT(on, sentenceLeft, delimeter);
                    break;
                }
                // no break;

            case PARAM_UINT:
                SFORMAT(on, sentenceLeft, "%s%u", delimeter, *((unsigned*)param));
                break;

            case PARAM_UINT64:
#if (TARGET_ARCH == __AX_lpc__)
                SFORMAT(on, sentenceLeft, ",%lu", *((unsigned long*)param));
#else
                SFORMAT(on, sentenceLeft, "%s%llu", delimeter, *((unsigned long long*)param));
#endif
                break;

            case PARAM_HEX_NZ:
                if (!*((unsigned*)param))
                {
                    SFORMAT(on, sentenceLeft, delimeter);
                    break;
                }
                // no break;

            case PARAM_HEX:
                SFORMAT(on, sentenceLeft, "%sx%X", delimeter, *((unsigned*)param));
                break;

            case PARAM_QUOTEDSTR:
                SFORMAT(on, sentenceLeft, "%s\"%s\"", delimeter, (PSTR)param);
                break;

            case PARAM_STR:
                if (param)
                    SFORMAT(on, sentenceLeft, "%s%s", delimeter, (PSTR)param);
                else
                    SFORMAT(on, sentenceLeft, delimeter);
                break;

            case PARAM_DATE:
                date = (PAXDATE)param;

                SFORMAT(on, sentenceLeft, "%s%.2u%.2u%.2u%.2u%.2u%.4u",
                        delimeter,
                        date->hour, date->minute, date->second,
                         date->day,  date->month,  date->year);
                break;

            default:
                SFORMAT(on, sentenceLeft, delimeter);
                break;
        }

        sentenceLeft = outBuffSize - strlen(outBuff);
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      ProtoNStyle::addTailToBuff
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      UINT --
// ***************************************************************************
UINT ProtoNStyle::addTailToBuff(UINT           seq)
{
    BOOL            result          = false;

    ENTER(true);

    nmea_add_checksum(outBuff, outBuffSize);

    result = strz_len(outBuff, -1);

    if (seq)
    {
        SFORMAT(outBuff + result, -1, (PSTR)" %d", seq);

        result = strz_len(outBuff, -1);
    }

    if ((outBuffSize - result) > 3)
    {
        memcpy(outBuff + result, "\r\n", 3);

        result += 2;
    }
    else
        result = 0;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      ProtoNStyle::putSentence
// PURPOSE
//
// PARAMETERS
//      PCSTR type --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL ProtoNStyle::putSentence(UINT seq)
{
    BOOL            result          = false;

    ENTER(true);

    if ((result = addTailToBuff(seq)) > 0)
    {
        //printf("OUT: %s", outBuff);

        result = (write(outBuff, result) > 0);
    }

    unlock();

    RETURN(result);
}
BOOL ProtoNStyle::postSentence(PAXRINGBUFF    buff)
{
    BOOL        result          = false;

    ENTER(true);

    if ((result = addTailToBuff()) > 0)
    {
        result = (axringbuff_write(buff, outBuff, result) > 0);
    }

    RETURN(result);
}
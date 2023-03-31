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

#include <Snarkell.h>
#include <axstring.h>
#include <axthreads.h>
#include <customboard.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

int Snarkell::receive(int            timeout)
{
    int         result          = 0;
    PSTR        key;
    PSTR        on;

    ENTER(true);

    if (inBuffer.gets(timeout) > 0)
    {
        parseOn = strz_clean(inBuffer.getStrz());

        if (((parseLen = (UINT)strlen(parseOn)) > 0))
        {
            if (checkCs(parseOn, parseLen))
            {
                if (flags.debug)
                    printf("<< '%s'\n", parseOn);

                if ((parseOn = strz_substrs_get_u(parseOn, &parseLen, separator)) != nil)
                {
                    key = parseOn;

                    if (flags.seqIsFirstParam)
                    {
                        currInSeq = 0;

                        if ((on = getNextField()) != nil)
                        {
                            if (*on)
                            {
                                currInSeq = (UINT)atol(on);
                            }
                        }
                    }

                    if (currInSeq)
                    {
                        if (currInSeq > lastInSeq)
                        {
                            UINT diff = currInSeq - lastInSeq - 1;

                            if (diff > 0)
                            {
                                numOfLost += diff;
                            }
                        }

                        lastInSeq = currInSeq;
                    }

                    result  = parseMessage(key);
                }
            }
            else
            {
                numOfErrors++;
            }
        }
    }

    RETURN(result);
}
BOOL Snarkell::snarkellInit(AXDevice *     dev,
                            UINT           inBuffSize,
                            char           separator,
                            EOLMODE        eolmode)
{
    BOOL        result          = false;

    ENTER(true);

#ifdef SNARKELL_USE_LCK
    LCK = axmutex_create();
#endif

    if (inBuffer.open(dev, inBuffSize))
    {
        if (separator)
            this->separator = separator;

        inBuffer.setEolMode(eolmode);
        result = true;
    }

    RETURN(result);
}
PSTR Snarkell::getNextField()
{
    parseOn = strz_substrs_get_u(parseOn, &parseLen, separator);

    return parseOn;
}
int Snarkell::send(PCSTR          message,
                   ...)
{
    int         result          = 0;
    va_list         st_va_list;
    unsigned char   cs          = 0;
    const char *    on          = buffOut + 2;

    ENTER(true);

#ifdef SNARKELL_USE_LCK
    axmutex_lock(LCK, true);
#endif

    va_start(st_va_list, message);
    result = vsprintf(buffOut, message, st_va_list);
    va_end(st_va_list);

    for (int i = 1; i < result; on++, i++)
    {
        cs ^= *on;
    }

    result += sprintf(buffOut + result, "*%.2X\n", cs);

#ifdef CBRD_PP_SNARKEL_485_RE
    Hal::getInstance().gpio.setPin(CBRD_PP_SNARKEL_485_RE, 1);
#endif
    if (inBuffer.write(buffOut, result, 1000) > 0)
    {
        currOutSeq++;
    }
#ifdef CBRD_PP_SNARKEL_485_RE
    Hal::getInstance().gpio.setPin(CBRD_PP_SNARKEL_485_RE, 0);
#endif

#ifdef SNARKELL_USE_LCK
    axmutex_unlock(LCK);
#endif

    RETURN(result);
}

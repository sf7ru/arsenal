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

#define SNARKELL_EXTRA


#ifndef __Snarkell_H__
#define __Snarkell_H__

#include <arsenal.h>
#include <string.h>
#include <stdio.h>

#include <customboard.h>
#include <axthreads.h>
#ifdef SNARKELL_USE_SINGLETON
#include <singleton_template.hpp>
#endif

#ifdef SNARKELL_EXTRA
#include <AXBuffer.h>
#endif

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#ifndef SNARKELL_OUTBUFF_SZ
#define SNARKELL_OUTBUFF_SZ      128
#endif


// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class Snarkell
#ifdef SNARKELL_USE_SINGLETON
    : public SingletonTemplate<Snarkell>
#endif

{
protected:
        char            separator;
        UINT            numOfLost;
        UINT            numOfErrors;
        U16             currOutSeq;
        UINT            currInSeq;
        UINT            lastInSeq;
        HAXMUTEX        LCK;
//        char            buffTmp                 [ SNARKELL_OUTBUFF_SZ ];
        char            buffOut                 [ SNARKELL_OUTBUFF_SZ ];

        struct
        {
            UINT        seqIsFirstParam     : 1;
            UINT        debug               : 1;
        } flags;

        unsigned        checkCs                 (char *         begin,
                                                 unsigned &     len)
        {
            int             result          = 0;
            char *          on              = begin;
            unsigned char   oriCs;
            unsigned char   cs              = 0;

            if (*on == '$')
                on++;

            while (*on && *on != '*')
            {
                cs ^= *on;

                on++;
            }

            if (*on == '*')
            {
                oriCs = (unsigned char)strtol(on + 1, 0, 16);

                if (cs || (cs == oriCs))
                {
                    currInSeq = (UINT)strtol(on + 1 + 2, 0, 10);

                    result = 1;
                }

                *on = 0;
            }

            len = (int)(on - begin);

            return result;
        }

        void            message                 (const char *   msg,
                                                 ...)
        {
            va_list         st_va_list;
            int             len;
            unsigned char   cs          = 0;
            const char *    on          = buffOut + 1;

            va_start(st_va_list, msg);
            len = vsprintf(buffOut, msg, st_va_list);
            va_end(st_va_list);

            for (int i = 1; i < len; on++, i++)
            {
                cs ^= *on;
            }

            sprintf(buffOut + len, "*%.2X\n", cs);

            write(buffOut, len + 4);
        }

virtual int             write                   (const char *   msg,
                                                 int            sz)
        { return 0; }


public:

                        Snarkell               ()
                        {
                            separator               = ',';
                            numOfLost               = 0;
                            numOfErrors             = 0;
                            lastInSeq               = 0;
                            currInSeq               = 0;
                            currOutSeq              = 1;
                            flags.seqIsFirstParam   = 0;
                        }

virtual                 ~Snarkell               ()
                        {}

        UINT            getNumOfLost            ()
        { UINT result = numOfLost; numOfLost = 0; return result; }



#ifdef SNARKELL_EXTRA

        PSTR            parseOn;
        UINT            parseLen;

        void            setDebug                (BOOL           ena)
        { flags.debug = (UINT)ena; }

        AXBuffer        inBuffer;
        int             receive                 (int            timeout);

        int             send                    (PCSTR          message,
                                                 ...);

virtual int             parseMessage            (PSTR           key)
        { return -1; }

        BOOL            snarkellInit            (AXDevice *     dev,
                                                 UINT           inBuffSize,
                                                 char           separator = 0,
                                                 EOLMODE        eolmode   = EOLMODE_lf);

        PSTR            getNextField            ();
#endif

};

#endif // #define __Snarkell_H__

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

#ifndef __AXBUFFERPPH__
#define __AXBUFFERPPH__

#include <AXDevice.h>
#include <axdata.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------


class AXBuffer
{
        AXDevice *      dev;
        PU8             buffData;
        UINT            buffSize;
        UINT            used;
        UINT            seen;
        UINT            strEnd;
        BOOL            allocated;

        INT             echoMode;


        void            echo                    (UINT           size,
                                                 UINT           TO);


        void            shake                   (void);

        INT             searchString            (UINT           start);

        BOOL            _realloc                (UINT       size);


        CHAR            eol1;
        CHAR            eol2;
public:

                        AXBuffer                (void);
                        ~AXBuffer               (void);

        BOOL            close                   (void);

        BOOL            open                    (AXDevice *     dev,
                                                 UINT           size,
                                                 EOLMODE        eolmode  = EOLMODE_crlf);

        BOOL            open2                   (AXDevice *     theDev,
                                                 PVOID          buff,
                                                 UINT           size,
                                                 EOLMODE        eolmode = EOLMODE_crlf);

        INT             gets                    (UINT           TO);

        INT             readDevice              (UINT           TO);

        INT             read                    (UINT           TO,
                                                 INT            size = 0);

        INT             read                    (PVOID          data,
                                                 INT            size,
                                                 UINT           TO);

        INT             write                   (PCVOID         data,
                                                 INT            size,
                                                 UINT           TO);

        PSTR            getStrz                 (void);

        PCVOID          getData                 (void)
                        { return (PCVOID)(buffData); };

        INT             getSize                 (void)
                        { return used - seen; };

        INT             getFreeSize             (void)
                        { return buffSize - used; };

        INT             getTotalSize            (void)
                        { return buffSize; };

        INT             purge                   (UINT           size)
                        { if (size) seen = MAC_MIN(size, used); shake(); return used; }

        void            purgeAll                ()
                        { seen = 0; used = 0; }

//        UINT            getStrLen               (void)
//                        { return strEnd ? (strEnd - seen) : 0; }
//
        void            setEolMode              (EOLMODE    m)
        { m == EOLMODE_cr ? setCr() : m == EOLMODE_lf ? setLf() : setCrLf(); }

        void            setCr                   (void)
                        { eol1 = 0; eol2 = '\r'; }

        void            setLf                   (void)
                        { eol1 = 0; eol2 = '\n'; }

        void            setCrLf                 (void)
                        { eol1 = '\r'; eol2 = '\n'; }

        void            setEol                  (CHAR           eol1,
                                                 CHAR           eol2)
        { this->eol1 = eol1; this->eol2 = eol2; }

        BOOL            setDevice               (AXDevice *     theDev)
                        { dev = theDev; return true; }

        void            closeDevice             (void)
                        { if (dev) delete dev; }

        void            setEchoMode             (INT            mode)
        { echoMode = mode; }

        INT             push                    (PVOID          data, 
                                                 INT            size);
};


#endif //  #ifndef __AXBUFFERPPH__


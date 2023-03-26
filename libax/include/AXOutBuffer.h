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

#ifndef __AXOUTBUFFERPPH__
#define __AXOUTBUFFERPPH__

#include <AXDevice.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------


class AXOutBuffer
{
        AXDevice *      dev;
        PU8             buffData;
        UINT            buffSize;
        UINT            used;

        BOOL            _realloc                (UINT       size);

public:

                        AXOutBuffer             (void);
                        ~AXOutBuffer            (void);

        BOOL            close                   (void);

        BOOL            open                    (AXDevice *     dev,
                                                 UINT           size);

        INT             write                   (PCVOID         data,
                                                 INT            size,
                                                 UINT           TO);

        INT             flush                   (UINT           TO);

        PCVOID          getData                 (void)
                        { return (PCVOID)(buffData); };

        INT             getSize                 (void)
                        { return used; };

        INT             getFreeSize             (void)
                        { return buffSize - used; };

        INT             getTotalSize            (void)
                        { return buffSize; };

        void            purge                   ()
                        { used = 0; }

        BOOL            setDevice               (AXDevice *     theDev)
                        { dev = theDev; return true; }

        void            closeDevice             (void)
                        { if (dev) delete dev; }
};


#endif //  #ifndef __AXOUTBUFFERPPH__


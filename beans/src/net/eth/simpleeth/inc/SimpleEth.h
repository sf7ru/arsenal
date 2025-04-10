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

#ifndef __ETHH__
#define __ETHH__

#include <arsenal.h>
#include <axstring.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class SimpleEth
{
        int             sock;
        int             ifindex;
        int             proto;

        BOOL            determineIndex          (void);
        BOOL            determineMac            (void);
        BOOL            bindToIface             (void);
        BOOL            setPromisc              (BOOL           enable);


        U8              ifmac                   [ 6 ];

        CHAR            iface                   [ 17 ];

        CHAR            ifmacString             [ AXLSHORT ];

public:

                        SimpleEth                     (void);
                        ~SimpleEth                    (void);

static  SimpleEth *           create                  (PCSTR          iface,
                                                 UINT           recvBuffSize,
                                                 UINT           sendBuffSize);

        int             recv                    (PVOID          buffer,
                                                 INT            size,
                                                 UINT           TO);

        int             send                    (PVOID          buffer,
                                                 INT            size);

        BOOL            setSendBuffSize         (int            size);
        BOOL            setRecvBuffSize         (int            size);

        PU8             getMac                  (void)
        { return ifmac; }

        PCSTR           getMacString            (void)
        { return ifmacString; }
        PCSTR           getName                 (void)
        { return iface; }
};

#endif //  #define __ETHH__

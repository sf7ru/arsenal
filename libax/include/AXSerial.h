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

#ifndef __AXSERIALPPH__
#define __AXSERIALPPH__

#include <serial.h>
#include <AXDevice.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------


class AXSerial: public AXDevice
{
        PAXDEV          dev;

public:
                        AXSerial                (void);

                        ~AXSerial               (void);


        BOOL            close                   (void);

        BOOL            isOpen                  (void);

        BOOL            open                    (PSERIALPROPS   props,
                                                 UINT           type);

        BOOL            open                    (PSERIALPROPS   props,
                                                 UINT           type,
                                                 PCSTR          name);

        INT             read                    (PVOID          data,
                                                 INT            size,
                                                 UINT           TO);

        INT             write                   (PCVOID          data,
                                                 INT            size,
                                                 UINT           TO);

        BOOL            setBaudrate             (UINT           baudrate);

        void            setDtr                  (BOOL           value);

        void            setRts                  (BOOL           value);

        INT             getPendingTx            ();


        INT             getDescriptor           ()
        { return dev->pfn_ctl ? axdev_ctl(dev, AXDEVCTL_GETFD, 0, 0) : -1; }
};

#endif //  #ifndef __AXSERIALPPH__

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

#ifndef __AXDEVICEPPH__
#define __AXDEVICEPPH__

#include <arsenal.h>
#include <axsystem.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------


class AXDevice
{
public:
virtual                 ~AXDevice               (void)
                        {};

virtual INT             read                    (PVOID          data,
                                                 INT            size,
                                                 UINT           TO)
                        = 0;

virtual INT             write                   (PCVOID         data,
                                                 INT            size,
                                                 UINT           TO)
                        = 0;

virtual INT             getDescriptor           ()
                        { return -1; }
};

class PAXDEVWrapper: public AXDevice
{
        PAXDEV          dev;

public:
                        ~PAXDEVWrapper          (void)
                        { if (dev != nil) axdev_close(dev, (UINT)-1); }

                        PAXDEVWrapper           (PAXDEV         theDev)
                        { dev = theDev; }

        INT             read                    (PVOID          data,
                                                 INT            size,
                                                 UINT           TO)
                        { return axdev_read(dev, data, size, TO); }

        INT             write                   (PCVOID         data,
                                                 INT            size,
                                                 UINT           TO)
                        { return axdev_write(dev, (PVOID)data, size, TO); }

        void            setDev                  (PAXDEV         theDev)
        { dev = theDev; }

        INT             getDescriptor           ()
                        { return dev->pfn_ctl ? axdev_ctl(dev, AXDEVCTL_GETFD, 0, 0) : -1; }
};


class HAXPIPEWrapper: public AXDevice
{
        HAXPIPE         pipeIn;
        HAXPIPE         pipeOut;

public:
                        ~HAXPIPEWrapper         (void)
                        { SAFEDESTROY(axpipe, pipeIn); SAFEDESTROY(axpipe, pipeOut);}

                        HAXPIPEWrapper          ()
                        { this->pipeIn = nil;  this->pipeOut = nil; }

                        HAXPIPEWrapper          (HAXPIPE    pipeIn,
                                                 HAXPIPE    pipeOut)
                        { setPipes(pipeIn, pipeOut); }

        void            setPipes                (HAXPIPE    pipeIn,
                                                 HAXPIPE    pipeOut)
                        {
                            this->pipeIn = pipeIn;  this->pipeOut = pipeOut;
                        }

        INT             read                    (PVOID          data,
                                                 INT            size,
                                                 UINT           TO)
                        {
                            return axpipe_read(pipeIn, data, size, TO);
                        }

        INT             write                   (PCVOID         data,
                                                 INT            size,
                                                 UINT           TO)
                        { return axpipe_write(pipeOut, (PVOID)data, size, TO); }
};



#endif //  #ifndef __AXDEVICEPPH__


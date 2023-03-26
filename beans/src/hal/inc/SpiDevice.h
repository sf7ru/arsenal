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

#ifndef __SPIDEVICE_H__
#define __SPIDEVICE_H__

#include <arsenal.h>

#include <Spi.h>
#include <PortPin.h>
#include <Hal.h>

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class SpiDevice
{
        friend class    Spi;

protected:
        Spi *           parent;
        UINT            mIfaceNo;
        PORTPIN         ppCs;
        INT             mDevFd;

virtual void            onFrameAssertSsel       () {};
virtual void            onFrameDeassertSsel     () {};
virtual void            onTransAssertSsel       ()
                        {
                            if (PP_IS_DEFINED(ppCs))
                            {
                                Hal::getInstance().gpio.setPin(ppCs, 0);
                            }
                        }
virtual void            onTransDeassertSsel     ()
                        {
                            if (PP_IS_DEFINED(ppCs))
                            {
                                Hal::getInstance().gpio.setPin(ppCs, 1);
                            }
                        }

        BOOL            manualSelect;

public:

        PVOID           control;

        //BOOL            flagNonBlock;

                        SpiDevice               ()
                        { mDevFd = -1; mIfaceNo = 0; control = 0; manualSelect = false; }

virtual                 ~SpiDevice              () {};

        BOOL            init                    (UINT           ifaceNo     = 0);

        U8              readReg                 (UINT           reg);

        INT             read                    (UINT           reg,
                                                 PVOID          data,
                                                 INT            size);

        INT             write                   (UINT           reg,
                                                 PVOID          data,
                                                 INT            size);

        int             writeReg                (UINT           reg,
                                                 U8             value);

        int             checkedWriteReg         (UINT           reg,
                                                 U8             value,
                                                 UINT           tries);

        int             modifyReg               (UINT           reg,
                                                 U8             clearbits,
                                                 U8             setbits);

        int             checkedModifyReg        (UINT           reg,
                                                 U8             clearbits,
                                                 U8             setbits,
                                                 UINT           tries);

        int             transfer                (PU8            send,
                                                 PU8            recv,
                                                 int            len);

        int             transmit                (PVOID          data,
                                                 int            len);



        U8              transferByte            (U8             data);

        void            select                  (BOOL           val); 

        void            forceUnselect           (BOOL           val); 

        Spi *           getIface                ()
        { return parent; }
};

#endif // #ifndef __SPIDEVICE_H__

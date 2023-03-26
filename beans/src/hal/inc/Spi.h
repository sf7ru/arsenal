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

#ifndef __SPI_H__
#define __SPI_H__

#include <arsenal.h>
#include <axthreads.h>

#define SPI_MAX_IFACES      3
#define SINGLETON_UNIQUES_NUMBER    SPI_MAX_IFACES
#include <singleton_template.hpp>
#undef SINGLETON_UNIQUES_NUMBER

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------


#define SPI_DUMMY_BYTE      0XFF

#define SPI_IFACES_NO       3
#define SPI_MDEVS_NO        4

#define SPI_IFACE_SOFTWARE  SPI_IFACES_NO

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct __tag_SPIMODE
{
    UINT            cpol        :  1;
    UINT            cph         :  1;
    UINT            firstMsb    :  1;
    UINT            bits        :  7;
    BOOL            master      :  1;
    BOOL            softSelect  :  1;
    UINT            baudrateKHz : 20;
} SPIMODE, * PSPIMODE;

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class SpiDevice;

class Spi: public SingletonTemplate<Spi>
{
        friend class    SpiDevice;
        friend          void staticOnTransAssertSsel0();
        friend          void staticOnTransDeassertSsel0();

        INT             ifaceNo;
        HAXMUTEX        LCK;

        SPIMODE         mode;

        SpiDevice *     lockedDevice;

        INT             mDevFds                 [ SPI_MDEVS_NO ];

protected:

        void            onTransAssertSsel       ();

        void            onTransDeassertSsel     ();


        void            constructClass          ();
        BOOL            initBase                ();
        void            deinitBase              ();

        BOOL            initMsp                 ();

        UINT            getModeNumber           ()
        { return ((mode.cpol << 1) | mode.cph); }

public:

        void            lockDevice              (SpiDevice *    dev);
        void            unlockDevice            (SpiDevice *    dev);

        int             transfer                (PU8            send,
                                                 PU8            recv,
                                                 int            len,
                                                 UINT           TO   = -1);

        int             transmit                (PU8            send,
                                                 int            len);

        int             receive                 (PU8            buff,
                                                 int            len);


                        Spi                     ();
                        ~Spi                    ();

        BOOL            init                    (SpiDevice *    dev);

        void            deinit                  (UINT           ifaceNo = 1);

        U8              readReg                 (UINT           reg);

        U16             readReg16               (UINT           reg);

        int             writeReg                (UINT           reg,
                                                 U8             value);

        int             modifyReg               (UINT           reg,
                                                 U8             clearbits,
                                                 U8             setbits);

        INT             read                    (UINT           reg,
                                                 PVOID          data,
                                                 INT            len);

        INT             write                   (UINT           reg,
                                                 PVOID          data,
                                                 INT            len);

        SPIMODE         getMode                 ()
        { return mode; }
};

#endif // #ifndef __SPI_H__

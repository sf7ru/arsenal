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

#ifndef __I2C_H__
#define __I2C_H__

#include <arsenal.h>
#include <axthreads.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define I2C_MAX_IFACES      5
#define SINGLETON_UNIQUES_NUMBER    I2C_MAX_IFACES
#include <singleton_template.hpp>
#undef SINGLETON_UNIQUES_NUMBER

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class I2cDevice;

class I2c: public SingletonTemplate<I2c>
{
        friend class    I2cDevice;

        INT             ifaceNo;
        INT             mFd;
        UINT            mSpeed;

        HAXMUTEX        LCK;
        I2cDevice *     lockedDevice;

protected:

//        UINT            nextDevIndex;

        void            lockDevice              (I2cDevice *    dev);
        void            unlockDevice            (I2cDevice *    dev);

        INT             masterRead              (UINT           addr,
                                                 PVOID          buff,
                                                 INT            size);

        INT             masterWrite             (UINT           addr,
                                                 PCVOID         buff,
                                                 INT            size);

        INT             masterTransaction       (UINT           addr,
                                                 PCVOID         outBuff,
                                                 INT            outSize,
                                                 PVOID          inBuff,
                                                 INT            inSize);
public:


                        I2c                     ();
                        ~I2c                    ();

         int            probeSlaves             ();

         BOOL           init                    (UINT           speed   = 100000);

         void           deinit                  ();

         void           recover                 ();

         void           bitBangCLK              (int            value);

//         U8             readReg                 (UINT           reg);
//
//         int            writeReg                (UINT           reg,
//                                                 U8             value);
//
//         int            modifyReg               (UINT           reg,
//                                                 U8             clearbits,
//                                                 U8             setbits);
};

#endif // #ifndef __I2C_H__

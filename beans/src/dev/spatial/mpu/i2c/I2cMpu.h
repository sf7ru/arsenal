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

#ifndef ____I2CMPU6XXX_H__
#define ____I2CMPU6XXX_H__

#include <Mpu_pvt.h>

#include <I2c.h>
#include <I2cDevice.h>
#include <Mpu.h>
#include <PortPin.h>


// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class I2cMpu: public I2cDevice, public Mpu
{
protected:

        INT             registerWrite           (U8             reg_addr,
                                                 U8             data)
        { return I2cDevice::registerWrite(reg_addr, data); }

        INT             registerWrite           (U8             reg_addr,
                                                 PVOID          data,
                                                 INT            len)
        { return I2cDevice::registerWrite(reg_addr, data, len); }


        INT             registerRead            (U8             reg_addr,
                                                 PVOID          data,
                                                 INT            len)
        { return I2cDevice::registerRead(reg_addr, data, len); }

public:
                        I2cMpu              ();

virtual                 ~I2cMpu             ();

        BOOL            init                    (UINT           addr,
                                                 UINT           ifaceNo,
                                                 UINT           speed,
                                                 PORTPIN        ppInt);

        BOOL            recover                 ();

};

#endif // #ifndef ____I2CMPU6XXX_H__

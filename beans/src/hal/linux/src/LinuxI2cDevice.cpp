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

#include <I2c.h>

#include <I2cDevice.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <stdio.h>

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      I2cDevice::I2cDevice               --
// ***************************************************************************
I2cDevice::I2cDevice               ()
{
    mDevAddr    = 0;
    mIface      = nil;
    mDevFd      = -1;
};
// ***************************************************************************
// FUNCTION
//
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      I2cDevice::~I2cDevice              --
// ***************************************************************************
I2cDevice::~I2cDevice              ()
{
}
// ***************************************************************************
// FUNCTION
//      I2cDevice::init
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL I2cDevice::init(UINT           addr,
                     UINT           ifaceNo,
                     UINT           speed)
{
    BOOL            result          = false;

    ENTER(true);

    mIface      = &I2c::getInstance(ifaceNo);
    mDevAddr    = addr;
    mDevFd      = mIface->init(speed);

    if (mDevFd != -1)
    {
        //ioctl(mDevFd, I2C_SLAVE, mDevAddr);

        result = true;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      I2cDevice::transaction
// PURPOSE
//
// PARAMETERS
//      U8  addr    --
//      INT wr_len  --
//      PU8 wr_data --
//      INT rd_len  --
//      PU8 rd_data --
// RESULT
//      int --
// ***************************************************************************
int I2cDevice::transaction(INT            wr_len,
                           PU8            wr_data,
                           INT            rd_len,
                           PU8            rd_data)
{
    int             result          = -1;

    ENTER(true);

    result = mIface->masterTransaction(mDevAddr, wr_data, wr_len, rd_data, rd_len);

    RETURN(result);
}

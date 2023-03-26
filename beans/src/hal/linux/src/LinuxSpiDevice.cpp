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

#include <Spi.h>

#include <SpiDevice.h>
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
//      SpiDevice::SpiDevice               --
// ***************************************************************************
SpiDevice::SpiDevice               ()
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
//      SpiDevice::~SpiDevice              --
// ***************************************************************************
SpiDevice::~SpiDevice              ()
{
}
// ***************************************************************************
// FUNCTION
//      SpiDevice::init
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL SpiDevice::init(UINT           addr,
                     UINT           ifaceNo,
                     UINT           speed)
{
    BOOL            result          = false;

    ENTER(true);

    mIface      = &Spi::getInstance(ifaceNo);
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
//      SpiDevice::transaction
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
int SpiDevice::transaction(INT            wr_len,
                           PU8            wr_data,
                           INT            rd_len,
                           PU8            rd_data)
{
    int             result          = -1;

    ENTER(true);

    result = mIface->masterTransaction(mDevAddr, wr_data, wr_len, rd_data, rd_len);

    RETURN(result);
}

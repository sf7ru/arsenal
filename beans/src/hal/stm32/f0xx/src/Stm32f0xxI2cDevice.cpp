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
    mIfaceNo    = 0;
    mDevAddr    = 0;
    control     = nil;
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
    I2c &           i2c             = I2c::getInstance();

    ENTER(true);

    mIfaceNo    = ifaceNo;
    mDevAddr    = addr;

    result = i2c.init(this, ifaceNo, speed);

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
//    I2C_XFER_T      xfer;

    ENTER(true);

//    xfer.slaveAddr    = mDevAddr;
//    xfer.rxBuff       = rd_data;
//    xfer.rxSz         = rd_len;
//    xfer.txBuff       = wr_data;
//    xfer.txSz         = wr_len;
//
//    if (Chip_I2C_MasterTransfer((I2C_ID_T)mIfaceNo, &xfer) == I2C_STATUS_DONE)
//    {
//        result = rd_len ? rd_len : wr_len;
//    }

//    if (wr_data)
//    {
//        result = Chip_I2C_MasterSend((I2C_ID_T)mIfaceNo, addr, wr_data, wr_len);
//        printf("I2C sent %u\n", result);
//
//    }
//
//    if (rd_data)
//    {
//        result = Chip_I2C_MasterRead((I2C_ID_T)mIfaceNo, addr, rd_data, rd_len);
//        printf("I2C recv %u\n", result);
//    }

    RETURN(result);
}

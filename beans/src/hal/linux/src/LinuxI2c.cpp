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
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      I2c::I2c
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
I2c::I2c()
{
    ifaceNo = -1;
    mFd     = -1;
}
// ***************************************************************************
// TYPE
//      Destructor
// FUNCTION
//      I2c::~I2c
// PURPOSE
//      Class destruction
// PARAMETERS
//          --
// ***************************************************************************
I2c::~I2c()
{

}
INT I2c::masterTransaction(UINT           addr,
                           PCVOID         outBuff,
                           INT            outSize,
                           PVOID          inBuff,
                           INT            inSize)
{
    INT         result = 0;
    INT         rd;

    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[2];

    packets.nmsgs  = 0;

    ENTER(true);

    if (outBuff && outSize)
    {
        messages[packets.nmsgs].addr  = (__u16)addr;
        messages[packets.nmsgs].flags = 0;
        messages[packets.nmsgs].len   = (__u16)outSize;
        messages[packets.nmsgs].buf   = (__u8*)outBuff;

        packets.nmsgs++;
    }

    if (inBuff && inSize)
    {
        messages[packets.nmsgs].addr  = (__u16)addr;
        messages[packets.nmsgs].flags = I2C_M_RD;
        messages[packets.nmsgs].len   = (__u16)inSize;
        messages[packets.nmsgs].buf   = (__u8*)inBuff;

        packets.nmsgs++;
    }

    packets.msgs  = messages;

    rd = ioctl(mFd, I2C_RDWR, &packets);

//    if (rd == -1)
//    {
//        perror("I2c: ");
//    }

    result = (rd > 0) ? (inSize ? inSize : outSize) : 0;

    RETURN(result);
}
BOOL I2c::init(unsigned speed)
{
    BOOL        result          = false;
    CHAR        path            [ AXLPATH ];

    ENTER(true);

    ifaceNo = getSingletoneIndex();

    if (ifaceNo < I2C_MAX_IFACES)
    {

        sprintf(path, "/dev/i2c-%d", ifaceNo
#ifndef I2C_NO_INC_IFACE
                                     + 1
#endif
        );

        if ((mFd = open(path, O_RDWR)) != -1)
        {
            result = true;
        }
        else
            perror(path);
    }

    RETURN(result);
}
void I2c::recover()
{

}
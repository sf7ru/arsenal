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

#include <I2cMpu.h>

#include <axtime.h>

#include <string.h>
#include <math.h>
#include <Hal.h>

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      I2cMpu::I2cMpu
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
I2cMpu::I2cMpu()
{
}
// ***************************************************************************
// TYPE
//      Destructor
// FUNCTION
//      I2cMpu::~I2cMpu
// PURPOSE
//      Class destruction
// PARAMETERS
//          --
// ***************************************************************************
I2cMpu::~I2cMpu()
{

}
// ***************************************************************************
// FUNCTION
//      I2cMpu::init
// PURPOSE
//
// PARAMETERS
//      UINT                     addr --
//      UINT           ifaceNo = 0    --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL I2cMpu::init(UINT           addr,
                  UINT           ifaceNo,
                  UINT           speed,
                  PORTPIN        ppInt)
{
    BOOL            result          = false;

    ENTER(true);

    if (I2cDevice::init(addr ? addr : DEFAULT_I2C_ADDR_MPU6XXX, ifaceNo, speed ? speed : 100 KHZ))
    {
        result = mpuInit(ppInt);
    }

    RETURN(result);
}
BOOL I2cMpu::recover()
{
    BOOL result = false;

    ENTER(true);

    mIface->recover();

    result = mpuInit(PP_UNDEF);


    RETURN(result);
}

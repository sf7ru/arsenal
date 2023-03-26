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

//#define DBGTRACE

#include <SpiDevice.h>

#include <app.h>

#include <axdbg.h>
#include <stdio.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      SpiDevice::init
// PURPOSE
//
// PARAMETERS
//      UINT ifaceNo --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL SpiDevice::init(UINT           ifaceNo)
{
    BOOL            result          = false;

    ENTER(true);

    parent = &Spi::getInstance(ifaceNo);

    if (parent->init(this))
    {
        onFrameDeassertSsel();
        onTransDeassertSsel();

        result = true;
    }

    RETURN(result);
}

// ***************************************************************************
// FUNCTION
//      SpiDevice::readReg
// PURPOSE
//
// PARAMETERS
//      UINT reg --
// RESULT
//      U8 --
// ***************************************************************************
U8 SpiDevice::readReg(UINT           reg)
{
    U8              result          = 0;

    ENTER(true);

    parent->lockDevice(this);
    result = parent->readReg(reg);
    parent->unlockDevice(this);

    RETURN(result);
}
INT SpiDevice::read(UINT           reg,
                    PVOID          data,
                    INT            size)
{
    INT             result          = 0;

    ENTER(true);

    parent->lockDevice(this);
    result = parent->read(reg, data, size);
    parent->unlockDevice(this);

//    AXTRACE("readRange %X sz %d = %X\n", reg, size, result);

    RETURN(result);
}
INT SpiDevice::write(UINT           reg,
                    PVOID          data,
                    INT            size)
{
    INT             result          = 0;

    ENTER(true);

    parent->lockDevice(this);
    result = parent->write(reg, data, size);
    parent->unlockDevice(this);

    //printf("SpiDevice.cpp:107 stage size = %d result = %d\n", size, result);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SpiDevice::writeReg
// PURPOSE
//
// PARAMETERS
//      UINT reg   --
//      U8   value --
// RESULT
//      int --
// ***************************************************************************
int SpiDevice::writeReg(UINT           reg,
                        U8             value)
{
    int             result          = -1;

    ENTER(true);

    parent->lockDevice(this);
    result = parent->writeReg(reg, value);
    parent->unlockDevice(this);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SpiDevice::checkedWriteReg
// PURPOSE
//
// PARAMETERS
//      UINT reg   --
//      U8   value --
//      UINT tries --
// RESULT
//      int --
// ***************************************************************************
int SpiDevice::checkedWriteReg(UINT        reg,
                               U8          value,
                               UINT        tries)
{
    int             result          = -1;
    U8              rd              = ~value;

    parent->lockDevice(this);

    for (UINT i = 0; (i < tries) && (rd != value); i--)
    {
        if ((result = parent->writeReg(reg, value)) > 1)
        {
            rd = parent->readReg(reg);

            if (rd != value)
            {
                AXTRACE("Spi: err chk wr reg %u", reg);
            }
        }
        else
            AXTRACE("Spi: err wr reg %u", reg);
    }

    parent->unlockDevice(this);

    return result;
}
// ***************************************************************************
// FUNCTION
//      SpiDevice::modifyReg
// PURPOSE
//
// PARAMETERS
//      UINT reg       --
//      U8   clearbits --
//      U8   setbits   --
// RESULT
//      int --
// ***************************************************************************
int SpiDevice::modifyReg(UINT           reg,
                         U8             clearbits,
                         U8             setbits)
{
    int             result          = -1;

    ENTER(true);

    parent->lockDevice(this);
    result = parent->modifyReg(reg, clearbits, setbits);
    parent->unlockDevice(this);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SpiDevice::checkedModifyReg
// PURPOSE
//
// PARAMETERS
//      UINT reg       --
//      U8   clearbits --
//      U8   setbits   --
//      UINT tries     --
// RESULT
//      int --
// ***************************************************************************
int SpiDevice::checkedModifyReg(UINT           reg,
                                U8             clearbits,
                                U8             setbits,
                                UINT           tries)
{
    int             result          = -1;
    U8              value;
    U8              rd;

    ENTER(true);

    parent->lockDevice(this);

    value  = parent->readReg(reg);
    value &= ~clearbits;
    value |= setbits;
    rd     = ~value;

    for (UINT i = 0; (i < tries) && (rd != value); i--)
    {
        if ((result = parent->modifyReg(reg, clearbits, setbits)) > 1)
        {
            rd = parent->readReg(reg);

            if (rd != value)
            {
                AXTRACE("Spi: err chk mod reg %u", reg);
            }
        }
        else
            AXTRACE("Spi: err mod reg %u", reg);
    }

    parent->unlockDevice(this);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SpiDevice::transfer
// PURPOSE
//
// PARAMETERS
//      PU8 send --
//      PU8 recv --
//      int len  --
// RESULT
//      int --
// ***************************************************************************
int SpiDevice::transfer(PU8            send,
                        PU8            recv,
                        int            len)
{
    int             result          = -1;

    ENTER(true);

    parent->lockDevice(this);
    result = parent->transfer(send, recv, len);
    parent->unlockDevice(this);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SpiDevice::sendByte
// PURPOSE
//
// PARAMETERS
//      U8 data --
// RESULT
//      int --
// ***************************************************************************
U8 SpiDevice::transferByte(U8             data)
{
    U8              result          = -1;

    ENTER(true);

    parent->lockDevice(this);
    result = parent->transfer(&data, &result, 1);
    parent->unlockDevice(this);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SpiDevice::transmit
// PURPOSE
//
// PARAMETERS
//      PVOID data --
//      int   len  --
// RESULT
//      int --
// ***************************************************************************
int SpiDevice::transmit(PVOID        data,
                        int          len)
{
    int             result          = -1;

    ENTER(true);

    parent->lockDevice(this);
    result = parent->transmit((PU8)data, len);
    parent->unlockDevice(this);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SpiDevice::select
// PURPOSE
//
// PARAMETERS
//      BOOL val --
// RESULT
//      void --
// ***************************************************************************
void SpiDevice::select(BOOL val)
{
    if (val)
        onTransAssertSsel();
    else
        onTransDeassertSsel();

    //axsleep(20);

    manualSelect = val;
}
void SpiDevice::forceUnselect(BOOL val)
{
    if (val)
        onTransDeassertSsel();
    // else
    //     onTransAssertSsel();

    //axsleep(20);

    manualSelect = val;
}

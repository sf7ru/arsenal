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

#include <app.h>
#include <string.h>

#include <customboard.h>
#include <axtime.h>
#include <axthreads.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

/* SPI protocol address bits */
#define DIR_READ                (1<<7)
#define DIR_WRITE               (0<<7)
#define ADDR_INCREMENT          (1<<6)


//#include <stdio.h>
//#undef AXTRACE
//#define AXTRACE printf

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

SPIMODE _getMode(int iface)
{
    SPIMODE result = { 0 };

    switch (iface)
    {
#ifdef CBRD_SPI_IDX_0_MODE
        case 0: result = CBRD_SPI_IDX_0_MODE; break;
#endif
#ifdef CBRD_SPI_IDX_1_MODE
        case 1: result = CBRD_SPI_IDX_1_MODE; break;
#endif
#ifdef CBRD_SPI_IDX_2_MODE
        case 2: result = CBRD_SPI_IDX_2_MODE; break;
#endif
#ifdef CBRD_SPI_IDX_3_MODE
        case 3: result = CBRD_SPI_IDX_3_MODE; break;
#endif
#ifdef CBRD_SPI_SOFT_MODE
        case SPI_IFACE_SOFTWARE: result = CBRD_SPI_SOFT_MODE; break;
#endif

        default:
            break;
    }

    return  result;
}

void Spi::constructClass()
{
    lockedDevice    = nil;
    LCK             = nil;
    ifaceNo         = -1;

    for (int i = 0; i < SPI_MDEVS_NO; i++)
    {
        mDevFds[i] = -1;
    }
}
void Spi::deinitBase()
{
    SAFEDESTROY(axmutex, LCK);
}
BOOL Spi::initBase()
{
    BOOL        result          = false;

    ENTER(true);

    ifaceNo         = getSingletoneIndex();
    mode            = _getMode(ifaceNo);

    if ((LCK = axmutex_create()) != nil)
    {
        result = true;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      Spi::lockDevice
// PURPOSE
//
// PARAMETERS
//      SpiDevice * dev --
// RESULT
//      void --
// ***************************************************************************
void Spi::lockDevice(SpiDevice *    dev)
{
    axmutex_lock(LCK, true);
    lockedDevice = dev;
    axmutex_unlock(LCK);

}
// ***************************************************************************
// FUNCTION
//      Spi::unlockDevice
// PURPOSE
//
// PARAMETERS
//      SpiDevice * dev --
// RESULT
//      void --
// ***************************************************************************
void Spi::unlockDevice(SpiDevice *    dev)
{
    axmutex_lock(LCK, true);
    lockedDevice = nil;
    axmutex_unlock(LCK);
}
void Spi::onTransAssertSsel       ()
{
    if (mode.softSelect)
    {
        if (lockedDevice)
        {
            if (!lockedDevice->manualSelect)
            {
                lockedDevice->onTransAssertSsel();
            }
        }
    }
}
void Spi::onTransDeassertSsel()
{
    if (mode.softSelect)
    {
        if (lockedDevice)
        {
            if (!lockedDevice->manualSelect)
            {
                lockedDevice->onTransDeassertSsel();
            }
        }
    }
}
// ***************************************************************************
// FUNCTION
//      Spi::modifyReg
// PURPOSE
//
// PARAMETERS
//      SpiDevice * dev       --
//      UINT        reg       --
//      U8          clearbits --
//      U8          setbits   --
// RESULT
//      int --
// ***************************************************************************
int Spi::modifyReg(UINT reg,
                   U8   clearbits,
                   U8   setbits)
{
    U8 val;

    val = readReg(reg);
    val &= ~clearbits;
    val |= setbits;

    return writeReg(reg, val);
}
// ***************************************************************************
// FUNCTION
//      Spi::writeReg
// PURPOSE
//
// PARAMETERS
//      SpiDevice * dev   --
//      UINT        reg   --
//      U8          value --
// RESULT
//      int --
// ***************************************************************************
int Spi::writeReg(UINT           reg,
                  U8             value)
{
    U8 cmd[2];

    cmd[0] = reg | DIR_WRITE;
    cmd[1] = value;

    return transfer(cmd, cmd, sizeof(cmd));
}
// ***************************************************************************
// FUNCTION
//      Spi::readReg
// PURPOSE
//
// PARAMETERS
//      SpiDevice * dev --
//      unsigned    reg --
// RESULT
//      U8 --
// ***************************************************************************
U8 Spi::readReg(unsigned       reg)
{
    U8 cmd[2];

    cmd[0] = reg | DIR_READ;
    cmd[1] = 0;

    //printf("1) readReg [0]= %X [1]= %X\n", cmd[0], cmd[1]);

    transfer(cmd, cmd, sizeof(cmd));

    //printf("2) readReg [0]= %X [1]= %X\n", cmd[0], cmd[1]);

    return cmd[1];
}


INT Spi::read(UINT           reg,
              PVOID          data,
              INT            len)
{
    INT         result          = -1;
    U8 cmd[len + 1];

    cmd[0] = reg | DIR_READ;

    memset(&cmd[1], 0xFF, len);

    //printf("1) readRange [0]= %X [0]= %X\n", cmd[0], cmd[1]);

    if ((result = transfer(cmd, cmd, sizeof(cmd))) > 0)
    {
        memcpy(data, &cmd[1], len);
        result--;


        //printf("GOT: %x %x %x %x %x %x\n", cmd[1], cmd[2], cmd[3], cmd[4], cmd[5], cmd[6]);
    }


    //printf("2) readRange result = %d\n", result);

    return result;
}
INT Spi::write(UINT           reg,
                    PVOID          data,
                    INT            len)
{
    INT         result          = -1;
    U8          cmd[len + 1];

    cmd[0] = reg | DIR_WRITE;
    memcpy(&cmd[1], data, len);

    //printf("1) readRange [0]= %X [0]= %X\n", cmd[0], cmd[1]);

    if ((result = transfer(cmd, cmd, sizeof(cmd))) > 0)
    {
        result--;

        //printf("GOT: %x %x %x %x %x %x\n", cmd[1], cmd[2], cmd[3], cmd[4], cmd[5], cmd[6]);
    }

    //printf("2) readRange result = %d\n", result);

    return result;
}

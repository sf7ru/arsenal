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


#include <stdio.h>
#include <I2c.h>

//#undef AXTRACE
//#define AXTRACE printf


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

//extern void myprintf(PCSTR msg, ...);

//#define printf myprintf

// ***************************************************************************
// FUNCTION
//      I2c::probeSlaves
// PURPOSE
//
// PARAMETERS
//      UINT ifaceNo --
// RESULT
//      void --
// ***************************************************************************
int  I2c::probeSlaves()
{
#define MAC_DEVICE(a,b) case a: printf("  %X - %s\n", a, b); break
    
    int     result      = 0;
    int     i;
    U8      ch[2];
    

    ENTER(true);

    printf("Probing available I2C devices on iface %u...\n\n", ifaceNo);
    printf("     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
    printf("====================================================\n");

    for (i = 0; i <= 0x7F; i++)
    {
        if (!(i & 0x0F))
        {
            printf("\n"); printf("%02X  ", (i >> 4) << 4);
        }

        if (i <= 7 || i > 0x78)
        {
            printf("   ");
            continue;
        }

        /* Address 0x48 points to LM75AIM device which needs 2 bytes be read */
        if (  (masterRead(i, (PVOID)ch, 1 + (i == 0x48)) > 0)   ||
              (masterWrite(i, (PVOID)ch, 0) > 0)                )
        {
            printf(" %02X", i);
            result++;
        }
        else
        {
            printf(" --");
        }
    }

    printf("\n\n");
    
    for (i = 0; i <= 0x7F; i++)
    {
        /* Address 0x48 points to LM75AIM device which needs 2 bytes be read */
        if (  (masterRead(i, (PVOID)ch, 1 + (i == 0x48)) > 0)   ||
              (masterWrite(i, (PVOID)ch, 0) > 0)                )
        {
            switch (i)
            {
                MAC_DEVICE(0x19, "BMI088 ACCL");
                MAC_DEVICE(0x1B, "DS2482");
                MAC_DEVICE(0x1D, "LSM303");
                MAC_DEVICE(0x1E, "FXOS8700CQ");
                MAC_DEVICE(0x20, "FXAS21002CQ");
                MAC_DEVICE(0x29, "BNO055");
                MAC_DEVICE(0x48, "LM75");
                MAC_DEVICE(0x50, "AT24");
                MAC_DEVICE(0x68, "DS1307/MPU6050/BMI088");
                MAC_DEVICE(0x69, "MPU6050/BMI088");
                
                default:
                    printf("  %X - ?\n", i);
                    break;
            }
        }
    }

    printf("\n");
    
    RETURN(result);
}
void I2c::lockDevice(I2cDevice *    dev)
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
void I2c::unlockDevice(I2cDevice *    dev)
{
    axmutex_lock(LCK, true);
    lockedDevice = nil;
    axmutex_unlock(LCK);
}

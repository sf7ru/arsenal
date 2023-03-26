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
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <SpiDevice.h>
#include <linux/spi/spidev.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdint.h>

#include <arsenal.h>
#include <fcntl.h>

#include <Spi.h>
#include <axstring.h>
#include <errno.h>

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
//      Spi::Spi
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
Spi::Spi()
{
    constructClass();
}
// ***************************************************************************
// TYPE
//      Destructor
// FUNCTION
//      Spi::~Spi
// PURPOSE
//      Class destruction
// PARAMETERS
//          --
// ***************************************************************************
Spi::~Spi()
{

}
// ***************************************************************************
// FUNCTION
//      Spi::deinit
// PURPOSE
//
// PARAMETERS
//      UINT ifaceNo --
// RESULT
//      void --
// ***************************************************************************
void Spi::deinit(UINT ifaceNo)
{
}
BOOL Spi::init(SpiDevice *    dev)
{
    BOOL            result          = false;
    unsigned long   tmp;
    CHAR            path            [ AXLPATH ];

    ENTER(true);

    if (!LCK)
    {
        if (initBase())
        {
            result = true;
//            dev->mIfaceNo = ifaceNo;
//            dev->control  = phspi;
        }
    }
    else
        result = true;

    if (result)
    {
        sprintf(path, "/dev/spidev%d.%d", ifaceNo, dev->ppCs.pin);

        //printf("SPI%d: file: %s\n", index, path);

        if (!((dev->ppCs.pin < SPI_MDEVS_NO) && (mDevFds[dev->ppCs.pin] != -1)))
        {
            if ((dev->mDevFd = open(path, O_RDWR)) != -1)
            {
                if (dev->ppCs.pin < SPI_MDEVS_NO)
                {
                    mDevFds[dev->ppCs.pin] = dev->mDevFd;
                }

                tmp = getModeNumber();

                // printf("SPI%d: mode: %d\n", index, tmp);

                if ( (ioctl(dev->mDevFd, SPI_IOC_RD_MODE, &tmp) != -1)  &&
                     (ioctl(dev->mDevFd, SPI_IOC_WR_MODE, &tmp) != -1)  )
                {
                    tmp = mode.firstMsb ? 0 : 1;
                    if (  (ioctl(dev->mDevFd, SPI_IOC_RD_LSB_FIRST, &tmp) != -1) &&
                          (ioctl(dev->mDevFd, SPI_IOC_WR_LSB_FIRST, &tmp) != -1) )
                    {
                        tmp = mode.bits;
                        if ( (ioctl(dev->mDevFd, SPI_IOC_RD_BITS_PER_WORD, &tmp) != -1) &&
                             (ioctl(dev->mDevFd, SPI_IOC_WR_BITS_PER_WORD, &tmp) != -1) )
                        {
                            tmp = mode.baudrateKHz * 1000;

                            if ( (ioctl(dev->mDevFd, SPI_IOC_RD_MAX_SPEED_HZ, &tmp) != -1) &&
                                 (ioctl(dev->mDevFd, SPI_IOC_WR_MAX_SPEED_HZ, &tmp) != -1) )
                            {
                                result = true;
                            }
                            else
                                printf("SPI%d: cant set speed\n", ifaceNo);
                        }
                        else
                            printf("SPI%d: cant set word length\n", ifaceNo);
                    }
                    else
                        printf("SPI%d: cant set bit flow\n", ifaceNo);
                }
                else
                    printf("SPI%d: cant set mode\n", ifaceNo);
            }
            else
                printf("SPI%d: cant open device '%s'\n", ifaceNo, path);
        }
        else
        {
            //printf("Used already opened FD\n");

            dev->mDevFd = mDevFds[dev->ppCs.pin];
        }
    }

    RETURN(result);
}

// for two messages
// struct spi_ioc_transfer xfer [ 2 ];
//    ioctl(fd, SPI_IOC_MESSAGE (2) , xfer ); //


// ***************************************************************************
// FUNCTION
//      Spi::transfer
// PURPOSE
//
// PARAMETERS
//      SpiDevice * dev  --
//      PU8         send --
//      PU8         recv --
//      int         len  --
// RESULT
//      int --
// ***************************************************************************
int Spi::transfer(PU8            send,
                  PU8            recv,
                  int            len,
                  UINT           TO)
{
    int                     result          = -1;
    int                     rd;
    struct spi_ioc_transfer xfer = { 0 };

    ENTER(true);

    xfer.tx_buf         = (unsigned long)send;
    xfer.rx_buf         = (unsigned long)recv;
    xfer.delay_usecs    = 10;
    xfer.speed_hz       = mode.baudrateKHz * 1000;
    xfer.bits_per_word  = mode.bits;
    xfer.len            = len;

    onTransAssertSsel();

    //printf("LinuxSpi.cpp:203 stage mDevFd = %d, len = %d\n", (int)lockedDevice->mDevFd, (int)len);

    rd = ioctl(lockedDevice->mDevFd, SPI_IOC_MESSAGE(1), &xfer);

    onTransDeassertSsel();

    if (rd != -1)
    {
        result = len;
    }
    else
        printf("SPI%d: transfer error = %d!\n", ifaceNo, errno);

    RETURN(result);
}

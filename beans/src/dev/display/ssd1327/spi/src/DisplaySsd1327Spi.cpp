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

#include <DisplaySsd1327Spi.h>
#include <Hal.h>
#include <axtime.h>
#include <string.h>
#include <customboard.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

DisplaySsd1327Spi::DisplaySsd1327Spi()
{
    ppDatCmd    = PP_UNDEF;
}
/*
void DisplaySsd1327Spi::onTransAssertSsel()
{
    Hal::getInstance().gpio.setPin(ppCs, 0);
}
void DisplaySsd1327Spi::onTransDeassertSsel()
{
    Hal::getInstance().gpio.setPin(ppCs, 1);
}
*/
BOOL DisplaySsd1327Spi::dispInit(int            ifaceNo,
                                 PORTPIN        ppDC,
                                 PORTPIN        ppCS,
                                 PORTPIN        ppRES)
{
    BOOL        result          = false;
    Hal &       hal             = Hal::getInstance();
    BOOL        bFlip           = false;
    U8          uc              [32];

    ENTER(true);

    this->ppDatCmd  = ppDC;
    this->ppCs      = ppCS;
    this->ppReset   = ppRES;

    hal.gpio.setMode(ppDatCmd);
    hal.gpio.setMode(ppCs);

    if (PP_IS_DEFINED(ppReset))
        hal.gpio.setMode(ppReset);

    onTransDeassertSsel();

    if (SpiDevice::init(ifaceNo))
    {
        reset();

        power(false);
        //if (oled_type == OLED_128x128)
        {
            uc[0] = 0x00; // command
            uc[1] = 0xa0; // GDDRAM mapping

            if (bFlip)
                uc[2] = 0x42;
            else
                uc[2] = 0x51; // default (top to bottom, left to right mapping)
            oledWrite(uc, 3);
        }

        power(true);

        result = true;
    }

    RETURN(result);
}
void DisplaySsd1327Spi::oledWrite(PU8            buff,
                                  UINT           size,
                                  BOOL           isData)
{
    if (isData)
    {
        Hal::getInstance().gpio.setPin(ppDatCmd, 1); // data versus command

        transmit(&buff[0], size);
    }
    else
    {
        Hal::getInstance().gpio.setPin(ppDatCmd, 0); // data versus command

        transmit(&buff[1], size - 1);
    }
}
void DisplaySsd1327Spi::writeDataBlock(PU8 ucBuf, int iLen)
{
    oledWrite(ucBuf, iLen, true);
}
/*
//
// Display part or whole of the backbuffer or a custom bitmap to the visible display
// Pass a NULL pointer to display the backbuffer
//
void DisplaySsd1327Spi::showBitmap(U8 *pBuffer, int iLocalPitch, int x, int y, int w, int h)
{
    int ty;
    U8 *s;

    if (x < 0 || y < 0 || x >= iMaxX || y >= iMaxY || (x+w) > iMaxX || (y+h) > iMaxY)
        return; // invalid coordinates
    if (pBuffer == NULL)
    {
        pBuffer = &ucBackbuffer[(y*wPitch)+(x/2)]; // starting point also
        iLocalPitch = wPitch;
    }
    setPosition(x, y, w, h);
    for (ty=0; ty<h; ty++)
    {
        s = &pBuffer[iLocalPitch * (y+ty)];
        writeDataBlock(&s[x/2], w/2);
    } // for y

}
*/

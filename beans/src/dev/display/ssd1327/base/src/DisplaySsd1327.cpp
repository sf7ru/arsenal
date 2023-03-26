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

#include <DisplaySsd1327.h>
#include <Hal.h>
#include <axtime.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

DisplaySsd1327::DisplaySsd1327()
{
    ppReset     = PP_UNDEF;
}
void DisplaySsd1327::reset()
{
    Hal & hal = Hal::getInstance();

    if (PP_IS_DEFINED(ppReset))
    {
        hal.gpio.setMode(ppReset, 1);
        axsleep(50);
        hal.gpio.setMode(ppReset, 0);
        axsleep(50);
        hal.gpio.setMode(ppReset, 1);
        axsleep(10);
    }
}
void DisplaySsd1327::ssd1327WriteCommand(U8 cmd)
{
// Send a single byte command to the OLED controller
    unsigned char buf[2];

    buf[0] = 0x00; // command introducer
    buf[1] = cmd;
    oledWrite(buf, 2);
}
void DisplaySsd1327::power(BOOL bOn)
{
    if (bOn)
        ssd1327WriteCommand(0xaf); // turn on OLED
    else
        ssd1327WriteCommand(0xae); // turn off OLED
} /* oledPower() */
void DisplaySsd1327::setPosition(int x, int y, int cx, int cy)
{
    unsigned char buf[8];

    buf[0] = 0x00; // command introducer
    buf[1] = 0x15; // column start/end
    buf[2] = x/2; // start address
    buf[3] = (U8)(((x+cx)/2)-1); // end address
    buf[4] = 0x75; // row start/end
    buf[5] = y; // start row
    buf[6] = y+cy-1; // end row
    oledWrite(buf, 7);
} /* ssd1327SetPosition() */
void DisplaySsd1327::applyBuffer(UINT           x,
                                 UINT           y,
                                 UINT           w,
                                 UINT           h)
{
    int ty;
    U8 *s;
    U8 *pBuffer;

    pBuffer = &mBuffer[(y*mPitch)+(x/2)]; // starting point also

    setPosition(x, y, w, h);
    for (ty=0; ty<h; ty++)
    {
        s = &pBuffer[mPitch * (y+ty)];
        writeDataBlock(&s[x/2], w/2);
    } // for y

} /* ssd1327ShowBitmap() */

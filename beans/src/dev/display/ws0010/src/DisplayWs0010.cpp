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

#define DBGTRACE


#include <axtime.h>
#include <DisplayWs0010.h>
#include <pvt_Ws0010.h>

#include <Hal.h>

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

#define LCD_CONVERT_TO_1251

#if defined(LCD_CONVERT_TO_1251)
static const unsigned char win1251 []=
{
    0x41, 0xa0, 0x42, 0xa1, 0xe0, 0x45, 0xa3, 0XA4, // АБВГДЕЖЗ
    0XA5, 0XA6, 0X4B, 0XA7, 0X4D, 0X48, 0X4F, 0XA8, // ИЙКЛМНОП
    0X50, 0X43, 0X54, 0XA9, 0XAA, 0X58, 0XE1, 0XAB, // РСТУФХЦЧ
    0XAC, 0XE2, 0XAD, 0XAE, 0X62, 0XAF, 0XB0, 0XB1, // ШЩЪЫЬЭЮЯ
    0X61, 0XB2, 0XB3, 0XB4, 0XE3, 0X65, 0XB6, 0xb7, // абвгдежз
    0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0x6f, 0xbe, // ийклмноп
    0x70, 0x63, 0xbf, 0x79, 0xe4, 0x78, 0xe5, 0xc0, // рстуфхцч
    0xc1, 0xe6, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7  //
} ;
#endif

#define axdelay(a)  _mydelay(a * 5000)
#define axudelay(a) _mydelay(a)

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#define LOOPS_1US (1000)

void _mydelay(int to)
{
    uint32_t cd = (to * LOOPS_1US) - 10;
    while (cd--);

}

// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      DisplayWs0010::DisplayWs0010
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
DisplayWs0010::DisplayWs0010()
{

}
// ***************************************************************************
// FUNCTION
//      DisplayWs0010::setData
// PURPOSE
//
// PARAMETERS
//      UINT value --
// RESULT
//      void --
// ***************************************************************************
void DisplayWs0010::setData(UINT value)
{
    Hal &           hal             = Hal::getInstance();

#ifdef CBRD_DISP_D5
    hal.gpio.setPin(CBRD_DISP_D4, (value >> 4) & 1);
    hal.gpio.setPin(CBRD_DISP_D5, (value >> 5) & 1);
    hal.gpio.setPin(CBRD_DISP_D6, (value >> 6) & 1);
    hal.gpio.setPin(CBRD_DISP_D7, (value >> 7) & 1);
#else
    hal.gpio.setRange(CBRD_DISP_D4_GPIO, CBRD_DISP_D4_PIN, 4,  value >> 4);
#endif
}
// ***************************************************************************
// FUNCTION
//      DisplayWs0010::getData
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      U8 --
// ***************************************************************************
U8 DisplayWs0010::getData()
{
    U8              result          = 0;
    Hal &           hal             = Hal::getInstance();

#ifdef CBRD_DISP_D5
    result |= (hal.gpio.getPin(CBRD_DISP_D4) & 1) << 4;
    result |= (hal.gpio.getPin(CBRD_DISP_D5) & 1) << 5;
    result |= (hal.gpio.getPin(CBRD_DISP_D6) & 1) << 6;
    result |= (hal.gpio.getPin(CBRD_DISP_D7) & 1) << 7;
#else
    result = (U8)hal.gpio.getRange(CBRD_DISP_D4_GPIO, CBRD_DISP_D4_PIN, 4) << 4;
#endif

    return result;
}
void DisplayWs0010::initBase()
{
    Hal &           hal             = Hal::getInstance();

    // initial write to lcd is 8bit

    setData(0);

    for (int i = 0; i < 5; i++)
    {
        toggleE(hal);
        axdelay(16);
    }

    setData(LCD_FUNCTION_4BIT_1LINE);

    // ??? i < 2
    for (int i = 0; i < 2; i++)
    {
        toggleE(hal);
        axdelay(16);
    }

    setData(0xA0); // Set mode

    toggleE(hal);
    axdelay(16);

    setGfxModeHalf(false);
}
void DisplayWs0010::selectHalf(int no)
{
#ifdef CBRD_DISP_CS1
    Hal &           hal             = Hal::getInstance();

    if (no)
    {
        hal.gpio.setPin(CBRD_DISP_CS1, 1);
        hal.gpio.setPin(CBRD_DISP_CS2, 0);
    }
    else
    {
        hal.gpio.setPin(CBRD_DISP_CS1, 0);
        hal.gpio.setPin(CBRD_DISP_CS2, 1);

    }

    //axsleep(10);

#endif
}
// ***************************************************************************
// FUNCTION
//      DisplayWs0010::init
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL DisplayWs0010::init()
{
    BOOL            result          = true;
    Hal &           hal             = Hal::getInstance();

    ENTER(true);

    hal.gpio.setMode(CBRD_DISP_RS);
    hal.gpio.setMode(CBRD_DISP_RW);
    hal.gpio.setMode(CBRD_DISP_E);
    hal.gpio.setMode(CBRD_DISP_D4);
    hal.gpio.setMode(CBRD_DISP_D5);
    hal.gpio.setMode(CBRD_DISP_D6);
    hal.gpio.setMode(CBRD_DISP_D7);

#ifdef CBRD_DISP_D0
    hal.gpio.setMode(CBRD_DISP_D0);
    hal.gpio.setMode(CBRD_DISP_D1);
    hal.gpio.setMode(CBRD_DISP_D2);
    hal.gpio.setMode(CBRD_DISP_D3);
#endif

#ifdef CBRD_DISP_CS1
    hal.gpio.setMode(CBRD_DISP_CS1);
    hal.gpio.setPin(CBRD_DISP_CS1, 1);
    hal.gpio.setMode(CBRD_DISP_CS2);
    hal.gpio.setPin(CBRD_DISP_CS2, 1);
#endif


    axdelay(300);        // wait 16ms or more after power-on

#ifdef CBRD_DISP_CS1

    selectHalf(0);

#endif

    initBase();

#ifdef CBRD_DISP_CS1

    selectHalf(1);

    initBase();

#endif


    RETURN(result);
}
void DisplayWs0010::setGfxModeHalf(BOOL enable)
{
    sendCommand(LCD_DISP_OFF);
    sendCommand(enable ? LCD_MODE_GFX : LCD_MODE_DEFAULT);
    clear();
    sendCommand(LCD_DISP_ON);
}

BOOL DisplayWs0010::setGfxMode(BOOL enable)
{
#ifdef CBRD_DISP_CS1

    selectHalf(0);
#endif

    setGfxModeHalf(enable);

#ifdef CBRD_DISP_CS1

    selectHalf(1);

    setGfxModeHalf(enable);

#endif

    return true;
}
// ***************************************************************************
// FUNCTION
//      DisplayWs0010::read
// PURPOSE
//
// PARAMETERS
//      U8 rs --
// RESULT
//      U8 --
// ***************************************************************************
U8 DisplayWs0010::read(U8 rs)
{
    U8              data;
    Hal &           hal = Hal::getInstance();

    turnRs(hal, rs); /* RS=1: read data
                        RS=0: read busy flag */

    turnRw(hal, 1);                           /* RW=1  read mode      */

    setDataOutput(hal, false);  /* configure data pins as input */

    turnE(hal, 1);
    delayE();

    /* read high nibble first */
    data = getData();

    turnE(hal, 0);
    delayE();                       /* Enable 500ns low       */

    turnE(hal, 1);
    delayE();

    /* read low nibble        */
    data |= (U8)((getData() >> 4) & 0x0F);
    turnE(hal, 0);

    return data;
}
// ***************************************************************************
// FUNCTION
//      DisplayWs0010::write
// PURPOSE
//
// PARAMETERS
//      U8 data --
//      U8 rs   --
// RESULT
//      void --
// ***************************************************************************
void DisplayWs0010::write(U8       data,
                           U8       rs)
{
    Hal &           hal = Hal::getInstance();
    //U8              dataBits;


    turnRs(hal, rs); /* write data        (RS=1, RW=0)
 =                 write instruction (RS=0, RW=0) */
    turnRw(hal, 0);

    setDataOutput(hal, true); /* configure data pins as output */


    /* output high nibble first */
//    hal.gpio.setRange(CBRD_DISP_D4_GPIO,
//                        CBRD_DISP_D4_PIN,
//                        4,
//                        (data >> 4) & 0x0F);
    setData(data);

    toggleE(hal);
    /* output low nibble */
//    hal.gpio.setRange(CBRD_DISP_D4_GPIO,
//                        CBRD_DISP_D4_PIN,
//                        4,
//                        data & 0x0F);
    setData(data << 4);

    toggleE(hal);

    /* all data pins high (inactive) */
//    hal.gpio.setRange(CBRD_DISP_D4_GPIO,
//                        CBRD_DISP_D4_PIN,
//                        4,
//                        0x0F);

    setData(0xFF);
}
// ***************************************************************************
// FUNCTION
//      DisplayWs0010::waitbusy
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      U8 --
// ***************************************************************************
U8 DisplayWs0010::waitbusy(void)
{

    register U8 c;
    register U16 countdown = 5000;

    /* wait until busy flag is cleared */
    while (((c = read(0)) & (1 << LCD_BUSY)) && countdown) { countdown--; }

    /* the address counter is updated 4us after the busy flag is cleared */
    axdelay(2);

    /* now read the address counter */
    return (read(0));  // return address counter
}
// ***************************************************************************
// FUNCTION
//      DisplayWs0010::sendCommand
// PURPOSE
//
// PARAMETERS
//      U8 cmd --
// RESULT
//      void --
// ***************************************************************************
void DisplayWs0010::sendCommand(U8 cmd)
{
    waitbusy();
    write(cmd, 0);
}
// ***************************************************************************
// FUNCTION
//      DisplayWs0010::sendData
// PURPOSE
//
// PARAMETERS
//      U8 data --
// RESULT
//      void --
// ***************************************************************************
void DisplayWs0010::sendData(U8 data)
{
    waitbusy();
    write(data, 1);
}
// ***************************************************************************
// FUNCTION
//      DisplayWs0010::gotoxy
// PURPOSE
//
// PARAMETERS
//      U8 x --
//      U8 y --
// RESULT
//      void --
// ***************************************************************************
void DisplayWs0010::gotoxy(U8 x, U8 y)
{
#if CBRD_DISP_HEIGHT==1
    sendCommand((1  << LCD_DDRAM) + LCD_START_LINE1 + x);
#endif
#if CBRD_DISP_HEIGHT==2
    if ( y == 0 )
        sendCommand((1 << LCD_DDRAM) + LCD_START_LINE1 + x);
    else
        sendCommand((1 << LCD_DDRAM) + LCD_START_LINE2 + x);
#endif
#if CBRD_DISP_HEIGHT==4
    if ( y==0 )
        sendCommand((1<<LCD_DDRAM)+LCD_START_LINE1+x);
    else if ( y==1)
        sendCommand((1<<LCD_DDRAM)+LCD_START_LINE2+x);
    else if ( y==2)
        sendCommand((1<<LCD_DDRAM)+LCD_START_LINE3+x);
    else /* y==3 */
        sendCommand((1<<LCD_DDRAM)+LCD_START_LINE4+x);
#endif

}
// ***************************************************************************
// FUNCTION
//      DisplayWs0010::delayE
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      void --
// ***************************************************************************
void DisplayWs0010::delayE()
{
    axudelay(100);
//    for (int i = 1; i < 10000;)
//    {
//        i++;
//    }
}
// ***************************************************************************
// FUNCTION
//      DisplayWs0010::turnRs
// PURPOSE
//
// PARAMETERS
//      BOOL val --
// RESULT
//      void --
// ***************************************************************************
void DisplayWs0010::turnRs(Hal &           hal,
                            BOOL            val)
{
    hal.gpio.setPin(CBRD_DISP_RS, val);
}
// ***************************************************************************
// FUNCTION
//      DisplayWs0010::turnRw
// PURPOSE
//
// PARAMETERS
//      BOOL val --
// RESULT
//      void --
// ***************************************************************************
void DisplayWs0010::turnRw(Hal &           hal,
                            BOOL            val)
{
    hal.gpio.setPin(CBRD_DISP_RW, val);
}
// ***************************************************************************
// FUNCTION
//      DisplayWs0010::turnE
// PURPOSE
//
// PARAMETERS
//      BOOL val --
// RESULT
//      void --
// ***************************************************************************
void DisplayWs0010::turnE(Hal &           hal,
                           BOOL            val)
{
    hal.gpio.setPin(CBRD_DISP_E, val);
}
// ***************************************************************************
// FUNCTION
//      DisplayWs0010::setDataOutput
// PURPOSE
//
// PARAMETERS
//      BOOL val --
// RESULT
//      void --
// ***************************************************************************
void DisplayWs0010::setDataOutput(Hal &           hal,
                                   BOOL            val)
{
    if (val)
    {
#ifdef CBRD_DISP_D5
        hal.gpio.changeDir(CBRD_DISP_D4, GpioDirOut);
        hal.gpio.changeDir(CBRD_DISP_D5, GpioDirOut);
        hal.gpio.changeDir(CBRD_DISP_D6, GpioDirOut);
        hal.gpio.changeDir(CBRD_DISP_D7, GpioDirOut);
#else
        hal.gpio.setAsOut(CBRD_DISP_D4_GPIO, CBRD_DISP_D4_PIN);
        hal.gpio.setAsOut(CBRD_DISP_D4_GPIO, hal.gpio.incPin(CBRD_DISP_D4_PIN, 1));
        hal.gpio.setAsOut(CBRD_DISP_D4_GPIO, hal.gpio.incPin(CBRD_DISP_D4_PIN, 2));
        hal.gpio.setAsOut(CBRD_DISP_D4_GPIO, hal.gpio.incPin(CBRD_DISP_D4_PIN, 3));
#endif
    }
    else
    {
#ifdef CBRD_DISP_D5
        hal.gpio.changeDir(CBRD_DISP_D4, GpioDirIn);
        hal.gpio.changeDir(CBRD_DISP_D5, GpioDirIn);
        hal.gpio.changeDir(CBRD_DISP_D6, GpioDirIn);
        hal.gpio.changeDir(CBRD_DISP_D7, GpioDirIn);
#else
        hal.gpio.setAsIn(CBRD_DISP_D4_GPIO, CBRD_DISP_D4_PIN);
        hal.gpio.setAsIn(CBRD_DISP_D4_GPIO, hal.gpio.incPin(CBRD_DISP_D4_PIN, 1));
        hal.gpio.setAsIn(CBRD_DISP_D4_GPIO, hal.gpio.incPin(CBRD_DISP_D4_PIN, 2));
        hal.gpio.setAsIn(CBRD_DISP_D4_GPIO, hal.gpio.incPin(CBRD_DISP_D4_PIN, 3));
#endif
    }
}
// ***************************************************************************
// FUNCTION
//      DisplayWs0010::toggleE
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      void --
// ***************************************************************************
void DisplayWs0010::toggleE(Hal &           hal)
{
    turnE(hal, 1);
    delayE();
    turnE(hal, 0);
}
// ***************************************************************************
// FUNCTION
//      DisplayWs0010::clear
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      void --
// ***************************************************************************
void DisplayWs0010::clear(void)
{
    sendCommand(1 << LCD_CLR);
}
// ***************************************************************************
// FUNCTION
//      DisplayWs0010::home
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      void --
// ***************************************************************************
void DisplayWs0010::home(void)
{
    sendCommand(1<<LCD_HOME);
}
// ***************************************************************************
// FUNCTION
//      DisplayWs0010::putc
// PURPOSE
//
// PARAMETERS
//      char c --
// RESULT
//      void --
// ***************************************************************************z
#undef putc
void DisplayWs0010::putc(unsigned char c)
{
    U8      pos;

    pos = waitbusy();   // read busy-flag and address counter

    if (c == '\n')
    {
        newline(pos);
    }
    else
    {
#if CBRD_DISP_HEIGHT==1
        if ( pos == LCD_START_LINE1+CBRD_DISP_WIDTH ) {
            HD44780_write((1<<LCD_DDRAM)+LCD_START_LINE1,0);
        }
#elif CBRD_DISP_HEIGHT==2
        if ( pos == LCD_START_LINE1 + CBRD_DISP_WIDTH ) {
            write((1<<LCD_DDRAM)+LCD_START_LINE2,0);
        }else if ( pos == LCD_START_LINE2 + CBRD_DISP_WIDTH ){
            write((1<<LCD_DDRAM)+LCD_START_LINE1,0);
        }
#elif CBRD_DISP_HEIGHT==4
//        if ( pos == LCD_START_LINE1+CBRD_DISP_WIDTH ) {
//            write((1<<LCD_DDRAM)+LCD_START_LINE2,0);
//        }else if ( pos == LCD_START_LINE2+CBRD_DISP_WIDTH ) {
//            write((1<<LCD_DDRAM)+LCD_START_LINE3,0);
//        }else if ( pos == LCD_START_LINE3+CBRD_DISP_WIDTH ) {
//            write((1<<LCD_DDRAM)+LCD_START_LINE4,0);
//        }else if ( pos == LCD_START_LINE4+CBRD_DISP_WIDTH ) {
//            write((1<<LCD_DDRAM)+LCD_START_LINE1,0);
//        }
#endif
//        waitbusy();

#if defined(LCD_CONVERT_TO_1251)
        switch (c)
        {
            case 0xA8:
                c = 0xa2;
                break;

            case 0xB8:
                c = 0xB5;
                break;

            case 0x08:
                c = 0xFF;
                break;

            default:
                if (c >= 0xc0)
                {
                    c  = win1251[c - 0xc0];
                }
                break;
        }
#endif
        write(c, 1);
    }

}
// ***************************************************************************
// FUNCTION
//      DisplayWs0010::newline
// PURPOSE
//
// PARAMETERS
//      U8 pos --
// RESULT
//      void --
// ***************************************************************************
void DisplayWs0010::newline(U8 pos)
{
    register U8 addressCounter;

#if CBRD_DISP_HEIGHT==1
    addressCounter = 0;
#endif
#if CBRD_DISP_HEIGHT==2
    if ( pos < (LCD_START_LINE2) )
        addressCounter = LCD_START_LINE2;
    else
        addressCounter = LCD_START_LINE1;
#endif
#if CBRD_DISP_HEIGHT==4
#if KS0073_4LINES_MODE
    if ( pos < LCD_START_LINE2 )
        addressCounter = LCD_START_LINE2;
    else if ( (pos >= LCD_START_LINE2) && (pos < LCD_START_LINE3) )
        addressCounter = LCD_START_LINE3;
    else if ( (pos >= LCD_START_LINE3) && (pos < LCD_START_LINE4) )
        addressCounter = LCD_START_LINE4;
    else
        addressCounter = LCD_START_LINE1;
#else
    if ( pos < LCD_START_LINE3 )
        addressCounter = LCD_START_LINE2;
    else if ( (pos >= LCD_START_LINE2) && (pos < LCD_START_LINE4) )
        addressCounter = LCD_START_LINE3;
    else if ( (pos >= LCD_START_LINE3) && (pos < LCD_START_LINE2) )
        addressCounter = LCD_START_LINE4;
    else
        addressCounter = LCD_START_LINE1;
#endif
#endif
    sendCommand((1 << LCD_DDRAM)+addressCounter);
}
// ***************************************************************************
// FUNCTION
//      DisplayWs0010::setCharsetWin1251
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      void --
// ***************************************************************************
void DisplayWs0010::setCharsetWin1251(void)
{
    sendCommand(0x2A);
    axdelay(64);
}
// ***************************************************************************
// FUNCTION
//      DisplayWs0010::puts
// PURPOSE
//
// PARAMETERS
//      PCSTR string --
// RESULT
//      void --
// ***************************************************************************
void DisplayWs0010::puts(PCSTR          string)
{
    PCSTR   on  = string;

    while (*on)
    {
        putc(*on);
        on++;
    }
}
// ***************************************************************************
// FUNCTION
//      DisplayWs0010::loadUserFont
// PURPOSE
//
// PARAMETERS
//      PU8  font   --
//      UINT number --
//      UINT height --
// RESULT
//      void --
// ***************************************************************************
void DisplayWs0010::loadUserFont(PCU8           font,
                                  UINT           number,
                                  UINT           height)
{
    UINT                symbol;
    UINT                y;
    PCU8                on;

    waitbusy();

    for (symbol = 0, on = font; symbol < number; symbol++)
    {
        sendCommand((1<<LCD_CGRAM) | (8 * symbol)) ;

        for (y = 0; y < height; y++, on++)
        {
            sendData(*on);
            axdelay(16);
        }
    }
}
PU8 DisplayWs0010::applyHalf(PU8            on)
{
    U8      pos;

    for (int x = 0; x < 200; x++)
    {
        pos = waitbusy();   // read busy-flag and address counter
        write(*on, 1);
        on++;
    }

    return on;
}
void DisplayWs0010::applyBitmap(PU8            bitmap)
{
    PU8     on;

#ifdef CBRD_DISP_CS1

    selectHalf(0);

#endif

    write((1<<LCD_DDRAM)+ 0, 0);
    write((1<<LCD_CGRAM)+ 0, 0);

    on = applyHalf(bitmap);


#ifdef CBRD_DISP_CS1

    selectHalf(1);

    write((1<<LCD_DDRAM)+ 0, 0);
    write((1<<LCD_CGRAM)+ 0, 0);

    applyHalf(on);

#endif
}

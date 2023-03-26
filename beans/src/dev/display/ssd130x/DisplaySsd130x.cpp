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


#include <DisplaySsd130x.h>
#include <customboard.h>
#include <string.h>


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      DisplaySsd130x::DisplaySsd130x
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
DisplaySsd130x::DisplaySsd130x()
{
    hal     = &Hal::getInstance();

    mWidth  = CBRD_DISP_WIDTH;
    mHeight = CBRD_DISP_HEIGHT;
}
void DisplaySsd130x::setDisplayClockDivider(U8 DCLKdiv, U8 Fosc)
{   
    U8 b[] = { SSD130X_SETDISPLAYCLOCKDIV, ((--DCLKdiv & 0x0F) | ((Fosc & 0x0F) << 4)) };
    command(&b[0], sizeof(b));
}

void DisplaySsd130x::setMultiplexRatio(U8 Mux)
{   
    U8 b[] = { SSD130X_SETMULTIPLEX, (--Mux & 0x3F)  };
    command(&b[0], sizeof(b));
}
void DisplaySsd130x::setDisplayOffset(U8 Offset)
{
    U8 b[] = { SSD130X_SETDISPLAYOFFSET, Offset };
    command(&b[0], sizeof(b));
}

void DisplaySsd130x::setDisplayStartLine(U8 Line)
{
    command(SSD130X_SETSTARTLINE | (Line & 0x3F));
}
void DisplaySsd130x::chargePumpSetting(U8 Value)
{   
    U8 b[] = { SSD130X_ChargePumpSet, Value ? 0x14 : 0x10 };
    command(&b[0], sizeof(b));
}
void DisplaySsd130x::setMemAdressingMode(U8 Mode)
{ 
    U8 b[] = { SSD130X_MEMORYMODE, Mode };
    command(&b[0], sizeof(b));
}
void DisplaySsd130x::setSegmentRemap(U8 Value)
{
    Value = Value ? 1 : 0;
    command(SSD130X_SEGREMAP | Value);
}
void DisplaySsd130x::setCOMoutScanDirection(U8 Value)
{
    Value = Value ? 0x08 : 0x00;
    command(SSD130X_COMSCANINC | Value);
}
void DisplaySsd130x::setCOMPinsConfig(U8 AltCOMpinConfig, U8 LeftRightRemap)
{
    U8 tmpValue = (1 << 1);
    
    if (AltCOMpinConfig)
        tmpValue |= (1 << 4);
  
    if (LeftRightRemap)
        tmpValue |= (1 << 5);

    U8 b[] = { SSD130X_SETCOMPINS, tmpValue };
    command(&b[0], sizeof(b));
}
void DisplaySsd130x::setContrast(U8 Value)
{
    U8 b[] = { SSD130X_SETCONTRAST, Value };
    command(&b[0], sizeof(b));
}
void DisplaySsd130x::setPrechargePeriod(U8 Phase1period, U8 Phase2period)
{
    Phase1period &= 0x0F;
    Phase1period &= 0x0F;
    
    if (!Phase1period)
        Phase1period = 2;
    
    if (!Phase2period)
        Phase2period = 2;
    
    Phase1period |= (Phase2period << 4);
  
    U8 b[] = {SSD130X_SETPRECHARGE, Phase1period };
    command(&b[0], sizeof(b));
}
void DisplaySsd130x::setVCOMHDeselectLevel(U8 Code)
{
    U8 b[] = {SSD130X_SETVCOMLEVEL, Code & 0x70 };
    command(&b[0], sizeof(b));
}
void DisplaySsd130x::allPixRAM(void)
{
  command(SSD130X_DISPLAYALLON_RESUME);
}
void DisplaySsd130x::setInverseOn(void)
{
  command(SSD130X_INVERTDISPLAY);
}
void DisplaySsd130x::setInverseOff(void)
{
  command(SSD130X_NORMALDISPLAY);
}
void DisplaySsd130x::deactivateScroll(void)
{
  command(SSD130X_DeactivateScroll);
}
// ***************************************************************************
// FUNCTION
//      DisplaySsd130x::init
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL DisplaySsd130x::init()
{
    BOOL            result          = false;

    ENTER(true);

    hal->gpio.setMode(ppReset);
    hal->gpio.setMode(ppDatCmd);

    hal->gpio.setPin(ppReset, 1);
    axdelay(1);

    reset();

    command(SSD130X_DISPLAYOFF);                    // 0xAE
    command(SSD130X_DISPLAYOFF);                    // 0xAE

    setDisplayClockDivider(1, 8);
    setMultiplexRatio(mHeight);
    setDisplayOffset(0);
    setDisplayStartLine(0);
    chargePumpSetting(1);
    setMemAdressingMode(SSD130X_Adressing_Vertical);
    setSegmentRemap(0);           // *меняет направление заполнение матрицы из буфера кадра (вертикаль/горизонталь)
    setCOMoutScanDirection(0);    // *переворачивает оторбражение на матрице (только по вертикали)
    
    if ((mWidth == 128) && (mHeight == 32))
        setCOMPinsConfig(0, 0);
    else  if ((mWidth == 128) && (mHeight == 64))
        setCOMPinsConfig(1, 0);
    else  if ((mWidth == 96) && (mHeight == 16))
        setCOMPinsConfig(0, 0);
    
    setContrast(127);
    setPrechargePeriod(2, 2);
    setVCOMHDeselectLevel(0x40);
    allPixRAM();
    setInverseOff();
    deactivateScroll();

    command(SSD130X_DISPLAYON);//--turn on oled panel

    result = bufferInit();

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      DisplaySsd130x::reset
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      void --
// ***************************************************************************
void DisplaySsd130x::reset()
{
    ENTER(true);

    hal->gpio.setPin(ppReset, 0);
    axdelay(10);
    hal->gpio.setPin(ppReset, 1);
    axdelay(10);

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      DisplaySsd130x::setCommand
// PURPOSE
//
// PARAMETERS
//      BOOL value --
// RESULT
//      void --
// ***************************************************************************
void DisplaySsd130x::setModeCommand(BOOL value)
{
    hal->gpio.setPin(ppDatCmd, !value);
}
void DisplaySsd130x::command(U8             cmd)
{
    ENTER(true);

    lockDevice();
    setModeCommand(true);
    write(&cmd, 1);
    unlockDevice();

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      DisplaySsd130x::command
// PURPOSE
//
// PARAMETERS
//      U8 cmd --
// RESULT
//      void --
// ***************************************************************************
void DisplaySsd130x::command(PU8            cmd,
                             UINT           sz)
{
    ENTER(true);

    lockDevice();
    setModeCommand(true);
    write(cmd, sz);
    unlockDevice();

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      DisplaySsd130x::sendData
// PURPOSE
//
// PARAMETERS
//      PU8  data --
//      UINT size --
// RESULT
//      void --
// ***************************************************************************
void DisplaySsd130x::sendData(PU8            data,
                              UINT           size)
{
    ENTER(true);

    lockDevice();
    setModeCommand(false);
    write(data, size);
    unlockDevice(); 

    QUIT;
}
void DisplaySsd130x::setColumns(U8 Start, U8 End)
{
  Start &= 0x7F;
  End &= 0x7F;
  U8 b[] = {SSD130X_SETCOLADDR, Start, End};
  command(&b[0], sizeof(b));
}

void DisplaySsd130x::setPages(U8 Start, U8 End)
{
  Start &= 0x07;
  End &= 0x07;
  U8 b[] = { SSD130X_SETPAGEADDR, Start, End};
  command(&b[0], sizeof(b));
}

void DisplaySsd130x::applyBuffer(UINT           x,
                                 UINT           y,
                                 UINT           w,
                                 UINT           h)
{
    lockDevice();
    longSelect(true);

    setColumns(0, mWidth - 1);
    setPages(0, (mHeight >> 3) - 1);
    sendData(mBuffer,  sizeof(mBuffer));

    longSelect(false);
    unlockDevice();
}

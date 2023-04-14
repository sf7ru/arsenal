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

#include<Hal.h>
#include<axtime.h>
#include <axthreads.h>

#include <stdio.h>
#include <stdlib.h>

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
//      Hal::Hal
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
Hal::Hal()
{
    powerState  = 0;
}
void Hal::init()
{

}
Hal::~Hal()
{

}
void Hal::Gpio::setMode(PORTPIN pp,
                        UINT    flags)
{

}
void Hal::Gpio::changeDir(PORTPIN        pp,
                          GpioDir        dir,
                          UINT           flags)
{

}
BOOL Hal::Gpio::setPinFunction(UINT           port,
                               UINT           pin,
                               UINT           fn)
{
    return false;
}

void Hal::Gpio::setPin(PORTPIN      pp,
                       UINT         value)
{

}
UINT Hal::Gpio::getPin(PORTPIN pp)
{
    return 0;
}

void mainLoop()
{
     while (true)
     {
        axsleep(100);
     }
}

void retarget(AXDevice * dev)
{


}
// ***************************************************************************
// FUNCTION
//      Hal::Sys::prepareForUpdate
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL Hal::Sys::prepareForUpdate()
{
    BOOL            result          = false;

    ENTER(true);

    result = true;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      Hal::Sys::reboot
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      void --
// ***************************************************************************
void Hal::Sys::reboot()
{
    ENTER(true);

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      Hal::Sys::writeProgram
// PURPOSE
//
// PARAMETERS
//      PU8  program --
//      PU8  update  --
//      UINT size    --
// RESULT
//      void --
// ***************************************************************************
int Hal::Sys::writeProgram(PU8            program,
                           PU8            update,
                           UINT           size)
{
    return -1;
}

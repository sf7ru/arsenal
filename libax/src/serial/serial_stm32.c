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

#include <arsenal.h>
#include <serial.h>
#include <axtime.h>

// ***************************************************************************
// FUNCTION
//      serial_set_dtr
// PURPOSE
//
// PARAMETERS       
//      PAXDEV dev   --
//      BOOL   value --
// RESULT
//      void --
// ***************************************************************************
void serial_set_dtr(PAXDEV             dev,
                    BOOL               value)
{

}
// ***************************************************************************
// FUNCTION
//      serial_set_rts
// PURPOSE
//
// PARAMETERS
//      PAXDEV dev   --
//      BOOL   value --
// RESULT
//      void --
// ***************************************************************************
void serial_set_rts(PAXDEV             dev,
                    BOOL               value)
{

}

// ***************************************************************************
// FUNCTION
//      serial_set_baudrate
// PURPOSE
//
// PARAMETERS
//      PAXDEV dev      --
//      UINT   baudRate --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL serial_set_baudrate(PAXDEV        dev,
                         UINT          baudRate)
{
//    LPC_USART_T *   reqBase         = REQBASE(dev);

    return true; // _serial_set_baudrate(reqBase, baudRate);
}

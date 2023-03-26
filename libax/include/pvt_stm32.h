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

#ifndef __PVT_STM32_H__
#define __PVT_STM32_H__

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define SERIAL_DEVS_MAX     6


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif                                      //  #ifdef __cplusplus


PAXDEV              _serial_close               (PAXDEV             dev);

PVOID               _serial_open                (PSERIALPROPS       p,
                                                 UINT               type,
                                                 PUINT              irqn);

INT                 __serial_write              (PVOID              internal,
                                                 PCVOID             data,
                                                 INT                size,
                                                 UINT               TO);

INT                 __serial_read               (PVOID              internal,
                                                 PCVOID             data,
                                                 INT                size,
                                                 UINT               TO);

#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus

#endif // __PVT_STM32_H__

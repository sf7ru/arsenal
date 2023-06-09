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

#ifndef __RTCDEVICE_H__
#define __RTCDEVICE_H__

#include <arsenal.h>
#include <axtime.h>
#include <ExtMemory.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------


// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------


class RtcDevice: public ExtMemory
{
public:
                        RtcDevice               () {};

virtual                 ~RtcDevice              () {};

virtual BOOL            rtcInit                 (UINT           addr,
                                                 UINT           i2cIfaceNo = 0) = 0;

virtual BOOL            getDate                 (PAXDATE        date)   = 0;

virtual BOOL            setDate                 (PAXDATE        date)   = 0;

virtual PCSTR           getName                 ()  = 0;


};

#endif // __RTCDEVICE_H__

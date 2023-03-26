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

#pragma once

#include <arsenal.h>
#include <axtime.h>
#include <ExtMemory.h>
#include <Rtc.h>

#include <singleton_template.hpp>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------


// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------


class IntRtc: public RtcDevice, public SingletonTemplate<IntRtc>
{
public:
                        IntRtc                  ();

virtual                 ~IntRtc                 ();

        BOOL            rtcInit                 (UINT           addr,
                                                 UINT           i2cIfaceNo = 0);

        BOOL            getDate                 (PAXDATE        date);

        BOOL            setDate                 (PAXDATE        date);

        INT             read                    (PVOID          buff,
                                                 UINT           offset,
                                                 INT            size);

        INT             write                   (PVOID          buff,
                                                 UINT           offset,
                                                 INT            size);

        INT             getSize                 ();

        PCSTR           getName                 ();

};

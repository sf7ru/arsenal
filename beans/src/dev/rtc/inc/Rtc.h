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

#ifndef __RTC_H__
#define __RTC_H__

#include <arsenal.h>
#include <axtime.h>
#include <ExtMemory.h>

#include <SingletonDevice.hpp>

#include <RtcDevice.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#ifndef RTC_MAX_DEVICES
#define RTC_MAX_DEVICES     2
#endif // RTC_MAX_DEVICES

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class Rtc: public SingletonDevice<Rtc, RtcDevice>
{
public:
                        Rtc                     ()
                        {  }

                        ~Rtc                    ()
                        {  }

        BOOL            getDate                 (PAXDATE        date)
        { return device->getDate(date); }

        BOOL            setDate                 (PAXDATE        date)
        { return device->setDate(date); }

};

#endif // __RTC_H__

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

#ifndef __ARSENAL_HAL_H__
#define __ARSENAL_HAL_H__

#include <arsenal.h>
#include <HalPlatformDefs.h>
#include <singleton_template.hpp>
#include <PortPin.h>
#include <AXDevice.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define GET_INSTANCE(a)         a::getInstance()

#define SETVECTOR_EDGE_RISE     1
#define SETVECTOR_EDGE_FALL     2

#define HAL_SETMODE_FLAG_FAST   2
#define HAL_SETMODE_FLAG_HIGH   6

#define GPIO                    Hal::getInstance().gpio

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef void (*PFNTIMERCB)(PVOID param);
typedef void (*PFNGPIOINTCB)(PVOID param,UINT port,UINT pin,UINT edge);




// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------


class BaseGpio
{
public:
virtual void            setPin                  (PORTPIN        pp,
                                                 UINT           value) = 0;

virtual UINT            getPin                  (PORTPIN        pp) = 0;

virtual void            setMode                 (PORTPIN        pp,
                                                 UINT           flags       = 0)  = 0;

virtual void            changeDir               (PORTPIN        pp,
                                                 GpioDir        dir,
                                                 UINT           flags       = 0)  = 0;

virtual BOOL            setPinFunction          (UINT           port,
                                                 UINT           pin,
                                                 UINT           fn) = 0;
};

class Hal: public SingletonTemplate<Hal>
{
protected:

public:
        U16             powerState;

                        Hal                    ();
                        ~Hal                   ();

        void            init                    ();

    class Sys
    {
    public:
            BOOL            prepareForUpdate        ();

            void            unprepareForUpdate      ();

            int             writeProgram            (PU8            program,
                                                     PU8            update,
                                                     UINT           size);

            void            reboot                  ();

            void            callIsp                 ();

    } sys;

    class Gpio: public BaseGpio
    {
    public:
        void            setPin                  (PORTPIN        pp,
                                                 UINT           value);

        UINT            getPin                  (PORTPIN        pp);

        void            setMode                 (PORTPIN        pp,
                                                 UINT           flags       = 0);

        void            changeDir               (PORTPIN        pp,
                                                 GpioDir        dir,
                                                 UINT           flags       = 0);

        BOOL            setPinFunction          (UINT           port,
                                                 UINT           pin,
                                                 UINT           fn);

        void            setRange                (PORTPIN        pp,
                                                 UINT           length,
                                                 UINT           value);

        UINT            getRange                (PORTPIN        pp,
                                                 UINT           length);

    } gpio;

    class Bootld
    {
    public:
        void            activate                (void);
    } bootld;

    class GpioInt
    {
    public:
        HAXHANDLE       setVector               (PORTPIN        pp,
                                                 PFNGPIOINTCB   cb,
                                                 PVOID          param,
                                                 UINT           edges = (SETVECTOR_EDGE_RISE | SETVECTOR_EDGE_FALL));

        HAXHANDLE       unsetVector             (HAXHANDLE      handler);

    } gpioint;

    class Timer
    {
    public:
        HAXHANDLE       set                     (UINT           periodUs,
                                                 PFNTIMERCB     callback,
                                                 PVOID          param,
                                                 INT            priority = -1);

        HAXHANDLE       unset                   (HAXHANDLE      timer);
    } timer;

};


void                retarget                    (AXDevice *     dev);

void                mainLoop                    ();


#endif // #ifndef __HAL_H__


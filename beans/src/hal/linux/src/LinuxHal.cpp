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
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <poll.h>

/*
   Prior calls assumed.
   sudo sh -c "echo 4      >/sys/class/gpio/export"
   sudo sh -c "echo in     >/sys/class/gpio/gpio4/direction"
   sudo sh -c "echo rising >/sys/class/gpio/gpio4/edge"
*/


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct __tag_INTSTRUCT
{
    HAXTHREADX              hThread;
    PORTPIN                 pp;
    int                     fd;
    struct timeval          tv;
    struct pollfd           pfd;
    PFNGPIOINTCB            cb;
    PVOID                   param;
    UINT                    edges;
} INTSTRUCT, * PINTSTRUCT;

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
static INT __thread_IRQHandler(HAXTHREADX     h_ctrl,
                               PINTSTRUCT     s)
{
    AXSIG               v_sig;
    struct pollfd       pfd;
    int                 rd;
    UINT                edge = 0;
    char                buf [8];

    TX_ENTER(h_ctrl, s)

    while (!MAC_THREADX_WAIT_QUIT(v_sig, u_param, 0))
    {
        pfd.fd = s->fd;
        pfd.events = POLLPRI;

        lseek(s->fd, 0, SEEK_SET);    /* consume any prior interrupt */
        read(s->fd, buf, sizeof buf);

        switch (poll(&pfd, 1, 100))
        {
            case 0:
            case -1:
                break;

            default:
                lseek(s->fd, 0, SEEK_SET);    /* consume interrupt */

                if ((rd = read(s->fd, buf, sizeof buf)) > 0)
                {
                    switch (buf[0])
                    {
                        case    '0':    edge = SETVECTOR_EDGE_FALL; break;
                        case    '1':    edge = SETVECTOR_EDGE_RISE; break;
                        default:        edge = 0;                   break;
                    }
                }

                (*s->cb)(s->param, s->pp.port, s->pp.pin, edge);
                break;
        }
    }

    TX_EXIT(h_ctrl, 0);
}
HAXHANDLE Hal::GpioInt::unsetVector(HAXHANDLE   handle)
{
    HAXHANDLE           result          = nil;
    PINTSTRUCT          s               = (PINTSTRUCT)handle;

    ENTER(true);

    if (s)
    {
        if (s->hThread)
        {
            axthreadX_destroy(s->hThread, -1);
        }

        if (s->fd != -1)
        {
            close(s->fd);
        }

        FREE(s);
    }

    RETURN(result);
}

HAXHANDLE Hal::GpioInt::setVector(PORTPIN        pp,
                                  PFNGPIOINTCB   cb,
                                  PVOID          param,
                                  UINT           edges)
{
    PINTSTRUCT      result      = nil;
    char            fname       [ AXLPATH ];

    if ((result = CREATE(INTSTRUCT)) != nil)
    {
        result->fd      = -1;
        result->pp      = pp;
        result->cb      = cb;
        result->param   = param;
        result->edges   = edges;

        if ((result->hThread = axthreadX_create(0, (PFNAXTHREADX)__thread_IRQHandler, result, 10000)) != nil)
        {
            snprintf(fname, AXLPATH, "/sys/class/gpio/gpio%d/value", (pp.port * 32) + pp.pin);

            if ((result->fd = open(fname, O_RDONLY)) < 0)
            {
                printf("LinuxHal.cpp(158): stage 3\n");
                result = (PINTSTRUCT)unsetVector((HAXHANDLE) result);
            }
        }
        else
            result = (PINTSTRUCT)unsetVector((HAXHANDLE)result);
    }

    if (!result)
    {
        printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ Hal::GpioInt::setVector FAILED! (%d %d)\n", pp.port, pp.pin);
    }

    return result;
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

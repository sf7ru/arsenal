// ***************************************************************************
// TITLE
//      Serial Port Communications for Unix platform
// PROJECT
//
// ***************************************************************************
//
// FILE
//      $Id$
// HISTORY
//      $Log$
// ***************************************************************************

#include <stdio.h>
#include <AXSerial.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      AXSerial::AXSerial
// PURPOSE
//      Class construction
// PARAMETERS
//        void -- None
// ***************************************************************************
AXSerial::AXSerial(void)
{
    dev = NULL;
}
// ***************************************************************************
// TYPE
//      Destructor
// FUNCTION
//      AXSerial::~AXSerial
// PURPOSE
//      Class destruction
// PARAMETERS
//        void -- None
// ***************************************************************************
AXSerial::~AXSerial(void)
{
    close();
}
// ***************************************************************************
// FUNCTION
//      AXSerial::close
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXSerial::close(void)
{
    if (dev)
    {
        dev = (PAXDEV)serial_close(dev);
    }

    return dev == NULL ? true : false;
}
// ***************************************************************************
// FUNCTION
//      AXSerial::isOpen
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXSerial::isOpen(void)
{
    return dev == NULL ? false : true;
}
// ***************************************************************************
// FUNCTION
//      AXSerial::open
// PURPOSE
//
// PARAMETERS
//      PSERIALPROPS props --
//      UINT         type  --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXSerial::open(PSERIALPROPS  props,
                    UINT          type)
{
    return ((dev = serial_open(props, type)) != NULL) ? true : false;
}
// ***************************************************************************
// FUNCTION
//      AXSerial::open
// PURPOSE
//
// PARAMETERS
//      PSERIALPROPS props --
//      UINT         type  --
//      PCSTR        name  --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXSerial::open(PSERIALPROPS   props,
                    UINT           type,
                    PCSTR          name)
{
    return ((dev = serial_open_x(props, type, name)) != NULL) ? true : false;
}
// ***************************************************************************
// FUNCTION
//      AXSerial::read
// PURPOSE
//
// PARAMETERS
//      PVOID data --
//      INT   size --
//      UINT  TO   --
// RESULT
//      INT --
// ***************************************************************************
INT AXSerial::read(PVOID            data,
                   INT              size,
                   UINT             TO)
{
    return isOpen() ? _serial_read(dev, data, size, TO) : -1;
}
// ***************************************************************************
// FUNCTION
//      AXSerial::write
// PURPOSE
//
// PARAMETERS
//      PCVOID data --
//      INT    size --
//      UINT   TO   --
// RESULT
//      INT --
// ***************************************************************************
INT AXSerial::write(PCVOID          data,
                    INT             size,
                    UINT            TO)
{
    return isOpen() ? _serial_write(dev, data, size, TO) : -1;
}
// ***************************************************************************
// FUNCTION
//      AXSerial::setBaudrate
// PURPOSE
//
// PARAMETERS
//      UINT baudrate --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXSerial::setBaudrate(UINT baudrate)
{
    return serial_set_baudrate(dev, baudrate);
}
// ***************************************************************************
// FUNCTION
//      AXSerial::setDtr
// PURPOSE
//
// PARAMETERS
//      BOOLEAN value --
// RESULT
//      void --
// ***************************************************************************
void AXSerial::setDtr(BOOLEAN        value)
{
    serial_set_dtr(dev, value);
}
// ***************************************************************************
// FUNCTION
//      AXSerial::setRts
// PURPOSE
//
// PARAMETERS
//      BOOLEAN value --
// RESULT
//      void --
// ***************************************************************************
void AXSerial::setRts(BOOLEAN        value)
{
    serial_set_rts(dev, value);
}

INT AXSerial::getPendingTx()
{
    return 0;
}

/*
// ***************************************************************************
// TITLE
//      Serial Port Communications for UNIX platform
// PROJECT
//
// ***************************************************************************
//
// FILE
//      $Id$
// HISTORY
//      $Log$
// ***************************************************************************

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/termios.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <arsenal.h>

#include <a7system.h>

#include <pvt_a7.h>

#include <AXSerial.h>

// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      refineBaudrate
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      UINT   baudrate --
// RESULT
//      speed_t --
// ***************************************************************************
static speed_t refineBaudrate(UINT baudrate)
{
#define MAC_BAUDRATE(a)     case a: speed = B##a##00; break;

    speed_t         speed       = 0;

    switch (baudrate)
    {
        MAC_BAUDRATE(3);
        MAC_BAUDRATE(12);
        MAC_BAUDRATE(24);
        MAC_BAUDRATE(48);
        MAC_BAUDRATE(96);
#ifdef B19200
        MAC_BAUDRATE(192);
#endif
#ifdef B38400
        MAC_BAUDRATE(384);
#endif
#ifdef B57600
        MAC_BAUDRATE(576);
#endif
#ifdef B115200
        MAC_BAUDRATE(1152);
#endif
#ifdef B230400
        MAC_BAUDRATE(2304);
#endif

        default:
            break;
    }

    return speed;

#undef  MAC_BAUDRATE
}
// ***************************************************************************
// FUNCTION
//      refineByteSize
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      UINT   baudrate --
//      UINT   byteSize --
// RESULT
//      UINT --
// ***************************************************************************
static UINT refineByteSize(UINT byteSize)
{
#define MAC_BYTESIZE(a)     case a: result = CS##a; break;

    UINT        result          = 0;

    switch (4 + byteSize)
    {
        MAC_BYTESIZE(5);
        MAC_BYTESIZE(6);
        MAC_BYTESIZE(7);
        MAC_BYTESIZE(8);

        default:
            result = CS8;
            break;
    }

    return result;

#undef  MAC_BYTESIZE
}
// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      AXSerial::AXSerial
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//        --
// ***************************************************************************
AXSerial::AXSerial(void)
{
    i_fd = -1;
}
// ***************************************************************************
// FUNCTION
//      AXSerial::~AXSerial
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//        --
// ***************************************************************************
AXSerial::~AXSerial(void)
{
    close();
}
// ***************************************************************************
// FUNCTION
//      AXSerial::close
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//      AXSerial * --
// ***************************************************************************
BOOL AXSerial::close(void)
{
    ENTER(true);

    if (isOpen())
    {
        ::close(i_fd);
        i_fd = -1;
    }

    RETURN(true);
}
BOOL AXSerial::isOpen(void)
{
    return i_fd == -1 ? false : true;
}
// ***************************************************************************
// FUNCTION
//      AXSerial::open
// PURPOSE
//
// PARAMETERS
//      PSERIALPROPS   props --
// RESULT
//      AXSerial * --
// ***************************************************************************
BOOL AXSerial::open(PSERIALPROPS  props)
{
#define TYPE(a,b)       case SERIALTYPE_##a: psz_type = #b; break

    BOOL                result      = false;
    speed_t             speed;
    UINT                byteSize;
    struct termios      st_tty;
    int                 i_flags;
    PCSTR               psz_type;
    CHAR                sz_tty      [ A7LPATH ];

    ENTER(true);

    speed       = refineBaudrate(props->u_baudrate);
    byteSize    = refineByteSize(props->u_bits);

    switch (props->u_type)
    {
        TYPE(irda,  S);
        TYPE(usb,   USB);
        TYPE(acm,   ACM);

        default:
            psz_type = "S";
            break;
    }

    sprintf(sz_tty, "/dev/tty%s%d", psz_type, (int)props->u_port);

    A7TRACE((A7P, "  opening serial++ %s at %d\n", sz_tty, props->u_baudrate));

    if ((i_fd = ::open(sz_tty, O_RDWR | O_NOCTTY | O_NDELAY)) >= 0)
    {
        // O_NDELAY needs just for the fast opening
        i_flags = fcntl(i_fd, F_GETFL, 0);
        fcntl(i_fd, F_SETFL, i_flags & ~O_NDELAY);

        // FOR VALGRIND
        memset(&st_tty, 0, sizeof(st_tty));
        // FOR VALGRIND

        tcgetattr(i_fd, &st_tty);

        st_tty.c_cflag      = ((st_tty.c_cflag & ~CSIZE) | byteSize) |
                              CLOCAL | CREAD;
        st_tty.c_iflag      = IGNBRK;
        st_tty.c_lflag      = 0;
        st_tty.c_oflag      = 0;
        st_tty.c_cc[VMIN]   = 1;
        st_tty.c_cc[VTIME]  = 5;

// ------------------------------- Parity ------------------------------------

        if (props->b_parity)
        {
            if (props->b_even)
            {
                st_tty.c_cflag |= PARENB;
                st_tty.c_cflag &= ~PARODD;
            }
            else
                st_tty.c_cflag |= (PARENB | PARODD);
        }
        else
            st_tty.c_cflag &= ~(PARENB | PARODD);

// ------------------------------ Stop bits ----------------------------------

        if (props->u_stop)
            st_tty.c_cflag |= CSTOPB;
        else
            st_tty.c_cflag &= ~CSTOPB;

// ---------------------------- Flow control ---------------------------------

        if (props->b_x)
            st_tty.c_iflag |= IXON | IXOFF;
        else
            st_tty.c_iflag &= ~(IXON | IXOFF | IXANY);

        if (props->b_rtscts)
            st_tty.c_cflag |= CRTSCTS;
        else
            st_tty.c_cflag &= ~CRTSCTS;

        cfsetospeed(&st_tty, speed);
        cfsetispeed(&st_tty, speed);

        tcsetattr(i_fd, TCSANOW, &st_tty);

        if (props->b_rtscts)
        {
            i_flags  = 0;
            ioctl(i_fd, TIOCMGET, &i_flags);
            i_flags |= TIOCM_RTS;
            ioctl(i_fd, TIOCMSET, &i_flags);
        }

        result = true;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXSerial::read
// PURPOSE
//
// PARAMETERS
//      PVOID   data --
//      INT     size --
//      UINT    TO   --
// RESULT
//      INT  --
// ***************************************************************************
INT AXSerial::read(PVOID            data,
                   INT              size,
                   UINT             TO)
{
    return _la6_nonblk_dev_read(i_fd, data, size, TO);
}
// ***************************************************************************
// FUNCTION
//      AXSerial::write
// PURPOSE
//
// PARAMETERS
//      PVOID   data --
//      INT     size --
//      UINT    TO   --
// RESULT
//      INT  --
// ***************************************************************************
INT AXSerial::write(PCVOID          data,
                    INT             size,
                    UINT            TO)
{
    return _la6_nonblk_dev_write(i_fd, (PVOID)data, size, TO);
}
// ***************************************************************************
// FUNCTION
//      AXSerial::setBaudrate
// PURPOSE
//
// PARAMETERS
//      UINT   baudrate --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL AXSerial::setBaudrate(UINT baudrate)
{
    BOOL                b_result    = false;
    struct termios      st_tty;
    UINT                speed;

    if (!tcgetattr(i_fd, &st_tty))
    {
        speed = refineBaudrate(baudrate);

        cfsetospeed(&st_tty, (speed_t)speed);
        cfsetispeed(&st_tty, (speed_t)speed);

        if (!tcsetattr(i_fd, TCSANOW, &st_tty))
        {
            b_result = true;
        }
    }

    return b_result;
}

*/



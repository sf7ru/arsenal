// ***************************************************************************
// TITLE
//      Serial Port Communications for UNIX platform
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: sport.c,v 1.8 2004/03/24 08:28:26 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
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

#include <axsystem.h>
#include <axstring.h>
#include <serial.h>

#include <pvt_ax.h>


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// STRUCTURE
//      SERIALDEV
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_SERIALDEV
{
    AXDEV               st_dev;
    U32              flags;
    int                 i_fd;
} SERIALDEV, * PSERIALDEV;


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

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
#ifdef B460800
        MAC_BAUDRATE(4608);
#endif

#ifdef B500000
        MAC_BAUDRATE(5000);
#endif
#ifdef B576000
        MAC_BAUDRATE(5760);
#endif
#ifdef B921600
        MAC_BAUDRATE(9216);
#endif
#ifdef B1000000
        MAC_BAUDRATE(10000);
#endif
#ifdef B1152000
        MAC_BAUDRATE(11520);
#endif
#ifdef B1500000
        MAC_BAUDRATE(15000);
#endif
#ifdef B2000000
        MAC_BAUDRATE(20000);
#endif
#ifdef B2500000
        MAC_BAUDRATE(25000);
#endif
#ifdef B3000000
        MAC_BAUDRATE(30000);
#endif
#ifdef B3500000
        MAC_BAUDRATE(35000);
#endif
#ifdef B4000000
        MAC_BAUDRATE(40000);
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
// ***************************************************************************
// FUNCTION
//      _serial_write
// TYPE
//      static
// PURPOSE
//      Send block of data to Serial Port
// PARAMETERS
//      PSERIALDEV   pst_dev -- Device control structure
//      PVOID        p_data  -- Pointer to data block
//      INT          i_size  -- Size of block to write
//      UINT         u_mTO   -- Timeout for operation in msecs
// RESULT
//      INT -- Result code
//          -1:     Error has occured
//          0:      Timeout
//          Other:  Size of written data
// ***************************************************************************
INT _serial_write(PAXDEV         dev,
                  PCVOID         p_data,
                  INT            i_size,
                  UINT           u_mTO)
{
    PSERIALDEV          pst_dev         = (PSERIALDEV)dev;
    INT                 i_sent          = -1;

    ENTER(pst_dev);

    i_sent = _ax_nonblock_dev_write(pst_dev->i_fd, (PVOID)p_data, i_size, u_mTO);

    RETURN(i_sent);
}
// ***************************************************************************
// FUNCTION
//      _serial_read
// TYPE
//      static
// PURPOSE
//      Receive block of data from Serial Port
// PARAMETERS
//      PSERIALDEV   pst_dev -- Device control structure
//      PVOID        p_data  -- Pointer to data block
//      INT          i_size  -- Size of block to read
//      UINT         u_mTO   -- Timeout for operation in msecs
// RESULT
//      INT -- Result code
//          -1:     Error has occured
//          0:      Timeout
//          Other:  Size of read data
// ***************************************************************************
INT _serial_read(PAXDEV          dev,
                 PVOID           p_data,
                 INT             i_size,
                 UINT            u_mTO)
{
    PSERIALDEV          pst_dev         = (PSERIALDEV)dev;
    INT                 i_received      = -1;

    ENTER(pst_dev);

    i_received = _ax_nonblock_dev_read(pst_dev->i_fd, p_data, i_size, u_mTO);

    RETURN(i_received);
}
/*
// ***************************************************************************
// FUNCTION
//      serial_purge
// PURPOSE
//      Purge Serial Port
// PARAMETERS
//      PSERIALDEV    pst_dev   -- Device control structure
//      U32         u_flags  -- Flag or combination which define
//                                 what kind of buffers purge
//                  AXDEVFL_READ:      Purge receiving buffers
//                  AXDEVFL_WRITE:     Purge transmitting buffers
// RESULT
//      BOOL  -- true if all is Ok, false if not
// ***************************************************************************
static BOOL _serial_purge(PSERIALDEV    pst_dev,
                          U32        u_flags)
{
    BOOL                b_result        = false;

    ENTER(pst_dev);

    switch (u_flags & AXDEVFL_FULL)
    {
        case AXDEVFL_READ:
            b_result = !tcflush(pst_dev->i_fd, TCIFLUSH);
            break;

        case AXDEVFL_WRITE:
            b_result = !tcflush(pst_dev->i_fd, TCOFLUSH);
            break;

        case AXDEVFL_FULL:
            b_result = !tcflush(pst_dev->i_fd, TCIOFLUSH);
            break;
    }

    RETURN(b_result);
}
*/
// ***************************************************************************
// FUNCTION
//      _serial_ctl
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PSERIALDEV   pst_dev --
//      UINT         u_func  --
//      PVOID        p_param --
//      UINT         u_param --
// RESULT
//      INT --
// ***************************************************************************
static INT _serial_ctl(PSERIALDEV   pst_dev,
                       UINT         u_func,
                       PVOID        p_param,
                       UINT         u_param)
{
    INT                 i_result        = -1;
    struct termios      st_tty;

    ENTER(pst_dev);

    switch (u_func)
    {
        case AXDEVCTL_SETSPEED:
            if (!tcgetattr(pst_dev->i_fd, &st_tty))
            {
                cfsetospeed(&st_tty, (speed_t)u_param);
                cfsetispeed(&st_tty, (speed_t)u_param);

                i_result = tcsetattr(pst_dev->i_fd, TCSANOW, &st_tty);
            }
            break;

        case AXDEVCTL_GETFD:
            i_result = pst_dev->i_fd;
            break;

        default:
            break;
    }

    RETURN(i_result);
}
// ***************************************************************************
// FUNCTION
//      serial_close
// PURPOSE
//      Close Serial Port Device
// PARAMETERS
//      PAXDEV   pst_dev -- Pointer to Device control structure
// RESULT
//      PAXDEV  -- Always NULL
// ***************************************************************************
PAXDEV serial_close(PAXDEV pst_dev)
{
    PSERIALDEV          pst_port      = (PSERIALDEV)pst_dev;

    ENTER(pst_port);

    if (pst_port->i_fd != -1)
        close(pst_port->i_fd);

    FREE(pst_port);

    RETURN(NULL);
}
// ***************************************************************************
// FUNCTION
//      serial_open
// PURPOSE
//
// PARAMETERS
//      PSERIALPROPS p    --
//      UINT         type --
// RESULT
//      PAXDEV --
// ***************************************************************************
PAXDEV serial_open(PSERIALPROPS p,
                   UINT         type)
{
#define TYPE(a,b)       case SERIALTYPE_##a: psz_type = b; break

    PCSTR               psz_type;
    CHAR                sz_tty      [ AXLPATH ];

    switch (type)
    {
        TYPE(usb,   "ttyUSB");
        TYPE(acm,   "ttyACM");
        TYPE(pts,   "pts/");

        default:
#if ((TARGET_ARCH == __AX_arm__) && !defined(UART_NAMES_ARE_TRADITIONAL))
            psz_type = "ttyO";
#else
            psz_type = "ttyS";
#endif
            break;
    }

    strz_sformat(sz_tty, AXLPATH, "/dev/%s%d", psz_type, (int)p->u_port);

    printf("serial_open: %s\n" , sz_tty);

    return serial_open_x(p, type, sz_tty);

#undef TYPE
}
// ***************************************************************************
// FUNCTION
//      serial_open_x
// PURPOSE
//
// PARAMETERS
//      PSERIALPROPS p    --
//      UINT         type --
//      PCSTR        name --
// RESULT
//      PAXDEV --
// ***************************************************************************
PAXDEV serial_open_x(PSERIALPROPS p,
                     UINT         type,
                     PCSTR        name)
{
    PSERIALDEV          pst_dev    = NULL;
    speed_t             speed;
    UINT                byteSize;
    struct termios      st_tty;
    int                 i_flags;

    ENTER(true);

    speed       = refineBaudrate(p->u_baudrate);
    byteSize    = refineByteSize(p->u_bits);

    AXTRACE("  opening serial %s at %d\n", name, p->u_baudrate * 100);

    if ((pst_dev = CREATE(SERIALDEV)) != NULL)
    {
//        switch (type)
//        {
//            case SERIALTYPE_pipe:
//            case SERIALTYPE_file:
//                pst_dev->flags |= SERIALFLAG_notty;
//                break;
//
//            default:
//                break;
//        }

        if ((pst_dev->i_fd = open(name, O_RDWR | O_NOCTTY | O_NDELAY)) >= 0)
        {
            // O_NDELAY needs just for the fast opening
            i_flags = fcntl(pst_dev->i_fd, F_GETFL, 0);
            fcntl(pst_dev->i_fd, F_SETFL, i_flags & ~O_NDELAY);

            // FOR VALGRIND
            memset(&st_tty, 0, sizeof(st_tty));

            tcgetattr(pst_dev->i_fd, &st_tty);

            st_tty.c_cflag      = ((st_tty.c_cflag & ~CSIZE) | byteSize) |
                                  CLOCAL | CREAD;
            st_tty.c_iflag      = IGNBRK;
            st_tty.c_lflag      = 0;
            st_tty.c_oflag      = 0;
            st_tty.c_cc[VMIN]   = 1;
            st_tty.c_cc[VTIME]  = 5;

    // ------------------------------- Parity ------------------------------------

            switch (p->b_parity)
            {
                case SERIALPARITY_even:
                    st_tty.c_cflag |= PARENB;
                    st_tty.c_cflag &= ~PARODD;
                    break;

                case SERIALPARITY_odd:
                    st_tty.c_cflag |= (PARENB | PARODD);
                    break;

                default:
                    st_tty.c_cflag &= ~(PARENB | PARODD);
                    break;

            }

//            if (p->b_parity)
//            {
//                if (p->b_even)
//                {
//                    st_tty.c_cflag |= PARENB;
//                    st_tty.c_cflag &= ~PARODD;
//                }
//                else
//                    st_tty.c_cflag |= (PARENB | PARODD);
//            }
//            else
//                st_tty.c_cflag &= ~(PARENB | PARODD);

    // ------------------------------ Stop bits ----------------------------------

            if (p->u_stop)
                st_tty.c_cflag |= CSTOPB;
            else
                st_tty.c_cflag &= ~CSTOPB;

    // ---------------------------- Flow control ---------------------------------

            if (p->flow == SERIALFLOW_soft)
                st_tty.c_iflag |= IXON | IXOFF;
            else
                st_tty.c_iflag &= ~(IXON | IXOFF | IXANY);

            if (p->flow == SERIALFLOW_rtscts)
                st_tty.c_cflag |= CRTSCTS;
            else
                st_tty.c_cflag &= ~CRTSCTS;

            cfsetospeed(&st_tty, speed);
            cfsetispeed(&st_tty, speed);

            tcsetattr(pst_dev->i_fd, TCSANOW, &st_tty);

            if (p->flow == SERIALFLOW_rtscts)
            {
                i_flags  = 0;
                ioctl(pst_dev->i_fd, TIOCMGET, &i_flags);
                i_flags |= TIOCM_RTS;
                ioctl(pst_dev->i_fd, TIOCMSET, &i_flags);
            }

            pst_dev->st_dev.pfn_ctl     = (PAXDEVFNCTL)_serial_ctl;
            pst_dev->st_dev.pfn_close   = (PAXDEVFNCLOSE)serial_close;
            pst_dev->st_dev.pfn_read    = (PAXDEVFNREAD)_serial_read;
            pst_dev->st_dev.pfn_write   = (PAXDEVFNWRITE)_serial_write;
        }
        else
        {
            AXTRACE("serial open  name: %s", name);
            perror("serial open error: ");
            pst_dev = (PSERIALDEV)serial_close((PAXDEV)pst_dev);
        }
    }

    RETURN((PAXDEV)pst_dev);
}
BOOL serial_set_baudrate(PAXDEV     dev,
                         UINT       baudrate)
{
    BOOL                b_result        = false;
    PSERIALDEV          d               = (PSERIALDEV)dev;
    struct termios      st_tty;
    UINT                speed;

    ENTER(d);

    if (!tcgetattr(d->i_fd, &st_tty))
    {
        speed = refineBaudrate(baudrate);

        cfsetospeed(&st_tty, (speed_t)speed);
        cfsetispeed(&st_tty, (speed_t)speed);

        if (!tcsetattr(d->i_fd, TCSANOW, &st_tty))
        {
            b_result = true;
        }
    }

    RETURN(b_result);
}
void serial_set_dtr(PAXDEV             dev,
                    BOOL               value)
{
    PSERIALDEV          d               = (PSERIALDEV)dev;
    int                 iFlags          = TIOCM_DTR;

    if (value)
        ioctl(d->i_fd, TIOCMBIS, &iFlags);
    else
        ioctl(d->i_fd, TIOCMBIC, &iFlags);
}

void serial_set_rts(PAXDEV             dev,
                    BOOL               value)
{
    PSERIALDEV          d               = (PSERIALDEV)dev;
    int                 iFlags          = TIOCM_RTS;

    if (value)
        ioctl(d->i_fd, TIOCMBIS, &iFlags);
    else
        ioctl(d->i_fd, TIOCMBIC, &iFlags);
}

/*
PAXDEV serial_open(PSERIALPROPS p)
{
#define TYPE(a,b)       case SERIALTYPE_##a: psz_type = #b; break

    PSERIALDEV          pst_dev    = NULL;
    struct termios      st_tty;
    SPORTPARAMS         st_params;
    int                 i_flags;
    PSTR                psz_type;
    CHAR                sz_tty      [ AXLPATH ];

    ENTER(true);

    memset(&st_params, 0, sizeof(SPORTPARAMS));
    st_params.i_baudrate    = (int)u_baudrate;
    st_params.i_byte_size   = SERIALFL_BITS_VALUE(u_flags);

    if (_serial_params_normalize(&st_params))
    {
        if ((pst_dev = CREATE(SERIALDEV)) != NULL)
        {
            pst_dev->i_fd      = -1;
            // pst_dev->u_flags   = u_flags;

            switch (v_type)
            {
                TYPE(irda,  S);
                TYPE(usb,   USB);
                TYPE(acm,   ACM);

                default:
                    psz_type = "S";
                    break;
            }

            sprintf(sz_tty, "/dev/tty%s%d", psz_type, u_port_num);

            AXTRACE("  opening serial %s at %d\n", sz_tty, u_baudrate);

            if ((pst_dev->i_fd = open(sz_tty, O_RDWR | O_NOCTTY | O_NDELAY)) >= 0)
            {
                // O_NDELAY needs just for the fast opening
                i_flags = fcntl(pst_dev->i_fd, F_GETFL, 0);
                fcntl(pst_dev->i_fd, F_SETFL, i_flags & ~O_NDELAY);

                // FOR VALGRIND
                memset(&st_tty, 0, sizeof(st_tty));
                // FOR VALGRIND

                tcgetattr(pst_dev->i_fd, &st_tty);

                st_tty.c_cflag      = ((st_tty.c_cflag & ~CSIZE) | st_params.i_byte_size) |
                                      CLOCAL | CREAD;
                st_tty.c_iflag      = IGNBRK;
                st_tty.c_lflag      = 0;
                st_tty.c_oflag      = 0;
                st_tty.c_cc[VMIN]   = 1;
                st_tty.c_cc[VTIME]  = 5;

// ------------------------------- Parity ------------------------------------

                switch (u_flags & SERIALFL_PAR_MASK)
                {
                    case SERIALFL_PAR_EVEN:
                        st_tty.c_cflag |= PARENB;
                        st_tty.c_cflag &= ~PARODD;
                        break;

                    case SERIALFL_PAR_ODD:
                        st_tty.c_cflag |= (PARENB | PARODD);
                        break;

                    default:
                        st_tty.c_cflag &= ~(PARENB | PARODD);
                        break;
                }

// ------------------------------ Stop bits ----------------------------------

                if (u_flags & SERIALFL_STOP_2)
                    st_tty.c_cflag |= CSTOPB;
                else
                    st_tty.c_cflag &= ~CSTOPB;

// ---------------------------- Flow control ---------------------------------

                if (u_flags & SERIALFL_FCTRL_X)
                    st_tty.c_iflag |= IXON | IXOFF;
                else
                    st_tty.c_iflag &= ~(IXON | IXOFF | IXANY);

                if (u_flags & SERIALFL_FCTRL_RTSCTS)
                    st_tty.c_cflag |= CRTSCTS;
                else
                    st_tty.c_cflag &= ~CRTSCTS;

                cfsetospeed(&st_tty, (speed_t)st_params.i_baudrate);
                cfsetispeed(&st_tty, (speed_t)st_params.i_baudrate);

                tcsetattr(pst_dev->i_fd, TCSANOW, &st_tty);

                if (u_flags & SERIALFL_FCTRL_RTSCTS)
                {
                    i_flags  = 0;
                    ioctl(pst_dev->i_fd, TIOCMGET, &i_flags);
                    i_flags |= TIOCM_RTS;
                    ioctl(pst_dev->i_fd, TIOCMSET, &i_flags);
                }


                pst_dev->st_dev.pfn_ctl     = (PAXDEVFNCTL)_serial_ctl;
                pst_dev->st_dev.pfn_close   = (PAXDEVFNCLOSE)serial_close;
                pst_dev->st_dev.pfn_read    = (PAXDEVFNREAD)_serial_read;
                pst_dev->st_dev.pfn_write   = (PAXDEVFNWRITE)_serial_write;
            }
            else
            {
                perror("Serial open");

                pst_dev = (PSERIALDEV)serial_close((PAXDEV)pst_dev);
            }
        }
    }

    RETURN((PAXDEV)pst_dev);
}
*/

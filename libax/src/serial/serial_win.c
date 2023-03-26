// ***************************************************************************
// TITLE
//      Serial Port Communications for Win32
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: sport.c,v 1.8 2004/03/24 08:28:26 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************

#include <stdio.h>

#include <windows.h>

#include <arsenal.h>

#include <a7system.h>
#include <serial.h>

#include <pvt_a7.h>
#include <pvt_win.h>

// ---------------------------------------------------------------------------
// ----------------------------- LOCAL TYPES ---------------------------------
// ---------------------------------------------------------------------------

#define DEF_MOD             (GENERIC_READ | GENERIC_WRITE)
#define DEF_ATTR            (FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED)
#define DEF_FULL_PURGE      (PURGE_TXABORT | PURGE_RXABORT  |   \
                             PURGE_TXCLEAR | PURGE_RXCLEAR  )

#define MODEM_ONLINE(a)     (GetCommModemStatus(a->h_dev,   \
                                &d_modem_stat) && (d_modem_stat & MS_RLSD_ON))

#define DEF_BUFF_SIZE       (16 * 1024)


// ---------------------------------------------------------------------------
// ----------------------------- LOCAL TYPES ---------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// STRUCTURE
//      SERIALDEV
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_SERIALDEV
{
    A7DEV               st_dev;
    BOOL                b_sd;
    HANDLE              h_dev;
} SERIALDEV, * PSERIALDEV;


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

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
INT _serial_write(PA7DEV         dev,
                  PCVOID         p_data,
                  INT            i_size,
                  UINT           u_mTO)
{
    PSERIALDEV          pst_dev         = (PSERIALDEV)dev;
    INT                 i_sent          = -1;
    DWORD               d_modem_stat    = 0;

    ENTER(pst_dev && p_data && i_size);

    if (!pst_dev->b_sd || MODEM_ONLINE(pst_dev))
    {
        i_sent  = _la6_nonblk_file_write(
                            pst_dev->h_dev, (PVOID)p_data, i_size, u_mTO);
    }
    else
        i_sent = 0;

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
INT _serial_read(PA7DEV          dev,
                 PVOID           p_data,
                 INT             i_size,
                 UINT            u_mTO)
{
    PSERIALDEV          pst_dev         = (PSERIALDEV)dev;
    INT                 i_received      = -1;
    DWORD               d_modem_stat    = 0;

    ENTER(pst_dev && p_data && i_size);

    if (!pst_dev->b_sd || MODEM_ONLINE(pst_dev))
    {
        i_received  = _la6_nonblk_file_read(
                            pst_dev->h_dev, p_data, i_size, u_mTO);
    }
    else
        i_received = 0;


    RETURN(i_received);
}
// ***************************************************************************
// FUNCTION
//      serial_close
// PURPOSE
//      Close Serial Port Device
// PARAMETERS
//      PA7DEV   pst_dev -- Pointer to Device control structure
// RESULT
//      PA7DEV  -- Always NULL
// ***************************************************************************
PA7DEV serial_close(PA7DEV pst_dev)
{
    PSERIALDEV          pst_port      = (PSERIALDEV)pst_dev;

    ENTER(pst_port);

    if (pst_port->h_dev != INVALID_HANDLE_VALUE)
        CloseHandle(pst_port->h_dev);

    FREE(pst_port);

    RETURN(NULL);
}
// ***************************************************************************
// FUNCTION
//      serial_open
// PURPOSE
//      Open Serial Port Device
// PARAMETERS
//      UIN    u_port_num   -- Serial port number (from 0)
//      UINT   u_baudrate   -- Baud rate
//                  300:        300 baud
//                  1200:       1200 baud
//                  2400:       2400 baud
//                  4800:       4800 baud
//                  9600:       9600 baud
//                  19200:      19200 baud
//                  38400:      38400 baud
//                  57600:      57600 baud
//                  115200:     115200 baud
//                  230400:     230400 baud
//      UINT32 u_flags      -- Flags
//                  SERIALFL_READ:              Can read Port
//                  SERIALFL_WRITE:             Can write Port
//                  SERIALFL_FCTRL_NONE:        No flow control
//                  SERIALFL_FCTRL_X:           X-flow control
//                  SERIALFL_FCTRL_DTRDSR:      DTR/DSR flow control
//                  SERIALFL_FCTRL_RTSCTS:      RTS/CTS flow control
//                  SERIALFL_BITS_5:            5 data bits
//                  SERIALFL_BITS_6:            6 data bits
//                  SERIALFL_BITS_7:            7 data bits
//                  SERIALFL_BITS_8:            8 data bits
//                  SERIALFL_IRDA:              Port is an IRDa device
//                  SERIALFL_MODEM:             Use modem controls
// RESULT
//      PA7DEV  -- Pointer to Device control structure
// ***************************************************************************
PA7DEV serial_open(PSERIALPROPS       p,
                   UINT               type)
{
    PSERIALDEV          pst_dev        = NULL;
    DCB                 st_DCB;
//#if (TARGET_SYSTEM == __AX_wince__)
//    TCHAR               sz_name    [ 5 ] = TEXT("COM :");
//#else
    CHAR                sz_name    [ A7LSHORT ];
//#endif
//    COMMTIMEOUTS        st_timeouts     = { -1, 0, 0, 10, 100 };
    COMMTIMEOUTS        st_timeouts     = { -1, 0, -1, 10, 0 };

    ENTER(p->u_baudrate);

    snprintf(sz_name, A7LSHORT, "\\\\.\\COM%u", p->u_port);

// ----------------------- Setting COM parameters ----------------------------

    memset(&st_DCB, 0, sizeof(DCB));
    st_DCB.DCBlength            = sizeof(DCB);
    st_DCB.BaudRate             = p->u_baudrate * 100;
    st_DCB.fBinary              = TRUE;
    st_DCB.fDtrControl          = DTR_CONTROL_ENABLE;
    st_DCB.ByteSize             = p->u_bits + 4;

    if (p->b_parity)
    {
        st_DCB.Parity           = p->b_even ? EVENPARITY : ODDPARITY;
        st_DCB.fParity          = TRUE;
    }

    if (p->b_x) // (u_flags & SERIALFL_FCTRL_X)
    {
        //st_DCB.fOutX            = TRUE;
        //st_DCB.fInX             = TRUE;
        st_DCB.XonLim           = 2048;
        st_DCB.XoffLim          = 2048;
        st_DCB.XonChar          = 0x11;
        st_DCB.XoffChar         = 0x13;
        st_DCB.EvtChar          = 0xA;
        st_DCB.EofChar          = 0xA;
    }

    if (p->b_dtrdsr)// (u_flags & SERIALFL_FCTRL_DTRDSR)
    {
        st_DCB.fDsrSensitivity  = TRUE;
        st_DCB.fOutxDsrFlow     = TRUE;
        st_DCB.fDtrControl      = DTR_CONTROL_HANDSHAKE;
    }

    if (p->b_rtscts) // (u_flags & SERIALFL_FCTRL_RTSCTS)
    {
        st_DCB.fOutxCtsFlow     = TRUE;
        st_DCB.fRtsControl      = RTS_CONTROL_HANDSHAKE;
    }

    if ((pst_dev = CREATE(SERIALDEV)) != NULL)
    {
        pst_dev->h_dev              = INVALID_HANDLE_VALUE;
        pst_dev->b_sd               = p->b_sd;

        //pst_dev->st_dev.pfn_ctl = (PA7DEVFNCTL);
        pst_dev->st_dev.pfn_close   = (PA7DEVFNCLOSE)serial_close;
        pst_dev->st_dev.pfn_read    = (PA7DEVFNREAD)_serial_read;
        pst_dev->st_dev.pfn_write   = (PA7DEVFNWRITE)_serial_write;

        if ( !( ((pst_dev->h_dev = CreateFile(sz_name, DEF_MOD, 0, NULL,
                  OPEN_EXISTING, DEF_ATTR, NULL)) != INVALID_HANDLE_VALUE)  &&
                SetupComm(pst_dev->h_dev, DEF_BUFF_SIZE, DEF_BUFF_SIZE)    &&
                PurgeComm(pst_dev->h_dev, DEF_FULL_PURGE)                  &&
#if (TARGET_SYSTEM == __AX_wince__)
                ((p->b_type != SERIALTYPE_irda) ||
                    EscapeCommFunction(pst_dev->h_dev, SETIR))             &&
#endif
                SetCommMask(pst_dev->h_dev, EV_RXCHAR)                     &&
                SetCommTimeouts(pst_dev->h_dev, &st_timeouts)              &&
                SetCommState(pst_dev->h_dev, &st_DCB)                      ))
        {
            pst_dev = (PSERIALDEV)serial_close((PA7DEV)pst_dev);
        }
    }

    RETURN((PA7DEV)pst_dev);
}
UINT serial_get_error()
{
    return GetLastError();
}

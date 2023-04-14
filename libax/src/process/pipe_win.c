// ***************************************************************************
// TITLE
//      Anonymous  Pipes Win32 Platform
// PROJECT
//      Arsenal Library
// ***************************************************************************
//
// FILE
//      $Id: pipe.c,v 1.2 2003/08/06 12:25:58 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// HISTORY
//      $Log: pipe.c,v $
//      Revision 1.2  2003/08/06 12:25:58  A.Kozhukhar
//      *** empty log message ***
//
//      Revision 1.1  2003/06/18 15:10:02  A.Kozhukhar
//      *** empty log message ***
//
//      Revision 1.2  2003/04/24 10:07:28  A.Kozhukhar
//      *** empty log message ***
//
//      Revision 1.1.1.1  2003/02/14 13:17:33  A.Kozhukhar
//      Initial release
//
// ***************************************************************************


#include <stdlib.h>

#include <windows.h>

#include <arsenal.h>

#include <axsystem.h>
#include <pvt_win.h>
#include <axtime.h>

// ---------------------------------------------------------------------------
// -------------------------- LOCAL DEFINITIONS ------------------------------
// ---------------------------------------------------------------------------

#define DEF_READ_QUANT      10



// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------


// ***************************************************************************
// FUNCTION
//      axpipe_destroy
// PURPOSE
//      Close Pipe and destroy Pipe control structure
// PARAMETERS
//      HAXPIPE h_pipe -- Handle of Pipe
// RESULT
//      HAXPIPE    -- Always NULL
// ***************************************************************************
HAXPIPE axpipe_destroy(HAXPIPE h_pipe)
{
    PAXPIPE            pst_pipe    = (PAXPIPE)h_pipe;

    if (pst_pipe)
    {
        if (pst_pipe->h_in != INVALID_HANDLE_VALUE)
            CloseHandle(pst_pipe->h_in);

        if (pst_pipe->h_out != INVALID_HANDLE_VALUE)
            CloseHandle(pst_pipe->h_out);

        FREE(pst_pipe);
    }

    return NULL;
}

// ***************************************************************************
// STATIC FUNCTION
//      _axpipe_fifo_make
// PURPOSE
//      Make Pipe's FIFO buffer
// PARAMETERS
//      PVOID   * ph_in  -- Pointer to pointer to inbound buffer
//      PVOID   * ph_out -- Pointer to pointer to outbound buffer
//      DWORD     d_buff_size -- Size of buffers, can be zero
// RESULT
//      BOOL
//          TRUE:  All is ok
//          FALSE: An error has occured
// ***************************************************************************
static BOOL _axpipe_fifo_make(PVOID * ph_in, PVOID * ph_out, DWORD d_buff_size)
{
    SECURITY_ATTRIBUTES     sa;

    memset(&sa, 0, sizeof(SECURITY_ATTRIBUTES));
    sa.nLength          = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle   = TRUE;

    return CreatePipe(ph_in, ph_out, &sa, d_buff_size);
}

// ***************************************************************************
// FUNCTION
//      axpipe_create
// PURPOSE
//      Open Pipe and create Pipe control structure
// PARAMETERS
//      DWORD    d_buff_size -- Size of buffers, can be zero
// RESULT
//      HAXPIPE    -- Handle of the just created Pipe if all is ok
//                     or NULL if not
// ***************************************************************************
HAXPIPE axpipe_create(UINT d_buff_size)
{
    PAXPIPE                pst_pipe    = NULL;

    if ((pst_pipe = MALLOC(sizeof(AXPIPE))) != NULL)
    {
        memset(pst_pipe, 0, sizeof(AXPIPE));

        pst_pipe->h_in     = INVALID_HANDLE_VALUE;
        pst_pipe->h_out    = INVALID_HANDLE_VALUE;

        if (!_axpipe_fifo_make(&pst_pipe->h_in, &pst_pipe->h_out, d_buff_size))
        {
            pst_pipe = (PAXPIPE)axpipe_destroy((HAXPIPE)pst_pipe);
        }
    }

    return (HAXPIPE)pst_pipe;
}

// ***************************************************************************
// FUNCTION
//      axpipe_read
// PURPOSE
//      Read from Pipe
// PARAMETERS
//      HAXPIPE h_pipe         -- Handle of Pipe
//      PVOID    p_buff         -- Pointer to target buffer
//      INT      i_size         -- Size to read
//      UINT     u_timeout_msec -- Timeout for operation in msecs
// RESULT
//      INT   -- Size of read data
// ***************************************************************************
INT axpipe_read(HAXPIPE h_pipe, PVOID p_buff, INT i_size, UINT u_timeout_msec)
{
    PAXPIPE            pst_pipe    = (PAXPIPE)h_pipe;
    int                 i_read      = -1;
    DWORD               d_read;
    UINT                u_timeout;
    DWORD               d_total;

    if (p_buff && i_size && pst_pipe && (pst_pipe->h_in != INVALID_HANDLE_VALUE))
    {
        u_timeout = (u_timeout_msec > DEF_READ_QUANT ? u_timeout_msec / DEF_READ_QUANT : DEF_READ_QUANT);
        i_read    = 0;

        while ( (u_timeout--) &&
                PeekNamedPipe(pst_pipe->h_in, NULL, 0, NULL, &d_total, NULL) &&
                !d_total)
        {
            axsleep(DEF_READ_QUANT);
        }

        if (d_total)
        {
            if (ReadFile(pst_pipe->h_in, p_buff, (d_total > (DWORD)i_size ? i_size : d_total), &d_read, NULL))
            {
                i_read = (int)d_read;
            }
            else
                i_read = -1;
        }
    }

    return i_read;
}

// ***************************************************************************
// FUNCTION
//      axpipe_write
// PURPOSE
//      Write to Pipe
// PARAMETERS
//      HAXPIPE h_pipe         -- Handle of Pipe
//      PVOID    p_buff         -- Pointer to data to write
//      INT      i_size         -- Size of data to write
//      UINT     u_timeout_msec -- Timeout for operation in msecs
// RESULT
//      INT    -- Size of written data
// ***************************************************************************
INT  axpipe_write(HAXPIPE h_pipe, PVOID p_buff, INT i_size, UINT u_timeout_msec)
{
    PAXPIPE            pst_pipe    = (PAXPIPE)h_pipe;
    INT                 i_written   = -1;
    DWORD               d_written;


    if (p_buff && i_size && pst_pipe && (pst_pipe->h_out != INVALID_HANDLE_VALUE))
    {
        if (WriteFile(pst_pipe->h_out, p_buff, i_size, &d_written, NULL))
        {
            i_written = (int)d_written;
        }
    }

    return i_written;
}

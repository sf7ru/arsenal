// ***************************************************************************
// TITLE
//      Anonymous  Pipes UNIX Platform
// PROJECT
//      Arsenal Library
// ***************************************************************************
//
// FILE
//      $Id: pipe.c,v 1.1 2003/06/18 15:10:02 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// HISTORY
//      $Log: pipe.c,v $
//      Revision 1.1  2003/06/18 15:10:02  A.Kozhukhar
//      *** empty log message ***
//
//      Revision 1.2  2003/04/24 10:07:28  A.Kozhukhar
//      *** empty log message ***
//
//      Revision 1.1  2003/02/28 10:34:08  A.Kozhukhar
//      Initial release
//
// ***************************************************************************


#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>

#include <axsystem.h>

#include <pvt_unix.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>

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
    PAXPIPE        pst_pipe        = (PAXPIPE)h_pipe;

    if (pst_pipe)
    {
        if (pst_pipe->i_in != -1)
        {
            close(pst_pipe->i_in);
        }

        if (  (pst_pipe->i_out != pst_pipe->i_in)   &&
              (pst_pipe->i_out != -1)               )
        {
            close(pst_pipe->i_out);
        }

        FREE(pst_pipe);
    }

    return NULL;
}

// ***************************************************************************
// FUNCTION
//      axpipe_create
// PURPOSE
//      Open Pipe and create Pipe control structure
// PARAMETERS
//      UINT     d_buff_size -- Size of buffers, can be zero
// RESULT
//      HAXPIPE    -- Pointer to the just created Pipe if all is ok
//                     or NULL if not
// ***************************************************************************
HAXPIPE axpipe_create(UINT d_buff_size)
{
    PAXPIPE             pst_pipe        = NULL;
    int                 ai_pipe         [ 2 ];

    if ((pst_pipe = CREATE(AXPIPE)) != NULL)
    {
        pst_pipe->i_in  = -1;
        pst_pipe->i_out = -1;

        if (!pipe(ai_pipe))
        {
            pst_pipe->i_in  = ai_pipe[0];
            pst_pipe->i_out = ai_pipe[1];

            axpipe_set_buff_size((HAXPIPE)pst_pipe, d_buff_size);
        }
        else
            pst_pipe = (PAXPIPE)axpipe_destroy((HAXPIPE)pst_pipe);
    }

    return (HAXPIPE)pst_pipe;
}

// ***************************************************************************
// FUNCTION
//      axpipe_read
// PURPOSE
//      Read from Pipe
// PARAMETERS
//      HAXPIPE h_pipe    -- Handle of Pipe
//      PVOID    p_data  -- Pointer to target buffer
//      INT      i_size    -- Size to read
//      DWORD    d_timeout -- Timeout for operation in msecs
// RESULT
//      INT    -- Size of read data
// ***************************************************************************
INT axpipe_read(HAXPIPE     h_pipe,
                PVOID       p_data,
                INT         i_size,
                UINT        u_timeout_msec)
{
    PAXPIPE            pst_pipe        = (PAXPIPE)h_pipe;
    INT                 i_read          = -1;

    if (pst_pipe)
    {
        i_read = _ax_nonblock_dev_read(pst_pipe->i_in, p_data, i_size, u_timeout_msec);
    }

    return i_read;
}

// ***************************************************************************
// FUNCTION
//      axpipe_write
// PURPOSE
//
// PARAMETERS
//      HAXPIPE h_pipe         -- Handle of Pipe
//      PVOID    p_data         -- Pointer to data to write
//      INT      i_size         -- Size of data to write
//      UINT     u_timeout_msec -- Timeout for operation in msecs
// RESULT
//      INT -- Size of written data
// ***************************************************************************
INT axpipe_write(HAXPIPE    h_pipe,
                 PVOID      p_data,
                 INT        i_size,
                 UINT       u_timeout_msec)
{
    PAXPIPE             pst_pipe        = (PAXPIPE)h_pipe;
    INT                 i_written       = -1;

    if (pst_pipe)
    {
        i_written = _ax_nonblock_dev_write(pst_pipe->i_out, p_data, i_size, u_timeout_msec);
//        tcflush(pst_pipe->i_out,TCIOFLUSH);
    }

//    printf("axpipe_write stage i_written = %d\n", i_written);

    return i_written;
}
BOOL axpipe_set_buff_size(HAXPIPE            h_pipe,
                          UINT               size)
{
    BOOL                result          = false;
    PAXPIPE             pst_pipe        = (PAXPIPE)h_pipe;

    ENTER(true);


    /* no stdio-buffering, please! */
//    setvbuf(stdout, NULL,_IONBF, 0);

//    if (fcntl(pst_pipe->i_out, F_SETPIPE_SZ, size) < 0)
//    {
//        perror("set pipe size failed.");
//    }
//    else
//        result = true;


    RETURN(result);

}
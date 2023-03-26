// ***************************************************************************
// TITLE
//      UNIX specific routines
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: specials.c,v 1.2 2003/04/24 10:07:28 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************


#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#include <axsystem.h>
#include <sys/stat.h>
#include <fcntl.h>

//#include <la6sockets.h>



// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------


// ***************************************************************************
// FUNCTION
//      _axnonblk_dev_read
// PURPOSE
//
// PARAMETERS
//      int   i_fd           --
//      PVOID p_data         --
//      int   i_size         --
//      UINT  u_timeout_msec --
// RESULT
//      int --
// ***************************************************************************
int _ax_nonblock_dev_read(int        i_fd,
                          PVOID      p_data,
                          int        i_size,
                          UINT       u_timeout_msec)
{
    INT                 i_read          = -1;
    BOOL                b_do            = false;
    int                 i_avail;
    struct  timeval     st_tv;
    fd_set              fds;
    INT                 rd;


    ENTER((i_fd != -1) && p_data && i_size);

    switch (u_timeout_msec)
    {
        case -1:
// ------------------------------ Blocking -----------------------------------

            b_do = true;
            break;

        case 0:
// -------------------------- Without awaiting -------------------------------

            if (ioctl(i_fd, FIONREAD, &i_avail) != -1)
            {
                if (i_avail)
                    b_do    = true;
                else
                    i_read  = 0;
            }
            else
                perror("_ax_nonblock_dev_read ioctl");
            break;

        default:
// ---------------------------- With Awaiting --------------------------------

            st_tv.tv_sec  = 0;
            st_tv.tv_usec = u_timeout_msec * 1000;

            FD_ZERO(&fds);
            FD_SET(i_fd, &fds);

            do
            {
                switch (rd = select(i_fd + 1, &fds, NULL, NULL, &st_tv))
                {
                    case -1:
                        perror("_ax_nonblock_dev_read select");
                        break;

                    case 0:
                        i_read      = 0;
                        break;

                    default:
                        b_do        = true;
                        break;
                }
            } while ((rd == -1) && (errno == EINTR));

            break;
    }

    if (b_do)
    {
// ------------------------------- Reading -----------------------------------

        switch (i_read = read(i_fd, p_data, i_size))
        {
            case -1:
                perror("_ax_nonblock_dev_read read");
                break;

            case 0:
                if (errno == EIO)
                {
                    i_read = -1;
                }
                break;

            default:
                break;
        }
    }

    RETURN(i_read);
}

// ***************************************************************************
// FUNCTION
//      _axnonblk_dev_write
// PURPOSE
//
// PARAMETERS
//      int   i_fd           --
//      PVOID p_data         --
//      int   i_size         --
//      UINT  u_timeout_msec --
// RESULT
//      int --
// ***************************************************************************
int _ax_nonblock_dev_write(int       i_fd,
                          PVOID     p_data,
                          int       i_size,
                          UINT      u_timeout_msec)
{
    INT                 i_written       = -1;
    BOOL                b_do            = false;
    struct  timeval     st_tv;
    fd_set              fds;


    ENTER((i_fd != -1) && p_data && i_size);

    switch (u_timeout_msec)
    {
        case -1:
// ------------------------------ Blocking -----------------------------------

            b_do            = true;
            break;

        default:
// ---------------------------- With Awaiting --------------------------------

            st_tv.tv_sec  = 0;
            st_tv.tv_usec = u_timeout_msec * 1000;

            FD_ZERO(&fds);
            FD_SET(i_fd, &fds);

            switch (select(i_fd + 1, NULL, &fds, NULL, &st_tv))
            {
                case -1:
                    break;

                case 0:
                    i_written   = 0;
                    break;

                default:
                    b_do        = true;
                    break;
            }
            break;
    }

    if (b_do)
    {
// ------------------------------- Writing -----------------------------------

        i_written = write(i_fd, p_data, i_size);
    }

    RETURN(i_written);
}

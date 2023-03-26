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

#include <stdio.h>
#include <poll.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <pvt_net.h>
#include <string.h>
#include <net/if.h>

#include <axstring.h>

#ifndef POLLRDNORM
# define POLLRDNORM 0x040
#endif


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      _sig_SIGPIPE
// PURPOSE
//
// PARAMETERS
//      int   i --
// RESULT
//      void  --
// ***************************************************************************
static void _sig_SIGPIPE(int i)
{
    AXTRACE("SIGPIPE: %d", i);
}
// ***************************************************************************
// FUNCTION
//      axsocket_start
// PURPOSE
//      Start Sockets support
// PARAMETERS
//      none
// RESULT
//      true if all is ok or false if error has occured
// ***************************************************************************
BOOL axsocket_start(void)
{
//    signal(SIGPIPE, SIG_IGN);
    signal(SIGPIPE, _sig_SIGPIPE);

    return true;
}
// ***************************************************************************
// FUNCTION
//      _set_nonblock
// PURPOSE
//
// PARAMETERS
//      int pst_dev->v_sock --
//      BOOL            b_set    --
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
static BOOL _set_nonblock(int v_sock, BOOL b_set)
{
    BOOL            b_result        = false;
    int             i_flags;

    ENTER(true);

    i_flags = fcntl(v_sock, F_GETFL, 0);

    if (b_set)
    {
        i_flags |= O_NONBLOCK;
    }
    else
    {
        i_flags &= ~O_NONBLOCK;
    }

    if (!fcntl(v_sock, F_SETFL, i_flags))
    {
        b_result = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      _axssocket_connect_nb
// PURPOSE
//
// PARAMETERS
//      PSSOCKDEV           pst_dev   --
//      struct sockaddr *   pst_sa    --
//      UINT                u_sa_size --
//      UINT                u_TO_msec --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL _axssocket_connect_nb(PSSOCKDEV            pst_dev,
                           struct sockaddr *    pst_sa,
                           UINT                 u_sa_size,
                           UINT                 u_TO_msec)
{
    BOOL                b_result        = false;
    int                 i_valopt;
    socklen_t           v_lon;
    int                 rd;

    ENTER(true);

    if (_set_nonblock(pst_dev->v_sock, true))
    {
        switch (rd = connect(pst_dev->v_sock, pst_sa, u_sa_size))
        {
            case 0:
                b_result = true;
                break;

            case -1:
                if (errno == EINPROGRESS)
                {
                    v_lon  = sizeof(i_valopt);

                    if (  (_axsocket_select(pst_dev->v_sock ,
                                     SELECT_WRITE, u_TO_msec) > 0)          &&

                          !getsockopt(pst_dev->v_sock, SOL_SOCKET,
                                     SO_ERROR, (void*)(&i_valopt), &v_lon)  &&
                          !i_valopt                                         )
                    {
                        b_result = true;
                    }
                    else
                        AXTRACE("_axssocket_connect_nb: select error: %d", errno);
                }
                else
                    AXTRACE("_axssocket_connect_nb: connect error: %d", errno);
                break;

            default:
                AXTRACE("_axssocket_connect_nb: connect return unexepcted: %d", rd);
                break;
        }

        _set_nonblock(pst_dev->v_sock, false);
    }
    else
        AXTRACE("_axssocket_connect_nb: cannot set non-block");

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axsocket_stop
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//      void  --
// ***************************************************************************
void axsocket_stop(void)
{
    signal(SIGPIPE, SIG_DFL);
}
/*
// ***************************************************************************
// FUNCTION
//      _axsocket_is_connected
// PURPOSE
//
// PARAMETERS
//      SOCKET   v_sock --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL _axsocket_is_connected(SOCKET v_sock)
{
    BOOL                b_result    = false;
    int                 i_res;

    ENTER(v_sock != INVALID_SOCKET);

    switch (recv(v_sock, (char*)&i_res, 1, MSG_PEEK | MSG_DONTWAIT))
    {
        case -1:
            if (errno == EAGAIN)
            {
                b_result = true;
            }
            break;

        case 0:
            if (errno != ESPIPE)
            {
                b_result = true;
//                perror("bebee");
//                errno = 0;
                AXTRACE("  bebe = %d", errno);
            }
            break;

        default:
            b_result = true;
            break;
    }

    RETURN(b_result);
}
*/
// ***************************************************************************
// FUNCTION
//      _axsocket_is_connected
// PURPOSE
//
// PARAMETERS
//      SOCKET   v_sock --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL _axsocket_is_connected(SOCKET v_sock)
{
    BOOL                b_result    = true;
//    int                 i_res;
//    int                 i_len;
    struct pollfd pfd;
    char buffer[32];

    ENTER(v_sock != INVALID_SOCKET);

    // use the poll system call to be notified about socket status changes

    pfd.fd = v_sock;
    pfd.events = POLLIN | POLLHUP | POLLRDNORM;
    pfd.revents = 0;

    if (poll(&pfd, 1, 100) > 0)
    {
        // if result > 0, this means that there is either data available on the
        // socket, or the socket has been closed

        if (recv(v_sock, buffer, sizeof(buffer), MSG_PEEK | MSG_DONTWAIT) == 0)
        {
            b_result    = false;
        }
    }
/*
    switch (recv(v_sock, (char*)&i_res, 1, MSG_PEEK | MSG_DONTWAIT))
    {
        case -1:
//            if (errno == EAGAIN)
            {
                b_result = true;
            }
            break;

        case 0:
            if (errno != ESPIPE)
            {
                b_result = true;
                perror("bebee");
                errno = 0;
            }
            break;

        default:
            b_result = true;
            break;
    }
*/

    RETURN(b_result);
}

// ***************************************************************************
// FUNCTION
//      axssocket_error
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//      UINT  --
// ***************************************************************************
UINT axsocket_error(void)
{
    UINT            error  = 0;

    ENTER(true);

    error = errno;

    RETURN(error);
}
// ***************************************************************************
// FUNCTION
//      axsocket_get_iface_ip
// PURPOSE
//
// PARAMETERS
//      PSTR addr   --
//      UINT lenght --
//      PSTR iface  --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL axsocket_get_iface_ip(PSTR          addr,
                           UINT          lenght,
                           PSTR          iface)
{
    BOOL                b_result        = false;
    int                 sfd;
    struct ifreq        ifr;
    struct sockaddr_in *sin             = (struct sockaddr_in *) &ifr.ifr_addr;

    ENTER(true);

    memset(&ifr, 0, sizeof ifr);

    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) != -1)
    {
        strz_cpy(ifr.ifr_name, iface, IFNAMSIZ);

        sin->sin_family = AF_INET;

        if (!ioctl(sfd, SIOCGIFADDR, &ifr))
        {
            strz_cpy(addr, inet_ntoa(sin->sin_addr), lenght);

            b_result = true;
        }

        close(sfd);
    }

    RETURN(b_result);
}

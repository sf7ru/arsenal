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

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <axplatforms.h>

#if (TARGET_FAMILY == __AX_unix__)
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#elif (TARGET_FAMILY == __AX_rtos__)

#else
//#define __USE_W32_SOCKETS
//#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#endif

#include <arsenal.h>
#include <axnet.h>
#include <axstring.h>

#include <pvt_net.h>


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      _axsocket_addr_set
// PURPOSE
//
// PARAMETERS
//      struct sockaddr_in * pst_sa    --
//      PSTR                 psz_iface --
//      UINT                 u_port    --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL _axsocket_addr_set(struct sockaddr_in *  pst_sa,
                        PCSTR                 psz_iface,
                        UINT                  u_port)
{
    BOOL                b_result        = false;
    struct hostent *    pst_hp;
    ULONG               u_s_addr;

    ENTER(true);

    memset(pst_sa, 0, sizeof(struct sockaddr_in));
    pst_sa->sin_port = htons((u_short)u_port);

    if (psz_iface)
    {
        if ((pst_sa->sin_addr.s_addr = inet_addr(psz_iface)) == INADDR_NONE)
        {
            if ((pst_hp = gethostbyname(psz_iface)) != NULL)
            {
                memcpy(&u_s_addr, pst_hp->h_addr, sizeof(u_s_addr));

                pst_sa->sin_family      = pst_hp->h_addrtype;
                pst_sa->sin_addr.s_addr = u_s_addr;
                b_result                = true;
            }
        }
        else
        {
            pst_sa->sin_family  = AF_INET;
            b_result            = true;
        }
    }
    else
    {
        pst_sa->sin_family      = AF_INET;
        pst_sa->sin_addr.s_addr = htonl(INADDR_ANY);
        b_result                = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      _axsocket_select
// PURPOSE
//
// PARAMETERS
//      INT  i_socket --
//      INT  mode     --
//      UINT TO       --
// RESULT
//      INT --
// ***************************************************************************
INT _axsocket_select(INT    i_socket,
                     INT    mode,
                     UINT   TO)
{
    INT                     result          = -1;
    struct timeval          st_tv;
    fd_set                  fds;
    ldiv_t                  divr;

    ENTER(true);

    divr = ldiv(TO, 1000);

    st_tv.tv_sec  = divr.quot;
    st_tv.tv_usec = divr.rem * 1000;

    FD_ZERO(&fds);
    FD_SET(i_socket, &fds);

    //applog(AXLOGDL_error, "_axsocket_select stage 1: TO = %u", TO);

    do
    {
        result = select(i_socket + 1,
                        mode & SELECT_READ   ? &fds : NULL,
                        mode & SELECT_WRITE  ? &fds : NULL,
                        mode & SELECT_EXCEPT ? &fds : NULL, &st_tv);


        if (result <= 0)
        {
            AXTRACE("_axsocket_select stage S(r=%d e=%d)\n", result, errno);
        }

    } while ((result == -1) && (errno == EINTR));

    if (result > 0)
    {
        if (
                        //(errno == EINPROGRESS) ||
                        (!FD_ISSET(i_socket, &fds))
                        )
        {
            AXTRACE("_axsocket_select stage S(i=NOT)\n");
            result = 0;
        }
    }

    //applog(AXLOGDL_error, "_axsocket_select stage 10 = %d", result);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      _axsocket_bind
// PURPOSE
//
// PARAMETERS
//      SOCKET v_socket  --
//      PCSTR  psz_iface --
//      UINT   u_low     --
//      UINT   u_high    --
// RESULT
//      UINT --
// ***************************************************************************
UINT _axsocket_bind(SOCKET      v_socket,
                    PCSTR       psz_iface,
                    UINT        u_low,
                    UINT        u_high)
{
    UINT                u_port          = 0;
    u_short             u_tmp;
    struct sockaddr_in  st_sa;

    ENTER(v_socket != INVALID_SOCKET);

    if (_axsocket_addr_set(&st_sa, psz_iface, 0))
    {
        for(  u_tmp = u_high;
              !u_port && (u_tmp > u_low);
              u_tmp--)
        {
//            A7TRACE("  binding socket to port %d\n", u_tmp);

            st_sa.sin_port = htons(u_tmp);

            if (!bind(v_socket, (struct sockaddr*)&st_sa, sizeof(st_sa)))
            {
                u_port = u_tmp;
            }
        }
    }

    RETURN(u_port);
}

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

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <pvt_net.h>

#include <axstring.h>
#include <axlog.h>

#include <pvt_net.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      _ssocket_ctl
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PSSOCKDEV   pst_dev --
//      UINT        u_func  --
//      PVOID       p_param --
//      UINT        u_param --
// RESULT
//      INT --
// ***************************************************************************
static INT _ssocket_ctl(PSSOCKDEV   pst_dev,
                        UINT         u_func,
                        PVOID        p_param,
                        UINT         u_param)
{
    INT                 i_result        = -1;

    ENTER(pst_dev);

    switch (u_func)
    {
        case AXDEVCTL_GETFD:
            i_result = pst_dev->v_sock;
            break;
    }

    RETURN(i_result);
}
// ***************************************************************************
// FUNCTION
//      _ssocket_read
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PSSOCKDEV   pst_dev --
//      PVOID       p_data  --
//      INT         i_size  --
//      UINT        u_mTO   --
// RESULT
//      INT --
// ***************************************************************************
static INT _ssocket_read(PSSOCKDEV      pst_dev,
                         PVOID          p_data,
                         INT            i_size,
                         UINT           u_TO)
{
    INT                 i_received      = -1;
    INT                 i_to_recv;

    ENTER(pst_dev && i_size);

    if (u_TO != (UINT)-1)
    {
// ---------------------------- Non-blocking ---------------------------------

        if (u_TO)
        {
            _axsocket_select(pst_dev->v_sock, SELECT_READ, u_TO);
        }

        if (ioctlsocket(pst_dev->v_sock, FIONREAD, &i_to_recv) != SOCKET_ERROR)
        {
            if (i_to_recv)
            {
                if (i_to_recv > i_size)
                    i_to_recv = i_size;


                if (p_data)
                {
                    i_received = recv(pst_dev->v_sock, p_data, i_to_recv, 0);
                }
                else
                    i_received = i_to_recv;
            }
            else
            {
                i_received = _axsocket_is_connected(pst_dev->v_sock) ? 0 : -1;
            }
        }
    }
    else
    {
// ------------------------------ Blocking -----------------------------------

        i_received = recv(pst_dev->v_sock, p_data, i_size, 0);
    }

    RETURN(i_received);
}
// ***************************************************************************
// FUNCTION
//      _ssocket_write
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PSSOCKDEV   pst_dev --
//      PVOID       p_data  --
//      INT         i_size  --
//      UINT        u_mTO   --
// RESULT
//      INT --
// ***************************************************************************
static INT _ssocket_write(PSSOCKDEV     pst_dev,
                          PVOID         p_data,
                          INT           i_size,
                          UINT          u_TO)
{
    INT                 i_sent          = -1;
    BOOL                b_do            = true;

    ENTER(pst_dev && p_data && i_size);

    if (u_TO != (UINT)-1)
    {
// ---------------------------- Non-blocking ---------------------------------

        i_sent = 0;                         // No error, just sent nothing

        if (u_TO)
        {
            b_do = (_axsocket_select(pst_dev->v_sock, SELECT_WRITE, u_TO) > 0);
        }

        if (b_do)
        {            
            i_sent = send(pst_dev->v_sock, p_data, i_size, 0);
        }
    }
    else
    {
// ------------------------------ Blocking -----------------------------------

        i_sent = send(pst_dev->v_sock, p_data, i_size, 0);
    }

    RETURN(i_sent);
}
// ***************************************************************************
// FUNCTION
//      _setdev
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PSSOCKDEV   pst_dev --
// RESULT
//      void --
// ***************************************************************************
static void _setdev(PSSOCKDEV pst_dev)
{
    ENTER(true);

    pst_dev->st_dev.pfn_ctl     = (PAXDEVFNCTL)_ssocket_ctl;
    pst_dev->st_dev.pfn_close   = (PAXDEVFNCLOSE)axssocket_close;
    pst_dev->st_dev.pfn_read    = (PAXDEVFNREAD)_ssocket_read;
    pst_dev->st_dev.pfn_write   = (PAXDEVFNWRITE)_ssocket_write;
    pst_dev->st_dev.pfn_accept  = (PAXDEVFNACCEPT)axssocket_accept;

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      axssocket_close
// PURPOSE
//
// PARAMETERS
//      PAXDEV   h_dev --
//      UINT     u_TO  --
// RESULT
//      PAXDEV  --
// ***************************************************************************
PAXDEV axssocket_close(PAXDEV             h_dev)
{
    PSSOCKDEV           pst_dev     = (PSSOCKDEV)h_dev;
    UINT                tries       = 3;

    ENTER(pst_dev);

    if (pst_dev->v_sock != INVALID_SOCKET)
    {
        do
        {
            shutdown(pst_dev->v_sock, SD_SEND);
        }
        while ((tries--) && !closesocket(pst_dev->v_sock));
    }

    FREE(pst_dev);
    pst_dev = NULL;

    RETURN((PAXDEV)pst_dev);
}
// ***************************************************************************
// FUNCTION
//      axssocket_listen
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_iface --
//      UINT   u_port    --
// RESULT
//      PAXDEV  --
// ***************************************************************************
PAXDEV axssocket_listen(PSTR             psz_iface,
                        UINT             u_port,
                        UINT             u_flags)
{
    PSSOCKDEV           pst_dev         = NULL;
    int                 i_yes           = 1;
    struct sockaddr_in  st_sa;

    ENTER(u_port);

    if ((pst_dev = CREATE(SSOCKDEV)) != NULL)
    {
        pst_dev->v_sock = INVALID_SOCKET;

        if (    _axsocket_addr_set(&st_sa, psz_iface, u_port)           &&
                ((pst_dev->v_sock =
                    socket(AF_INET, SOCK_STREAM, 0)) != INVALID_SOCKET) &&
                (!(u_flags & 1) ||
                ((setsockopt(pst_dev->v_sock, SOL_SOCKET, SO_REUSEADDR,
                    (char *)&i_yes, sizeof(int)))    != SOCKET_ERROR))  &&
                (bind(pst_dev->v_sock, (struct sockaddr *)&st_sa,
                    sizeof(struct sockaddr_in))      != SOCKET_ERROR)   &&
                (listen(pst_dev->v_sock,
                    SOMAXCONN)                       != SOCKET_ERROR)   )
        {
            _setdev(pst_dev);
        }
        else
            pst_dev = (PSSOCKDEV)axssocket_close((PAXDEV)pst_dev);
    }

    RETURN((PAXDEV)pst_dev);
}
// ***************************************************************************
// FUNCTION
//      axssocket_listen_any
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_iface --
//      UINT   u_low     --
//      UINT   u_high    --
// RESULT
//      PAXDEV  --
// ***************************************************************************
PAXDEV axssocket_listen_any(PSTR        psz_iface,
                            UINT        u_low,
                            UINT        u_high)
{
    PSSOCKDEV           pst_dev         = NULL;
//    int                 i_yes           = 1;

    ENTER(u_low || u_high);

    if ((pst_dev = CREATE(SSOCKDEV)) != NULL)
    {
        pst_dev->v_sock = INVALID_SOCKET;

        if (    ((pst_dev->v_sock =
                    socket(AF_INET, SOCK_STREAM, 0)) != INVALID_SOCKET) &&
//                ((setsockopt(pst_dev->v_sock, SOL_SOCKET, SO_REUSEADDR,
//                    (char *)&i_yes, sizeof(int)))    != SOCKET_ERROR)   &&
                ((pst_dev->u_port = _axsocket_bind(pst_dev->v_sock,
                    psz_iface, u_low, u_high)) != 0)                    &&
                (listen(pst_dev->v_sock,
                    SOMAXCONN)                       != SOCKET_ERROR)   )
        {
            _setdev(pst_dev);
        }
        else
            pst_dev = (PSSOCKDEV)axssocket_close((PAXDEV)pst_dev);
    }

    RETURN((PAXDEV)pst_dev);
}
// ***************************************************************************
// FUNCTION
//      axssocket_get_port
// PURPOSE
//
// PARAMETERS
//      PAXDEV   h_dev --
// RESULT
//      UINT  --
// ***************************************************************************
UINT axssocket_get_port(PAXDEV        h_dev)
{
    UINT                u_port      = 0;
    PSSOCKDEV           pst_dev     = (PSSOCKDEV)h_dev;

    ENTER(pst_dev);

    u_port = pst_dev->u_port;

    RETURN(u_port);
}
// ***************************************************************************
// FUNCTION
//      axssocket_connect
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_host  --
//      UINT   u_port    --
//      PSTR   psz_iface --
//      UINT   u_highest --
//      UINT   u_TO      --
// RESULT
//      PAXDEV  --
// ***************************************************************************
PAXDEV axssocket_connect(PCSTR        psz_host,
                         UINT         u_port,
                         PCSTR        psz_iface,
                         UINT         u_high,
                         UINT         u_TO)
{
    PSSOCKDEV           pst_dev         = NULL;
//    int                 i_yes       = 1;
    struct sockaddr_in  st_sa;

    ENTER(psz_host && u_port);

    if (_axsocket_addr_set(&st_sa, psz_host, u_port))
    {
        if ((pst_dev = CREATE(SSOCKDEV)) != NULL)
        {
            pst_dev->v_sock = INVALID_SOCKET;

            if (    ((pst_dev->v_sock = socket(AF_INET,
                        SOCK_STREAM, 0)) != INVALID_SOCKET)         &&
//                    ((setsockopt(pst_dev->v_sock, SOL_SOCKET,
//                        SO_REUSEADDR, (char *)&i_yes,
//                        sizeof(int))) != SOCKET_ERROR)              &&
                    (!u_high || _axsocket_bind(
                        pst_dev->v_sock, psz_iface,
                        u_port, u_high))                            &&
                    _axssocket_connect_nb(pst_dev,
                        (struct sockaddr *)&st_sa,
                        sizeof(st_sa), u_TO)                        )
            {
                _setdev(pst_dev);
            }
            else
            {
                AXTRACE(AXLOGDL_error, "axssocket_connect: cannot create socket: %d", errno);

                pst_dev = (PSSOCKDEV)axssocket_close((PAXDEV)pst_dev);
            }
        }
        else
            AXTRACE(AXLOGDL_error, "axssocket_connect: cannot create SOCK structure");
    }
    else
        AXTRACE(AXLOGDL_error, "axssocket_connect: cannot set address");

    RETURN((PAXDEV)pst_dev);
}
// ***************************************************************************
// FUNCTION
//      axssocket_accept
// PURPOSE
//      Accept connection on listening socket
// PARAMETERS
//      PAXDEV   h_listen -- Handler of listen socket
//      UINT     u_TO     -- Timeout for operation in msecs
// RESULT
//      PAXDEV  -- Handler of the new connection, or NULL if no one connected
// ***************************************************************************
PAXDEV axssocket_accept(PAXDEV      h_listen,
                        UINT        u_TO)
{
    PSSOCKDEV           pst_connect         = NULL;
    PSSOCKDEV           pst_listen          = (PSSOCKDEV)h_listen;
    struct sockaddr_in  st_client_addr;
    UINT                u_remote_addr_len;
    SOCKET              v_connect;

    ENTER(pst_listen && (pst_listen->v_sock != INVALID_SOCKET));


    u_remote_addr_len = sizeof(st_client_addr);

    if ( (_axsocket_select(pst_listen->v_sock,
            SELECT_READ, u_TO) > 0)                 &&
         ((v_connect = accept(pst_listen->v_sock,
            (struct sockaddr *)&st_client_addr,
            &u_remote_addr_len)) != INVALID_SOCKET) &&
         ((pst_connect = CREATE(SSOCKDEV)) != NULL) )
    {
        _setdev(pst_connect);

        pst_connect->v_sock = v_connect;

        memcpy(&pst_connect->st_peer_addr,
               &st_client_addr,
               sizeof(struct sockaddr_in));
    }

    RETURN((PAXDEV)pst_connect);
}
// ***************************************************************************
// FUNCTION
//      axssocket_peer_get
// PURPOSE
//
// PARAMETERS
//      PAXDEV h_dev    --
//      PSTR   psz_name --
//      UINT   u_len    --
//      PUINT  pu_port  --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL axssocket_peer_get(PAXDEV             h_dev,
                        PSTR               psz_name,
                        UINT               u_len,
                        PUINT              pu_port)
{
    BOOL                b_result        = false;
    PSSOCKDEV           pst_dev         = (PSSOCKDEV)h_dev;
    PSTR                psz_tmp;

    ENTER(psz_name && u_len && pu_port);

    if ((psz_tmp = inet_ntoa(pst_dev->st_peer_addr.sin_addr)) != NULL)
    {
        strz_cpy(psz_name, psz_tmp, u_len);

        *pu_port    = pst_dev->st_peer_addr.sin_port;
        b_result    = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axssocket_get_peer
// PURPOSE
//
// PARAMETERS
//      PAXDEV   h_dev    --
//      PSTR     psz_name --
//      UINT     u_len    --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL axssocket_peer_get_s(PAXDEV        h_dev,
                          PSTR          psz_name,
                          UINT          u_len)
{
    BOOL                b_result        = false;
    PSSOCKDEV           pst_dev         = (PSSOCKDEV)h_dev;
    PSTR                psz_tmp;

    ENTER(true);

    if ((psz_tmp = inet_ntoa(pst_dev->st_peer_addr.sin_addr)) != NULL)
    {
        strz_sformat(psz_name, u_len, "%s:%d",
                     psz_tmp,
                     pst_dev->st_peer_addr.sin_port);

        b_result = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axssocket_is_connected
// PURPOSE
//
// PARAMETERS
//      PAXDEV h_dev --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL axssocket_is_connected(PAXDEV        h_dev)
{
    BOOL                b_result        = false;
    PSSOCKDEV           pst_dev         = (PSSOCKDEV)h_dev;

    ENTER(true);

    b_result = _axsocket_is_connected(pst_dev->v_sock);

    RETURN(b_result);
}

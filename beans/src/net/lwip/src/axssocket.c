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

#include <axnet.h>
#include <lwip/init.h>
#include <lwip/api.h>
#include <lwip/udp.h>
#include <lwip/ip4_addr.h>
#include <lwip/netif.h>
#include <lwip/tcpip.h>
#include <string.h>
//#include <lwip_sock_compat.h>
#include <lwip/dhcp.h>
#include <lwip/prot/dhcp.h>

#include <lwip/sockets.h>
#include <lwip/sys.h>

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct __tag_MYLWIPSOCK
{
    AXDEV               dev;
    int                 sock;
    UINT                u_port;
    struct sockaddr_in  st_peer_addr;

} MYLWIPSOCK, * PMYLWIPSOCK;


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

static BOOL _wait(int s, BOOL rd_wr, UINT u_TO)
{
    fd_set          readset;
    fd_set          writeset;
    fd_set          errset;
    div_t           secs;
    struct timeval  tv;

    ENTER(true);
    
    FD_ZERO(&readset);
    if (rd_wr)
        FD_SET(s, &readset);

    FD_ZERO(&writeset);

    if (!rd_wr)
        FD_SET(s, &writeset);

    FD_ZERO(&errset);
    //FD_SET(s, &errset);

    secs        = div(u_TO, 1000);
    tv.tv_sec   = secs.quot;
    tv.tv_usec  = secs.rem * 1000;

    int ret = lwip_select(s + 1, &readset, &writeset, &errset, &tv);

    return ret > 0;
}
static INT _ssocket_ctl(PMYLWIPSOCK  pst_dev,
                        UINT         u_func,
                        PVOID        p_param,
                        UINT         u_param)
{
    INT                 i_result        = -1;
    
    ENTER(pst_dev);

    RETURN(i_result);
}
static INT _ssocket_read(PMYLWIPSOCK    pst_dev,
                         PVOID          p_data,
                         INT            i_size,
                         UINT           u_TO)
{
    INT                 i_received      = -1;
    INT                 i_to_recv;

    ENTER(pst_dev && p_data && i_size);

    if (u_TO != (UINT)-1)
    {
        if (u_TO)
        {
            _wait(pst_dev->sock, true, u_TO);
        }
        
        if (lwip_ioctl(pst_dev->sock, FIONREAD, &i_to_recv) != -1)
        {
            if (i_to_recv)
            {
                if (i_to_recv > i_size)
                    i_to_recv = i_size;

                i_received = lwip_recv(pst_dev->sock, p_data, i_to_recv, 0);
            }
            else
            {
                i_received = 0;
                //i_received = _a7socket_is_connected(pst_dev->v_sock) ? 0 : -1;
            }
        }
    }
    else
    {
        i_received = lwip_recv(pst_dev->sock, p_data, i_size, 0);
    }
    
    RETURN(i_received);
}
static INT _ssocket_write(PMYLWIPSOCK   pst_dev,
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
            b_do = _wait(pst_dev->sock, false, u_TO);
        }
        
        if (b_do)
        {
            i_sent = lwip_send(pst_dev->sock, p_data, i_size, 0);
        }
    }
    else
    {
// ------------------------------ Blocking -----------------------------------

        i_sent = lwip_send(pst_dev->sock, p_data, i_size, 0);
    }
    
    RETURN(i_sent);
}
static void _setdev(PMYLWIPSOCK pst_dev)
{
    ENTER(true);

    pst_dev->dev.pfn_ctl     = (PAXDEVFNCTL)_ssocket_ctl;
    pst_dev->dev.pfn_close   = (PAXDEVFNCLOSE)axssocket_close;
    pst_dev->dev.pfn_read    = (PAXDEVFNREAD)_ssocket_read;
    pst_dev->dev.pfn_write   = (PAXDEVFNWRITE)_ssocket_write;
    pst_dev->dev.pfn_accept  = (PAXDEVFNACCEPT)axssocket_accept;

    QUIT;
}       
PAXDEV axssocket_connect(PCSTR              psz_host,
                         UINT               u_port,
                         PCSTR              psz_iface,
                         UINT               u_highest,
                         UINT               u_TO)
{
    PMYLWIPSOCK     result      = nil;
    u32_t           opt;

    struct sockaddr_in addr;

    if ((result = CREATE(MYLWIPSOCK)) != nil)
    {
        result->sock = -1;

        if ((result->sock = lwip_socket(AF_INET, SOCK_STREAM, 0)) >= 0)
        {
            opt = lwip_fcntl(result->sock, F_GETFL, 0);
            opt |= O_NONBLOCK;

            if (!lwip_fcntl(result->sock, F_SETFL, opt))
            {
                memset(&addr, 0, sizeof(addr));
                addr.sin_len = sizeof(addr);
                addr.sin_family = AF_INET;
                addr.sin_port = PP_HTONS(u_port);
                addr.sin_addr.s_addr = inet_addr(psz_host);

                if ((lwip_connect(result->sock, (struct sockaddr*)&addr, sizeof(addr)) == -1) && (errno == EINPROGRESS))
                {
                    if (_wait(result->sock, false, u_TO))
                    {
                        _setdev(result);
                    }
                    else
                        result = (PMYLWIPSOCK)axssocket_close((PAXDEV)result);
                }
                else
                    result = (PMYLWIPSOCK)axssocket_close((PAXDEV)result);
            }
            else
                result = (PMYLWIPSOCK)axssocket_close((PAXDEV)result);
        }
        else
            result = (PMYLWIPSOCK)axssocket_close((PAXDEV)result);
    }

    return (PAXDEV)result;
}
PAXDEV axssocket_close(PAXDEV             h_dev)
{
    PMYLWIPSOCK     pst_dev      = (PMYLWIPSOCK)h_dev;
    UINT            tries       = 3;

    ENTER(pst_dev);

    if (pst_dev->sock != -1)
    {
        do
        {
            //shutdown(pst_dev->v_sock, SD_SEND);
        }
        while ((tries--) && !lwip_close(pst_dev->sock));
    }

    FREE(pst_dev);
    pst_dev = NULL;

    RETURN((PAXDEV)pst_dev);
}
PAXDEV axssocket_accept(PAXDEV      h_listen,
                        UINT        u_TO)
{
    PMYLWIPSOCK         pst_connect         = NULL;
    PMYLWIPSOCK         pst_listen          = (PMYLWIPSOCK)h_listen;
    struct sockaddr_in  st_client_addr;
    socklen_t           u_remote_addr_len;
    int                 v_connect;

    ENTER(pst_listen && (pst_listen->v_sock != INVALID_SOCKET));


    u_remote_addr_len = sizeof(st_client_addr);

    if ( _wait(pst_listen->sock,
            true, u_TO)                 &&
         ((v_connect = lwip_accept(pst_listen->sock,
            (struct sockaddr*)&st_client_addr,
            &u_remote_addr_len)) != -1) &&
         ((pst_connect = CREATE(MYLWIPSOCK)) != NULL) )
    {
        _setdev(pst_connect);

        pst_connect->sock = v_connect;

        memcpy(&pst_connect->st_peer_addr,
               &st_client_addr,
               sizeof(struct sockaddr_in));
    }
    
    RETURN((PAXDEV)pst_connect);
}

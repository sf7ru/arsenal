// ***************************************************************************
// TITLE
//      Datagram Sockets abstraction layer
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: dgramsock.c,v 1.5 2004/03/24 08:28:26 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************


#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <axplatforms.h>

#if (TARGET_FAMILY == __AX_unix__)
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#endif

#include <arsenal.h>
#include <axnet.h>
#include <stdio.h>
#include <axstring.h>

#include <pvt_net.h>


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------


// ***************************************************************************
// FUNCTION
//      axusocket_close
// PURPOSE
//      Destroy socket
// PARAMETERS
//      HAXSOCKET h_socket -- Socket handler
// RESULT
//      HAXSOCKET  -- Always NULL
// ***************************************************************************
HAXSOCKET axusocket_close(HAXSOCKET h_socket)
{
    PAXSOCKET      pst_socket  = (PAXSOCKET)h_socket;

    if (pst_socket)
    {
        if (pst_socket->i_socket != INVALID_SOCKET)
            closesocket(pst_socket->i_socket);

        FREE(pst_socket);
    }

    return NULL;
}
// ***************************************************************************
// FUNCTION
//      axusocket_open
// PURPOSE
//      Connect Socket
// PARAMETERS
//      PSTR       psz_iface            -- Pointer to the Remote Host name
//      UINT       u_port              -- Port number
//      UINT       u_timeout_msecs     -- Timeout for operation (msecs)
// RESULT
//      HAXSOCKET  -- Pointer to the just created Socket control structure
//                     if connected or NULL if not
// ***************************************************************************
HAXSOCKET axusocket_open(PSTR       psz_iface,
                         UINT       u_port,
                         UINT       d_flags,
                         UINT       u_timeout_msecs)
{
    PAXSOCKET             pst_socket  = NULL;
    int                     i_yes       = 1;
//    struct sockaddr_in      st_sa;

    if (u_port)
    {
        if ((pst_socket = CREATE(AXSOCKET)) != NULL)
        {
            pst_socket->i_socket = INVALID_SOCKET;

            if ( _axsocket_addr_set(&pst_socket->st_sa, psz_iface, u_port)       &&
                 ((pst_socket->i_socket =
                        socket(AF_INET, SOCK_DGRAM, 0))             != -1)  &&
                 ( !(d_flags & DEFSO_REUSE_ADDR)  ||
                   ((setsockopt(pst_socket->i_socket, SOL_SOCKET,
                        SO_REUSEADDR,(char *)&i_yes, sizeof(int)))  != -1)) &&
                 (bind(pst_socket->i_socket,
                        (struct sockaddr*)&pst_socket->st_sa,
                        sizeof(pst_socket->st_sa))                  != -1)  )
            {
                if (_axsocket_select(pst_socket->i_socket, SELECT_WRITE, u_timeout_msecs) <= 0)
                {
                    pst_socket = (PAXSOCKET)axusocket_close((HAXSOCKET)pst_socket);
                }
            }
            else
            {
                AXTRACE((0, "DSOCK OPEN FAILED, address = '%s' : %d", psz_iface, u_port));
                pst_socket = (PAXSOCKET)axusocket_close((HAXSOCKET)pst_socket);
            }
        }
    }

    return (HAXSOCKET)pst_socket;
}
// ***************************************************************************
// FUNCTION
//      la6_dgsocket_multi_group
// PURPOSE
//
// PARAMETERS
//      HAXSOCKET h_sock    --
//      PSTR       psz_iface --
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
BOOL axusocket_mcast_set_noloop(HAXSOCKET    h_socket)
{
    BOOL                b_result        = false;
    PAXSOCKET         pst_socket      = (PAXSOCKET)h_socket;
    char                loopch          = 0;

    ENTER(pst_socket);

    if (setsockopt(pst_socket->i_socket, IPPROTO_IP,
                   IP_MULTICAST_LOOP, (char *)&loopch, sizeof(loopch)) != -1)
    {
        b_result = true;
    }
#if (TARGET_FAMILY == __AX_win__)
    else
        AXTRACE((0, "WSA error: %d", WSAGetLastError()));
#endif                                      //  #if (TARGET_FAMILY...

    RETURN(b_result);
}

// ***************************************************************************
// FUNCTION
//      la6_dgsocket_multi_group
// PURPOSE
//
// PARAMETERS
//      HAXSOCKET h_sock    --
//      PSTR       psz_iface --
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
BOOL axusocket_mcast_set_group(HAXSOCKET    h_socket,
                              PSTR          psz_group)
{
    BOOL                b_result        = false;
    PAXSOCKET         pst_socket      = (PAXSOCKET)h_socket;
    struct ip_mreq      st_mreq;

    ENTER(pst_socket && psz_group);

    st_mreq.imr_multiaddr.s_addr = inet_addr(psz_group);
    //st_mreq.imr_interface.s_addr = pst_socket->st_sa.sin_addr.s_addr;
    st_mreq.imr_interface = pst_socket->st_sa.sin_addr;

    if (setsockopt(pst_socket->i_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                    (char*)&st_mreq, sizeof(st_mreq)) != -1)
    {
        b_result = true;
    }
#if (TARGET_FAMILY == __AX_win__)
    else
        AXTRACE((0, "WSA error: %d", WSAGetLastError()));
#elif (TARGET_FAMILY == __AX_unix__)
    else
        perror("setsockopt: ");
#endif                                      //  #if (TARGET_FAMILY...

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axusocket_mcast_set_ttl
// PURPOSE
//
// PARAMETERS
//      HAXSOCKET h_sock --
//      UINT       u_ttl  --
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
BOOL axusocket_mcast_set_ttl(HAXSOCKET h_socket, UINT u_ttl)
{
    BOOL                b_result        = false;
    PAXSOCKET         pst_socket      = (PAXSOCKET)h_socket;

    ENTER(pst_socket);

    if (setsockopt(pst_socket->i_socket, IPPROTO_IP,
            IP_MULTICAST_TTL, (char *)&u_ttl, sizeof(u_ttl)) != -1)
    {
        b_result = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axusocket_mcast_set_iface
// PURPOSE
//
// PARAMETERS
//      HAXSOCKET h_socket  --
//      PSTR       psz_iface --
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
BOOL axusocket_mcast_set_iface(HAXSOCKET h_socket, PSTR psz_iface)
{
    BOOL                    b_result        = false;
    PAXSOCKET             pst_socket      = (PAXSOCKET)h_socket;
    unsigned long           u_addr;

    ENTER(pst_socket);
    
    u_addr = psz_iface ? inet_addr(psz_iface) : htonl(INADDR_ANY);

    if (setsockopt(pst_socket->i_socket, IPPROTO_IP,
                IP_MULTICAST_IF, (char *)&u_addr, sizeof(u_addr)) != -1)
    {
        b_result = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axusocket_recv
// PURPOSE
//      Receive data from peer
// PARAMETERS
//      HAXSOCKET   h_socket            -- Handler to Socket
//      HAXSOCKPEER h_peer              -- Handler to Peer which will receive
//                                          info about sender
//      PVOID        p_target            -- Pointer to target buffer
//      UINT         u_size              -- Size of target buffer
//      UINT         u_timeout_msecs     -- Timeout for operation (msecs)
// RESULT
//      INT    -- Number of received bytes
// ***************************************************************************
INT axusocket_recv(HAXSOCKET          h_socket,
                   HAXSOCKPEER        h_peer,
                   PVOID              p_target,
                   UINT               u_size,
                   UINT               u_timeout_msecs)
{
    PAXSOCKET           pst_socket      = (PAXSOCKET)h_socket;
    PAXSOCKPEER         pst_peer        = (PAXSOCKPEER)h_peer;
    BOOL                b_do            = false;
    int                 i_received      = -1;
    UINT                u_from_size;
    struct sockaddr_in  st_addr;

    if (pst_socket && p_target && u_size)
    {
// ------------------------------- Waiting -----------------------------------

        if (u_timeout_msecs != -1)
        {
            b_do = (_axsocket_select(pst_socket->i_socket, SELECT_READ, u_timeout_msecs) > 0);
        }
        else
            b_do = true;

// ------------------------------ Receiving ----------------------------------

        if (b_do)
        {
            u_from_size = sizeof(st_addr);

//            printf(">> recvfrom \n");

            i_received  = recvfrom(pst_socket->i_socket, p_target, u_size,
                                0, (struct sockaddr*)&st_addr, &u_from_size);

//            printf("<< recvfrom \n");

            if (pst_peer && (i_received  > 0))
            {
                memset(&pst_peer->st_addr, 0, sizeof(st_addr));
                memcpy(&pst_peer->st_addr, &st_addr, u_from_size);
            }
        }
        else
            i_received = 0;
    }

    return i_received;
}

// ***************************************************************************
// FUNCTION
//      axusocket_send
// PURPOSE
//
// PARAMETERS
//      HAXSOCKET   h_socket            -- Handler to Socket
//      HAXSOCKPEER h_peer              -- Handler to Peer
//      PVOID        p_data              -- Pointer to data buffer
//      UINT         u_size              -- Size of data to send
//      UINT         u_timeout_msecs     -- Timeout for operation (msecs)
// RESULT
//      INT    -- Number pf sent bytes
// ***************************************************************************
INT axusocket_send(HAXSOCKET          h_socket,
                   HAXSOCKPEER        h_peer,
                   PVOID              p_data,
                   UINT               u_size,
                   UINT               u_timeout_msecs)
{
    PAXSOCKET         pst_socket      = (PAXSOCKET)h_socket;
    PAXSOCKPEER        pst_peer        = (PAXSOCKPEER)h_peer;
    BOOL                b_do            = true;
    INT                 i_sent          = -1;

    if (pst_peer && p_data && u_size)
    {
// ------------------------------- Waiting -----------------------------------

        if (pst_socket && u_timeout_msecs != -1)
        {
            b_do = (_axsocket_select(pst_socket->i_socket, SELECT_WRITE, u_timeout_msecs) > 0);
        }


// ------------------------------- Sending -----------------------------------

        if (b_do)
        {
            //printf(">> axusocket_send_to stage 3\n");

            i_sent = sendto(pst_socket ? pst_socket->i_socket : -1,
                                p_data, u_size, 0,
                                (struct sockaddr*)&pst_peer->st_addr,
                                sizeof(pst_peer->st_addr));

            //printf(">> axusocket_send_to stage 4\n");
        }
        else
            i_sent = 0;
    }

    return i_sent;
}
// ***************************************************************************
// FUNCTION
//      axusocket_ip_is_mcast
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_addr --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL axusocket_ip_is_mcast(PSTR psz_addr)
{
    BOOL            b_result        = false;
    PSTR            psz_on;
    PSTR            psz_buff;
    int             i_net;

    ENTER(true);

    if ((psz_buff = strz_dup(psz_addr)) != NULL)
    {
        if ((psz_on = strchr(psz_buff, '.')) != NULL)
        {
            *psz_on     = 0;
            i_net       = atol(psz_buff);
            b_result    = ((i_net > 223) && (i_net < 240));
        }

        FREE(psz_buff);
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axusocket_bcast_set
// PURPOSE
//
// PARAMETERS
//      HAXSOCKET h_sock --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL axusocket_bcast_set(HAXSOCKET          h_sock,
                         BOOL               ena)
{
    BOOL            result          = false;
    PAXSOCKET     pst_socket      = (PAXSOCKET)h_sock;
    int             buf_ena             = ena ? 1 : 0;

    ENTER(true);

    result = setsockopt(pst_socket->i_socket, SOL_SOCKET, SO_BROADCAST, (CHAR*)&buf_ena, sizeof(buf_ena)) != -1;

    RETURN(result);
}


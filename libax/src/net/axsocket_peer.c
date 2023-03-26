// ***************************************************************************
// TITLE
//      Common Sockets Abstraction Layer
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: sockets.c,v 1.4 2003/12/08 12:48:23 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************


#include <stdlib.h>
#include <string.h>

#include <axplatforms.h>

#if (TARGET_FAMILY == __AX_unix__)
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#else
//#define __USE_W32_SOCKETS
//#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#endif

#include <arsenal.h>
#include <axnet.h>

#include <pvt_net.h>



// ---------------------------------------------------------------------------
// -------------------------- LOCAL DEFINITIONS ------------------------------
// ---------------------------------------------------------------------------

//#define DEF_DEFAULT_TIMEOUT     10000
//#define DEF_TIMEOUT_QUANT       10
//#define DEF_BIND_TRIES          3
//#define DEF_MAX_MSG_SIZE        65507
//#define DEF_BUFF_SIZE           32
//
//#define MAC_REMAP(a)    case DEFSO_##a: i_opt = SO_##a; break;



// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      axsocket_peer_destroy
// PURPOSE
//
// PARAMETERS
//      HAXSOCKPEER h_peer --
// RESULT
//      HAXSOCKPEER    --
// ***************************************************************************
HAXSOCKPEER axsocket_peer_destroy(HAXSOCKPEER h_peer)
{
    PAXSOCKPEER    pst_peer    = (PAXSOCKPEER)h_peer;

    SAFEFREE(pst_peer);

    return NULL;
}
// ***************************************************************************
// FUNCTION
//      axsocket_peer_set
// PURPOSE
//
// PARAMETERS
//      HAXSOCKPEER h_peer      --
//      PSTR         psz_address --
//      UINT         u_port      --
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
BOOL axsocket_peer_set(HAXSOCKPEER      h_peer,
                       PSTR             psz_address,
                       UINT             u_port)
{
    BOOL                b_result        = false;
    PAXSOCKPEER        pst_peer        = (PAXSOCKPEER)h_peer;
    struct hostent *    pst_hp;

    if (pst_peer && psz_address && u_port)
    {
        if ((pst_hp = gethostbyname(psz_address)) != NULL)
        {
            pst_peer->st_addr.sin_family    = pst_hp->h_addrtype;
            pst_peer->st_addr.sin_port      = htons((U16)u_port);

            memcpy((char *)&pst_peer->st_addr.sin_addr,
                                    pst_hp->h_addr, pst_hp->h_length);

            b_result                        = true;
        }
//        else
//            pst_peer = (PAXSOCKPEER)axsocket_peer_destroy((HAXSOCKPEER)pst_peer);
    }

    return b_result;
}

// ***************************************************************************
// FUNCTION
//      axsocket_peer_copy
// PURPOSE
//
// PARAMETERS
//      HAXSOCKPEER h_targte --
//      HAXSOCKPEER h_source --
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
BOOL axsocket_peer_copy(HAXSOCKPEER     h_targte,
                        HAXSOCKPEER     h_source)
{
    BOOL                b_result        = false;
    PAXSOCKPEER        pst_target      = (PAXSOCKPEER)h_targte;
    PAXSOCKPEER        pst_source      = (PAXSOCKPEER)h_source;


    if (pst_target && pst_source)
    {
        memcpy(pst_target, pst_source, sizeof(AXSOCKPEER));
        b_result = true;
    }

    return b_result;
}
HAXSOCKPEER axsocket_peer_dup(HAXSOCKPEER        h_source)
{
    HAXSOCKPEER     result          = nil;

    ENTER(true);

    if ((result = (HAXSOCKPEER)CREATE(AXSOCKPEER)) != nil)
    {
        if (!axsocket_peer_copy(result, h_source))
        {
            result = axsocket_peer_destroy(result);
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      a7socket_peer_create
// PURPOSE
//
// PARAMETERS
//      PSTR         psz_address --
//      UINT         u_port      --
// RESULT
//      HAXSOCKPEER    --
// ***************************************************************************
HAXSOCKPEER axsocket_peer_create(PSTR psz_address, UINT u_port)
{
    PAXSOCKPEER        pst_peer        = NULL;


    if ((pst_peer = CREATE(AXSOCKPEER)) != NULL)
    {
        if (psz_address)
        {
            if (!axsocket_peer_set((HAXSOCKPEER)pst_peer, psz_address, u_port))
                pst_peer = (PAXSOCKPEER)axsocket_peer_destroy((HAXSOCKPEER)pst_peer);
        }
        else
        {
            pst_peer->st_addr.sin_family    = AF_INET;
            pst_peer->st_addr.sin_port      = htons((U16)u_port);
        }
    }

    return (HAXSOCKPEER)pst_peer;
}
// ***************************************************************************
// FUNCTION
//      axsocket_get_peer_desc
// PURPOSE
//
// PARAMETERS
//      HAXSOCKET h_socket --
//      PSTR       psz_name --
//      UINT       u_len    --
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
//BOOL axsocket_get_peer_desc(HAXSOCKET h_socket,
//                            PSTR      psz_name,
//                            UINT      u_len)
//{
//    BOOL                b_result        = false;
//    PAXSOCKET           pst_socket      = (PAXSOCKET)h_socket;
//
//    if (pst_socket)
//    {
//        if (*pst_socket->sz_peer)
//        {
//            strz_sformat(psz_name, u_len, "%s:%d", pst_socket->sz_peer, pst_socket->w_peer);
//            b_result    = true;
//        }
//    }
//
//    return b_result;
//}
// ***************************************************************************
// FUNCTION
//      axsocket_peer_get_addr
// PURPOSE
//
// PARAMETERS
//      HAXSOCKPEER h_peer      --
//      PSTR         psz_address --
//      UINT         u_len       --
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
BOOL axsocket_peer_get_addr(HAXSOCKPEER    h_peer,
                            PVOID           p_address,
                            UINT            u_len)
{
    BOOL            b_result    = false;
    UINT            u_size;
    PAXSOCKPEER    pst_peer    = (PAXSOCKPEER)h_peer;


    if (pst_peer)
    {
        u_size = sizeof(pst_peer->st_addr.sin_addr);

        if (u_len >= u_size)
        {
            memcpy(p_address, &pst_peer->st_addr.sin_addr, u_size);
            b_result = true;
        }
    }

    return b_result;
}

// ***************************************************************************
// FUNCTION
//      la6_sockets_peer_get_addr
// PURPOSE
//
// PARAMETERS
//      HAXSOCKPEER h_peer      --
//      PSTR         psz_address --
//      UINT         u_len       --
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
BOOL axsocket_peer_get_addr_str(HAXSOCKPEER h_peer,
                                PSTR        psz_address,
                                UINT        u_len)
{
    BOOL            b_result    = false;
    PSTR            psz_tmp;
    PAXSOCKPEER    pst_peer    = (PAXSOCKPEER)h_peer;


    if (pst_peer)
    {
        if ((psz_tmp = inet_ntoa(pst_peer->st_addr.sin_addr)) != NULL)
        {
            strz_cpy(psz_address, psz_tmp, u_len);
            b_result = true;
        }
    }

    return b_result;
}

// ***************************************************************************
// FUNCTION
//      la6_sockets_peer_get_port
// PURPOSE
//
// PARAMETERS
//      HAXSOCKPEER h_peer  --
//      PUINT        pu_port --
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
BOOL axsocket_peer_get_port(HAXSOCKPEER h_peer,
                            PUINT       pu_port)
{
    BOOL            b_result    = false;
    PAXSOCKPEER    pst_peer    = (PAXSOCKPEER)h_peer;


    if (pst_peer && pu_port)
    {
        *pu_port    = ntohs((u_short)pst_peer->st_addr.sin_port);
        b_result    = true;
    }

    return b_result;
}

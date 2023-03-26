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
#include <lwip_sock_compat.h>


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
            pst_peer->addr      = INADDR_ANY;
            pst_peer->port      = u_port;
        }
    }

    return (HAXSOCKPEER)pst_peer;
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
    PAXSOCKPEER         pst_peer        = (PAXSOCKPEER)h_peer;

    if (pst_peer && psz_address && u_port)
    {
        pst_peer->addr  = strz_to_ip(psz_address);
        pst_peer->port  = u_port;
        b_result        = true;
    }

    return b_result;
}
BOOL axsocket_peer_get_addr_str  (HAXSOCKPEER        h_peer,
                                  PSTR               psz_address,
                                  UINT               u_len)
{
    BOOL                result          = false;
    PAXSOCKPEER         pst_peer        = (PAXSOCKPEER)h_peer;

    ENTER(true);

    if (pst_peer && psz_address && u_len)
    {
        U32  a  = pst_peer->addr;
        result  = true;

        strz_sformat(psz_address, u_len, "%d.%d.%d.%d",
                     (a     ) & 0xFF,
                     (a >> 8) & 0xFF,
                     (a >> 16) & 0xFF,
                     (a >> 24) & 0xFF);

    }

    RETURN(result);
}
BOOL axsocket_peer_get_port      (HAXSOCKPEER        h_peer,
                                  PUINT              pu_port)
{
    BOOL                result          = false;
    PAXSOCKPEER         pst_peer        = (PAXSOCKPEER)h_peer;

    ENTER(true);

    if (pst_peer && pu_port)
    {
        *pu_port = pst_peer->port;
        result   = true;
    }

    RETURN(result);
}
BOOL axsocket_peer_set_port      (HAXSOCKPEER        h_peer,
                                  UINT               u_port)
{
    BOOL                result          = false;
    PAXSOCKPEER         pst_peer        = (PAXSOCKPEER)h_peer;

    ENTER(true);

    if (pst_peer && u_port)
    {
        pst_peer->port = u_port;
        result         = true;
    }

    RETURN(result);
}

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
    PAXSOCKPEER         pst_result      = nil;
    PAXSOCKPEER         pst_source      = (PAXSOCKPEER)h_source;

    ENTER(true);

    if ((pst_result = CREATE(AXSOCKPEER)) != NULL)
    {
        memcpy(pst_result, pst_source, sizeof(AXSOCKPEER));
    }

    RETURN((HAXSOCKPEER)pst_result);
}

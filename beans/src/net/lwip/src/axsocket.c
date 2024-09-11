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
#include <lwip/dhcp.h>
#include <lwip/prot/dhcp.h>

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct _tag_PARAMHOLDER
{
        PVOID               userptr;
        PFNAXUSOCKRECVCB    cb;
} PARAMHOLDER, * PPARAMHOLDER;

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

struct netif gnetif;
ip4_addr_t ipaddr;
ip4_addr_t netmask;
ip4_addr_t gw;

extern err_t ethernetif_init(struct netif *netif);

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

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
    tcpip_init( NULL, NULL );

    /* IP addresses initialization with DHCP (IPv4) */
    ipaddr.addr = 0;
    netmask.addr = 0;
    gw.addr = 0;

    /* add the network interface (IPv4/IPv6) with RTOS */
    netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

    /* Registers the default network interface */
    netif_set_default(&gnetif);
    
    if (netif_is_link_up(&gnetif))
    {
        /* When the netif is fully configured this function must be called */
        netif_set_up(&gnetif);
    }
    else
    {
        /* When the netif link is down this function must be called */
        netif_set_down(&gnetif);
    }

    /* Start DHCP negotiation for a network interface (IPv4) */
    dhcp_start(&gnetif);

    return true;
}
BOOL axsocket_start_static(PCSTR        addressStr, 
                           PCSTR        netmaskStr, 
                           PCSTR        gatewayStr)
{
    tcpip_init( NULL, NULL );

    /* IP addresses initialization with DHCP (IPv4) */
    ipaddr.addr = strz_to_ip(addressStr);
    netmask.addr = strz_to_ip(netmaskStr);
    gw.addr = strz_to_ip(gatewayStr);

    /* add the network interface (IPv4/IPv6) with RTOS */
    netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

    /* Registers the default network interface */
    netif_set_default(&gnetif);
    
    if (netif_is_link_up(&gnetif))
    {
        /* When the netif is fully configured this function must be called */
        netif_set_up(&gnetif);
    }
    else
    {
        /* When the netif link is down this function must be called */
        netif_set_down(&gnetif);
    }

    return true;
}
BOOL axsocket_host_get_ip(PVOID buff,
                          UINT index)
{
    BOOL            result          = false;
    struct dhcp *   dhcp;

    ENTER(true);

    if (!index)
    {
        dhcp = netif_dhcp_data(&gnetif);

        if (dhcp->state == DHCP_STATE_BOUND)
        {
            if (gnetif.ip_addr.addr != 0)
            {
                memcpy(buff, &gnetif.ip_addr.addr, sizeof(gnetif.ip_addr.addr));
                result = true;
            }
        }
    }

    RETURN(result);
}

static void udpecho_raw_recv(PPARAMHOLDER       params,
                             struct udp_pcb *   pcb,
                             struct pbuf *      p,
                             const ip_addr_t *  addr,
                             u16_t              port)
{
    AXSOCKPEER  peer;

    if(p != NULL)
    {
        peer.addr = addr->addr;
        peer.port = port;
//        U32 a = addr->addr;
//        sprintf(ipBuff, "%d.%d.%d.%d", (a >> 0) & 0xFF, (a >> 8) & 0xFF, (a >> 16) & 0xFF, (a >> 24) & 0xFF);
//        memcpy(buff, p->payload, p->tot_len);
//        *(buff + p->tot_len) = 0;
//        printf("GOT: from %s:%d '\n", ipBuff, port);
//        strz_dump("GOT: ", p->payload, p->tot_len);

//        processPacket(pcb, addr, port, (PRLOGMSG )p->payload, p->tot_len);

        (*params->cb)(params->userptr, (HAXSOCKET)pcb, p->payload, p->tot_len, (HAXSOCKPEER)&peer);

        pbuf_free(p);
    }
}

HAXHANDLE axusocket_recv_handler_set(HAXSOCKET          h_socket,
                                     PVOID              userptr,
                                     PFNAXUSOCKRECVCB   cb)
{
    HAXHANDLE           result          = nil;
    PPARAMHOLDER        p;


    ENTER(true);

    if ((p = CREATE(PARAMHOLDER)) != nil)
    {
        p->userptr  = userptr;
        p->cb       = cb;
        udp_recv((struct udp_pcb*)h_socket, (udp_recv_fn)udpecho_raw_recv, p);

        result = udpecho_raw_recv;
    }

    RETURN(result);
}


HAXSOCKET axusocket_open(PSTR               psz_host,
                         UINT               u_port,
                         UINT               d_flags,
                         UINT               u_timeout_msecs)
{
    struct udp_pcb *    result          = nil;

    ENTER(true);

    if ((result = udp_new()) != nil)
    {
        if (udp_bind(result, IP_ADDR_ANY, u_port) != ERR_OK)
        {

        }
    }

    RETURN((HAXSOCKET)result);
}
HAXSOCKBLK axsockblk_alloc(UINT               size)
{
    struct pbuf *   result          = nil;

    ENTER(true);

    result = pbuf_alloc(PBUF_TRANSPORT, size, PBUF_RAM);

    RETURN((HAXSOCKBLK)result);
}
HAXSOCKBLK axsockblk_free(HAXSOCKBLK        data)
{

    if (data != nil)
    {
        pbuf_free((struct pbuf *)data);
    }

    return nil;
}
PVOID axsockblk_payload(HAXSOCKBLK        data)
{
    struct pbuf * buf = (struct pbuf *)data;

    return buf->payload;
}
INT axusocket_send_blk(HAXSOCKET          h_socket,
                       HAXSOCKPEER        peer,
                       HAXSOCKBLK         block,
                       UINT               TO)
{
    PAXSOCKPEER     p = (PAXSOCKPEER)peer;
    struct pbuf *   b = (struct pbuf *)block;
    ip_addr_t       addr;

    addr.addr = p->addr;

    udp_sendto((struct udp_pcb*)h_socket, b, &addr, (u16_t)p->port);

    return b->tot_len;
}
HAXSOCKET axusocket_close(HAXSOCKET          h_socket)
{
    udp_remove((struct udp_pcb *)h_socket);

    return nil;
}
INT axusocket_recv(HAXSOCKET          h_socket,
                   HAXSOCKPEER        h_peer,
                   PVOID              p_target,
                   UINT               u_size,
                   UINT               u_timeout_msecs)
{
    INT         result          = 0;

    ENTER(true);

    RETURN(result);
}
INT axusocket_send(HAXSOCKET          h_socket,
                   HAXSOCKPEER        h_peer,
                   PVOID              p_data,
                   UINT               u_size,
                   UINT               u_timeout_msecs)
{
    INT         result          = false;
    HAXSOCKBLK  blk;

    ENTER(true);

    if ((blk = axsockblk_alloc(u_size)) != nil)
    {
        memcpy(axsockblk_payload(blk), p_data, u_size);

        result = axusocket_send_blk(h_socket, h_peer, blk, u_timeout_msecs);

        axsockblk_free(blk);
    }

    RETURN(result);
}
BOOL axusocket_bcast_set(HAXSOCKET          h_sock,
                         BOOL               ena)
{
    if (ena)
        ip_set_option((struct udp_pcb *)h_sock, SOF_BROADCAST);
    else
        ip_reset_option((struct udp_pcb *)h_sock, SOF_BROADCAST);

    return true;
}
void axsocket_set_link_up(int index)
{
    netif_set_link_up(&gnetif);
    dhcp_start(&gnetif);
}
void axsocket_set_link_down(int index)
{
    netif_set_link_down(&gnetif);
    dhcp_stop(&gnetif);
}

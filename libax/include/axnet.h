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

#ifndef __AXNETH__
#define __AXNETH__

#include <arsenal.h>
#include <axsystem.h>
#include <axdata.h>
#include <axstring.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define PORT_TELNET         23

// ---------------------------- Socket flags ---------------------------------

#define DEFSO_REUSE_ADDR        0x00000010


// --------------------------- Socket options --------------------------------

#define DEFSO_MAX_MSG_SIZE      5
#define DEFSO_SNDBUF            7
#define DEFSO_RCVBUF            8

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct __tag_HAXSOCKPEER { UINT u_id; } * HAXSOCKPEER;
typedef struct __tag_HAXSOCKET   { UINT u_id; } * HAXSOCKET;
typedef struct __tag_HAXSOCKDATA { UINT u_id; } * HAXSOCKBLK;

typedef struct _tag_TELNET
{
    PAXDEV          pst_dev;
    PAXBUFF         pst_buff;
} TELNET, * PTELNET;

typedef struct _tag_AXURN
{
    UINT            port;
    CHAR            proto           [ AXLSHORT  ];
    CHAR            name            [ AXLMEDIUM ];
} AXURN, * PAXURN;

typedef void (*PFNAXUSOCKRECVCB)(PVOID        userptr,
                                 HAXSOCKET    sock,
                                 PVOID        msg,
                                 UINT         size,
                                 HAXSOCKPEER  peer);




// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif

void                axsocket_stop               (void);

BOOL                axsocket_start              (void);

UINT                axsocket_error              (void);

BOOL                axsocket_host_get_ip        (PVOID              addr,
                                                 UINT               index);


// ---------------------------

HAXSOCKPEER         axsocket_peer_destroy       (HAXSOCKPEER        h_peer);

BOOL                axsocket_peer_set           (HAXSOCKPEER        h_peer,
                                                 PSTR               psz_address,
                                                 UINT               u_port);

BOOL                axsocket_peer_set_port      (HAXSOCKPEER        h_peer,
                                                 UINT               u_port);

BOOL                axsocket_peer_copy          (HAXSOCKPEER        h_targte,
                                                 HAXSOCKPEER        h_source);

HAXSOCKPEER         axsocket_peer_dup           (HAXSOCKPEER        h_source);


HAXSOCKPEER         axsocket_peer_create        (PSTR               psz_address,
                                                 UINT               u_port);

BOOL                axsocket_peer_get_addr      (HAXSOCKPEER        h_peer,
                                                 PVOID              p_address,
                                                 UINT               u_len);

BOOL                axsocket_peer_get_addr_str  (HAXSOCKPEER        h_peer,
                                                 PSTR               psz_address,
                                                 UINT               u_len);

BOOL                axsocket_peer_get_port      (HAXSOCKPEER        h_peer,
                                                 PUINT              pu_port);

//BOOL                axsocket_get_peer_desc      (HAXSOCKET          h_socket,
//                                                 PSTR               psz_name,
//                                                 UINT               u_len);

// ---------------------------

HAXSOCKET           axusocket_close             (HAXSOCKET          h_socket);

HAXSOCKET           axusocket_open              (PSTR               psz_host,
                                                 UINT               u_port,
                                                 UINT               d_flags,
                                                 UINT               u_timeout_msecs);

INT                 axusocket_recv              (HAXSOCKET          h_socket,
                                                 HAXSOCKPEER        h_peer,
                                                 PVOID              p_target,
                                                 UINT               u_size,
                                                 UINT               u_timeout_msecs);

INT                 axusocket_send              (HAXSOCKET          h_socket,
                                                 HAXSOCKPEER        h_peer,
                                                 PVOID              p_data,
                                                 UINT               u_size,
                                                 UINT               u_timeout_msecs);

BOOL                axusocket_bcast_set         (HAXSOCKET          h_sock,
                                                 BOOL               ena);

// ------------- special for LwIp

HAXHANDLE           axusocket_recv_handler_set  (HAXSOCKET          h_socket,
                                                 PVOID              userptr,
                                                 PFNAXUSOCKRECVCB   cb);

HAXSOCKBLK          axsockblk_alloc             (UINT               size);

HAXSOCKBLK          axsockblk_free              (HAXSOCKBLK         block);

PVOID               axsockblk_payload           (HAXSOCKBLK         block);

INT                 axusocket_send_blk          (HAXSOCKET          h_socket,
                                                 HAXSOCKPEER        peer,
                                                 HAXSOCKBLK         block,
                                                 UINT               TO);

// -------------


//BOOL                axusocket_ex




PAXDEV              axssocket_close             (PAXDEV             h_dev);

PAXDEV              axssocket_listen            (PSTR               psz_iface,
                                                 UINT               u_port,
                                                 UINT               u_flags);

PAXDEV              axssocket_listen_any        (PSTR               psz_iface,
                                                 UINT               u_low,
                                                 UINT               u_high);

PAXDEV              axssocket_accept            (PAXDEV             h_listen,
                                                 UINT               u_TO);

UINT                axssocket_get_port          (PAXDEV             h_dev);

PAXDEV              axssocket_connect           (PCSTR              psz_host,
                                                 UINT               u_port,
                                                 PCSTR              psz_iface,
                                                 UINT               u_highest,
                                                 UINT               u_TO);

BOOL                axssocket_peer_get          (PAXDEV             h_dev,
                                                 PSTR               psz_name,
                                                 UINT               u_len,
                                                 PUINT              pu_port);

BOOL                axssocket_peer_get_s        (PAXDEV             h_dev,
                                                 PSTR               psz_name,
                                                 UINT               u_len);

BOOL                axssocket_is_connected      (PAXDEV             h_dev);

void                axsocket_set_link_up        (int                index);

void                axsocket_set_link_down      (int                index);


/*
PTELNET             telnet_destroy              (PTELNET            pst_t);


PTELNET             telnet_create               (PAXDEV             pst_dev,
                                                 UINT               u_buff_size);

BOOL                telnet_negotiate            (PTELNET            pst_t,
                                                 UINT               u_TO);

BOOL                telnet_login                (PTELNET            pst_t,
                                                 PCSTR              user,
                                                 PCSTR              passwd,
                                                 UINT               TO);

BOOL                telnet_wait                 (PTELNET            pst_t,
                                                 PCSTR              psz_wait_for,
                                                 UINT               u_TO);

INT                 telnet_printf               (PTELNET            pst_t,
                                                 UINT               u_TO,
                                                 PCSTR              psz_message,
                                                 ...);

PSTR                telnet_gets                 (PTELNET            pst_t,
                                                 UINT               u_TO);














HAXSOCKPEER         axsocket_peer_destroy       (HAXSOCKPEER            h_peer);

HAXSOCKPEER         axsocket_peer_create        (PSTR                   psz_address,
                                                 UINT                   u_port);

BOOL                la6_sockets_peer_set        (HAXSOCKPEER            h_peer,
                                                 PSTR                   psz_address,
                                                 UINT                   u_port);

BOOL                la6_sockets_peer_set2       (HAXSOCKPEER           h_target,
                                                 HAXSOCKPEER           h_source);

BOOL                la6_sockets_peer_get_addr   (HAXSOCKPEER           h_peer,
                                                 PSTR                   psz_address,
                                                 UINT                   u_len);

BOOL                la6_sockets_peer_get_addr2  (HAXSOCKPEER           h_peer,
                                                 PVOID                  p_address,
                                                 UINT                   u_len);

BOOL                la6_sockets_peer_get_port   (HAXSOCKPEER           h_peer,
                                                 PUINT                  pu_port);

BOOL                axsocket_get_opt            (HAXSOCKET             h_socket,
                                                 UINT                   u_cmd,
                                                 PVOID                  p_value,
                                                 UINT                   u_size);

BOOL                la6_sockets_set_opt         (HAXSOCKET             h_socket,
                                                 UINT                   u_cmd,
                                                 PVOID                  p_value,
                                                 UINT                   u_size);

BOOL                la6_sockets_get_peer2       (HAXSOCKET             h_socket,
                                                 PSTR                   psz_name,
                                                 UINT                   u_len);

// ------------------------- net/?/streamsock.c ------------------------------

HAXSOCKET          la6_ssocket_close           (HAXSOCKET             h_socket);

HAXSOCKET          la6_ssocket_shutdown        (HAXSOCKET             h_socket);

HAXSOCKET          la6_ssocket_connect         (PSTR                   psz_host,
                                                 UINT                   u_port,
                                                 PSTR                   psz_iface,
                                                 UINT                   u_highest_bind_port,
                                                 UINT                   u_timeout_msec);

BOOL                la6_ssocket_is_connected    (HAXSOCKET             h_socket);

HAXSOCKET          la6_ssocket_listen          (PSTR                   psz_iface,
                                                 UINT                   u_port);

HAXSOCKET          la6_ssocket_listen_any      (PSTR                   psz_iface,
                                                 UINT                   u_highest_port);

HAXSOCKET          la6_ssocket_accept          (HAXSOCKET             h_socket,
                                                 UINT                   u_timeout_msec);

INT                 la6_ssocket_recv            (HAXSOCKET             h_socket,
                                                 PVOID                  p_target,
                                                 INT                    i_size,
                                                 UINT                   u_timeout_msec);

INT                 la6_ssocket_recv_block      (HAXSOCKET             h_socket,
                                                 PVOID                  p_target,
                                                 INT                    i_size,
                                                 UINT                   u_timeout_msec);

INT                 la6_ssocket_peek            (HAXSOCKET             h_socket,
                                                 PVOID                  p_target,
                                                 INT                    i_size);

INT                 la6_ssocket_wait            (HAXSOCKET             h_socket,
                                                 UINT                   u_timeout_msec);

INT                 la6_ssocket_send            (HAXSOCKET             h_socket,
                                                 PVOID                  p_data,
                                                 INT                    i_size,
                                                 UINT                   u_timeout_msec);


BOOL                la6_inet_ntoa               (PSTR                   psz_buff,
                                                 UINT                   u_size,
                                                 UINT32                 u_addr);

UINT32              la6_inet_addr               (PSTR                   psz_addr);

UINT                la6_ssocket_get_bound_port  (HAXSOCKET             h_socket);


// --------------------------- net/?/dgsock.c --------------------------------

INT                 axsocket_recv_from           (HAXSOCKET         h_socket,
                                                 HAXSOCKPEER        h_peer,
                                                 PVOID              p_target,
                                                 UINT               u_size,
                                                 UINT               u_timeout_msecs);

INT                 axsocket_send_to           (HAXSOCKET           h_socket,
                                                 HAXSOCKPEER        h_peer,
                                                 PVOID              p_data,
                                                 UINT               u_size,
                                                 UINT               u_timeout_msecs);

BOOL                axsocket_mcast_set_group    (HAXSOCKET          h_sock,
                                                 PSTR               psz_group);

BOOL                axsocket_bcast_set          (HAXSOCKET          h_sock);

BOOL                axsocket_mcast_set_ttl      (HAXSOCKET          h_sock,
                                                 UINT               u_ttl);

BOOL                axsocket_mcast_set_iface    (HAXSOCKET          h_socket,
                                                 PSTR               psz_iface);

BOOL                axsocket_mcast_set_noloop   (HAXSOCKET          h_socket);

BOOL                axsocket_ip_is_mcast        (PSTR               psz_addr);


BOOL                la6_sockets_host_get_name   (PSTR               psz_target,
                                                 UINT               d_target_size);

BOOL                la6_sockets_host_get_addr   (PSTR               psz_addr,
                                                 UINT               d_target_size);

BOOL                axsocket_get_iface_ip       (PSTR               addr,
                                                 UINT               lenght,
                                                 PSTR               iface);

BOOL                axsocket_get_addr           (PSTR               psz_addr,
                                                 UINT               d_target_size,
                                                 PCSTR              name);
*/
#ifdef __cplusplus
}
#endif

#endif                                      //  #define __AXNETH__

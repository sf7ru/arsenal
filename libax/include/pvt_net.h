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

#ifndef __PVTAXNETH__
#define __PVTAXNETH__

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
#include <fcntl.h>
#elif (TARGET_FAMILY == __AX_rtos__)
#include <lwip_sock_compat.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
//#if (_MSC_VER >= 1300)
#include <winsock2.h>
//#endif
#include <ws2tcpip.h>
#endif

#include <axnet.h>

// ---------------------------------------------------------------------------
// ----------------------------- DEFINITIONS ---------------------------------
// ---------------------------------------------------------------------------

#define DEF_BIND_TRIES          3

#define DEF_DEFAULT_TIMEOUT     10000
#define DEF_TIMEOUT_QUANT       10

#define DEF_TIMEOUT             700
#define SO_MAX_MSG_SIZE         0x2003      // maximum message size

#if (TARGET_FAMILY != __AX_win__)
#define SOCKET                  int
#define SOCKET_ERROR            -1
#define INVALID_SOCKET          -1

#define closesocket             close
#define ioctlsocket             ioctl

#define SD_RECEIVE              0x00
#define SD_SEND                 0x01
#define SD_BOTH                 0x02
#endif

// For _axsocket_select
#define SELECT_READ             1
#define SELECT_WRITE            2
#define SELECT_EXCEPT           4



// ---------------------------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// -------------------------------- TYPES ------------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// STRUCTURE
//      SSOCKDEV
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_SSOCKDEV
{
    AXDEV               st_dev;
    SOCKET              v_sock;
    UINT                u_port;
    struct sockaddr_in  st_peer_addr;
} SSOCKDEV, * PSSOCKDEV;

// ***************************************************************************
// STRUCTURE
//      SSOCKDEV
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_AXSOCKET
{
    UINT                d_obj_id;
    SOCKET              i_socket;
    UINT                w_peer;
    UINT                u_bound;
    struct sockaddr_in  st_sa;
    fd_set              st_fds;
//    CHAR                sz_peer     [ 32 ];
} AXSOCKET, * PAXSOCKET;

// ***************************************************************************
// STRUCTURE
//      AXSOCKPEER
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_AXSOCKPEER
{
    struct sockaddr_in  st_addr;
} AXSOCKPEER, * PAXSOCKPEER;

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif
BOOL                _axsocket_addr_set          (struct sockaddr_in * pst_sa,
                                                 PCSTR              psz_iface,
                                                 UINT               u_port);

UINT                _axsocket_bind              (SOCKET             v_socket,
                                                 PCSTR              psz_iface,
                                                 UINT               u_low,
                                                 UINT               u_high);

BOOL                _axsocket_is_connected      (SOCKET             v_sock);


BOOL                _axssocket_connect_nb       (PSSOCKDEV          pst_dev,
                                                 struct sockaddr *  pst_sa,
                                                 UINT               u_sa_size,
                                                 UINT               u_TO_msec);

INT                 _axsocket_select            (INT                i_socket,
                                                 INT                mode,
                                                 UINT               TO);

#ifdef __cplusplus
}
#endif

#endif                                      //  #define __PVTAXNETH__

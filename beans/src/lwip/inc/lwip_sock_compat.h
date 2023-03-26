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

#ifndef __lwip_sock_compat_H__
#define __lwip_sock_compat_H__


// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

/* Address to accept any incoming messages.  */
#define INADDR_ANY              ((in_addr_t) 0x00000000)
/* Address to send to all hosts.  */
#define INADDR_BROADCAST        ((in_addr_t) 0xffffffff)
/* Address indicating an error return.  */
#define INADDR_NONE             ((in_addr_t) 0xffffffff)

#define AF_INET         2       /* IP protocol family.  */


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef PVOID               SOCKET;
typedef unsigned short      u_short;

typedef uint32_t in_addr_t;
struct in_addr
{
    in_addr_t s_addr;
};

// ***************************************************************************
// STRUCTURE
//      AXSOCKPEER
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_AXSOCKPEER
{
    U32         addr;
    UINT        port;
} AXSOCKPEER, * PAXSOCKPEER;


//struct in_addr {
//    unsigned long s_addr;  // load with inet_aton()
//};

struct sockaddr_in {
    short            sin_family;   // e.g. AF_INET
    unsigned short   sin_port;     // e.g. htons(3490)
    struct in_addr   sin_addr;     // see struct in_addr, below
    char             sin_zero[8];  // zero this if you want to
};

//typedef struct fd_set {
//    unsigned int  fd_count;
////    SOCKET fd_array[FD_SETSIZE];
//} fd_set, FD_SET, *PFD_SET, *LPFD_SET;

typedef struct hostent {
    char  *h_name;
    char  **h_aliases;
    short h_addrtype;
    short h_length;
    char  *h_addr;
} HOSTENT, *PHOSTENT, *LPHOSTENT;

#endif // #define __lwip_sock_compat_H__

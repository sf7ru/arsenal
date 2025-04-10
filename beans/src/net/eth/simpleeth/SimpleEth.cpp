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

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>

#include <time.h>

#include <pvt_ax.h>
#include <string.h>
#include <stdio.h>

#include <axstring.h>

#include <SimpleEth.h>


// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      SimpleEth::SimpleEth
// PURPOSE
//      Class construction
// PARAMETERS
//        void -- None
// ***************************************************************************
SimpleEth::SimpleEth(void)
{
    sock            = -1;
    ifindex         = -1;
    *ifmacString    = 0;
    proto           = htons(ETH_P_ALL);
}
// ***************************************************************************
// TYPE
//      Destructor
// FUNCTION
//      SimpleEth::~SimpleEth
// PURPOSE
//      Class destruction
// PARAMETERS
//        void -- None
// ***************************************************************************
SimpleEth::~SimpleEth(void)
{
    struct ifreq    rq;

    if (sock != -1)
    {
        if (ioctl(sock, SIOCGIFFLAGS, &rq) != -1)
        {
            rq.ifr_flags &= (~IFF_PROMISC);
            ioctl(sock, SIOCSIFFLAGS, &rq);
        }

        close(sock);
    }
}
// ***************************************************************************
// FUNCTION
//      SimpleEth::determineIndex
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SimpleEth::determineIndex()
{
    BOOL            result          = false;
    struct ifreq    ifr;

    ENTER(true);

    strz_cpy((PSTR)ifr.ifr_name, iface, IFNAMSIZ);

    if (ioctl(sock, SIOCGIFINDEX, &ifr) != -1)
    {
        ifindex = ifr.ifr_ifindex;
        result  = true;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SimpleEth::determineMac
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SimpleEth::determineMac()
{
    BOOL            result          = false;
    struct ifreq    ifr;

    ENTER(true);

    strz_cpy((PSTR)ifr.ifr_name, iface, IFNAMSIZ);

    if (ioctl(sock, SIOCGIFHWADDR, &ifr) != -1)
    {
        memcpy(ifmac, ifr.ifr_hwaddr.sa_data, 6);

        snprintf(ifmacString, AXLSHORT, "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",
                 ifmac[0], ifmac[1], ifmac[2], ifmac[3], ifmac[4], ifmac[5]);



        result  = true;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SimpleEth::bindToIface
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SimpleEth::bindToIface()
{
    BOOL                result          = false;
    struct sockaddr_ll  sll;

    ENTER(true);

    memset(&sll, 0, sizeof(struct sockaddr_ll));

    sll.sll_family      = AF_PACKET;
    sll.sll_ifindex     = ifindex;
    sll.sll_protocol    = proto;

    result = (bind(sock, (struct sockaddr *)&sll, sizeof(sll)) != -1);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SimpleEth::setPromisc
// PURPOSE
//
// PARAMETERS
//      BOOL enable --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SimpleEth::setPromisc(BOOL enable)
{
    BOOL                result          = false;
    struct packet_mreq  mr;

    ENTER(true);

    memset(&mr, 0, sizeof (mr));
    mr.mr_ifindex   = ifindex;
    mr.mr_type      = PACKET_MR_PROMISC;

    result = (setsockopt(sock, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) != -1);

    RETURN(result);
}
BOOL SimpleEth::setRecvBuffSize(int size)
{
    BOOL                result          = false;
    UINT                res;
    UINT                ressz;

    ENTER(true);

    printf("SimpleEth::setRecvBuffSize = %d\n", size);

    result = (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size)) != -1);

    ressz = sizeof(res);
    if (getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &res, &ressz) != -1)
    {
        printf("SimpleEth::getRecvBuffSize = %d\n", res);
    }
    else
        perror("get recv buff");

    RETURN(result);
}
BOOL SimpleEth::setSendBuffSize(int size)
{
    BOOL                result          = false;
    UINT                res;
    UINT                ressz;

    ENTER(true);

    //printf("SimpleEth::setSendBuffSize = %d\n", size);

    result = (setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size)) != -1);

    ressz = sizeof(res);
    if (getsockopt(sock, SOL_SOCKET, SO_SNDBUF, &res, &ressz) != -1)
    {
        // printf("SimpleEth::getSendBuffSize = %d\n", res);
    }
    else
        perror("get send buff");

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SimpleEth::create
// PURPOSE
//
// PARAMETERS
//      PCSTR device --
// RESULT
//      SimpleEth * --
// ***************************************************************************
SimpleEth * SimpleEth::create(PCSTR          device,
                  UINT           recvBuffSize,
                  UINT           sendBuffSize)
{
    SimpleEth *               result      = NULL;

    ENTER(true);

    if ((result = new SimpleEth) != NULL)
    {
        strz_cpy(result->iface, device, 17);

        if ( !( ((result->sock  = socket(AF_PACKET,
                       SOCK_RAW, result->proto)) != -1)     &&
                  result->determineIndex()                  &&
                  result->determineMac()                    &&
                  result->bindToIface()                     &&
                  result->setPromisc(true)                  &&
                  (!sendBuffSize ||
                    result->setSendBuffSize(sendBuffSize))  &&
                  (!recvBuffSize ||
                    result->setRecvBuffSize(recvBuffSize))  ))
        {
            perror(device);

            delete result;
            result = NULL;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SimpleEth::recv
// PURPOSE
//
// PARAMETERS
//      PVOID buffer --
//      INT   size   --
// RESULT
//      int --
// ***************************************************************************
int SimpleEth::recv(PVOID     buffer,
              INT       size,
              UINT      TO)
{
    int             result          = -1;

    ENTER(true);

    result = _ax_nonblock_dev_read(sock, buffer, size, TO);
    //result = read(sock, buffer, size);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SimpleEth::send
// PURPOSE
//
// PARAMETERS
//      PVOID buffer --
//      INT   size   --
// RESULT
//      int --
// ***************************************************************************
int SimpleEth::send(PVOID     buffer,
              INT       size)
{
    int                 result          = -1;

    ENTER(true);

    if ((result = ::send(sock, buffer, size, MSG_NOSIGNAL)) <= 0)
    {
//        perror("send error: ");
    }
//    else
//    {
//        printf("%d bytes ", size);
//        la6_dump_print("SENT ", buffer, size);
//    }


//    if ((result = sendto(sock, buffer, size, MSG_NOSIGNAL | MSG_DONTROUTE,
//                         //(struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)
//                         NULL, 0
//                         )) < 0)
//    {
//        perror("send error: ");
//    }

    RETURN(result);
}

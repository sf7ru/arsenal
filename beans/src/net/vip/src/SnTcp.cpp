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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

//#include <linux/ip.h>
//#include <netinet/tcp.h>
//#include <arpa/inet.h>

//#include <a7utils.h>

#include <SnTcp.h>
#include <SnIp.h>

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

#pragma pack (1)
typedef AXPACKED(struct) __PSEUDO
{
    U32         saddr;
    U32         daddr;
    U8          zero;
    U8          proto;
    U16         size;
} PSEUDO;
#pragma pack ()

#pragma pack (1)
typedef AXPACKED(struct) __SAKBLOCK
{
    U32         left;
    U32         right;
} SAKBLOCK, * PSAKBLOCK;
#pragma pack ()

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

static PCSTR _getStateName(UINT state)
{
#define CH(a) case TCP_##a: result = #a; break;

    PCSTR       result;

    switch (state)
    {
        CH(ESTABLISHED);
        CH(SYN_SENT);
        CH(SYN_RECV);
        CH(FIN_WAIT1);
        CH(FIN_WAIT2);
        CH(TIME_WAIT);
        CH(CLOSE);
        CH(CLOSE_WAIT);
        CH(LAST_ACK);
        CH(LISTEN);
        CH(CLOSING);
        CH(CLOSED);

        default:
            result = "UNKNOWN";
            break;
    }

    return result;
}
void SnTcp::_showPkt(PSNPACKET pkt)
{
//    return ;

    struct tcphdr * h           =  HEADERTCP(pkt);

    printf("%6u -> %6u: SEQ=%u{%.8X} ",
           ntohs(h->source),
           ntohs(h->dest),
           ntohl(h->seq), ntohl(h->seq)
           );

    if (h->ack)
        printf("ACK=%u{%.8X} ", ntohl(h->ack_seq), ntohl(h->ack_seq));

    if (h->syn)
        printf("SYN ");

    if (h->fin)
        printf("FIN ");

    if (h->urg)
        printf("URG=%u ", ntohs(h->urg_ptr));

    if (h->psh)
        printf("PSH ");

    if (h->rst)
        printf("RST ");

    printf("[doff=%u win=%u] ", h->doff, ntohs(h->window));

/*
    printf(
    "u_int16_t h->source      = %u\n"
    "u_int16_t h->dest        = %u\n"
    "u_int32_t h->seq;        = %u\n"
    "u_int32_t h->ack_seq     = %u\n"
    "u_int16_t h->doff        = %u\n"
    "u_int16_t h->urg         = %u\n"
    "u_int16_t h->ack         = %u\n"
    "u_int16_t h->psh         = %u\n"
    "u_int16_t h->rst         = %u\n"
    "u_int16_t h->syn         = %u\n"
    "u_int16_t h->fin         = %u\n"
    "u_int16_t h->window      = %u\n"
    "u_int16_t h->check       = %u\n"
    "u_int16_t h->urg_ptr     = %u\n"
    "%s",
    ntohs(h->source),
    ntohs(h->dest),
    ntohl(h->seq),
    ntohl(h->ack_seq),
    h->doff,
    h->urg,
    h->ack,
    h->psh,
    h->rst,
    h->syn,
    h->fin,
    h->window,
    h->check,
    h->urg_ptr,

    ""
    );
*/
    if (h->doff > TCP_HEADER_SZ)
    {
        PU8 on    = (PU8)h;
        PU8 end   = on + QUAD(h->doff);
        on         += QUAD(TCP_HEADER_SZ);

        while (on < end)
        {
            U8    opt = *(on++);
            UINT    tmp;
            INT     len = (opt > TCPOPT_NOP) ? *(on++) - 2 : 0;

            if ((on + len) <= end)
            {
                switch (opt)
                {
                    case TCPOPT_EOL:
                        printf("(EOL)");
                        on = end;
                        break;

                    case TCPOPT_NOP:
                        printf("(NOP)");
                        break;

                    case TCPOPT_SACK:
                        printf("(SACK)");
                        break;

                    case TCPOPT_MAXSEG:
                        tmp   = read_int(on, len, true);
                        printf("(MAXSEG=%u)", tmp);
                        break;

                    case TCPOPT_WINDOW:
                        tmp = read_int(on, len, true);
                        printf("(WSCALE=%u)", tmp);
                        break;

                    case TCPOPT_SACK_PERMITTED:
                        printf("(SACK_PERM)");
                        break;

                    case TCPOPT_TIMESTAMP:
                        tmp   = read_int(on, MAC_MIN(len, 4), true);
                        printf("(TIMESTAMP=%u)", tmp);
                        break;

                    default:
                        printf("(UNKNOWN %d)", *on);
                        on = end;
                        break;
                }

                on += len;
            }
            else
                on = end;
        }
    }

    printf(" CS=%x\n", h->check);
}
void SnTcp::handleHeader(struct tcphdr *   th)
{
    SND.wndSize = ntohs(th->window) << SND.wndScale;

    if (th->ack)
    {
        U32  ack = ntohl(th->ack_seq);

//        printf("SnTcp::onRecv_ESTABLISHED stage SND.seqAck = %X, SND.seqNext = %X, ack = %X\n", (UINT)SND.seqAck, (UINT)SND.seqNext, (UINT)ack);
        if ( (SND.seqAck  == ack)   &&
             (SND.seqNext != ack)   )
        {
//            printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ DUB!!!\n");
            dubAck++;
        }
        else
            dubAck = 0;

        SND.seqAck = ack;
    }

    if (th->doff > TCP_HEADER_SZ)
    {
        PU8 on    = (PU8)th;
        PU8 end   = on + QUAD(th->doff);
        on         += QUAD(TCP_HEADER_SZ);

        while (on < end)
        {
            U8        opt = *(on++);
            UINT        tmp;
            PSAKBLOCK   b;
            INT         len = (opt > TCPOPT_NOP) ? *(on++) - 2 : 0;

            if ((on + len) <= end)
            {
                switch (opt)
                {
                    case TCPOPT_EOL:
                        on = end;
                        break;

                    case TCPOPT_NOP:
                        break;

                    case TCPOPT_SACK:
                        b = (PSAKBLOCK)on;

                        for (UINT i =0; i < (len / sizeof(SAKBLOCK)); i++, b++)
                        {
                            printf("SAK %X - %X stage \n", ntohl(b->left), ntohl(b->right));
                        }
                        break;

                    case TCPOPT_MAXSEG:
                        tmp = read_int(on, len, true);

                        if (!tmp && (tmp < optMaxSeg))
                        {
                            optMaxSeg = tmp;
                        }
                        break;

                    case TCPOPT_WINDOW:
                        SND.wndScale   = read_int(on, len, true);
                        RCV.wndScale   = optWndScale;
                        RCV.wndSize    = optWndSize;
                        break;

                    case TCPOPT_SACK_PERMITTED:
                        optSak = true;
                        break;

                    case TCPOPT_TIMESTAMP:
                        RCV.timestamp   = *(PU32)on;

                        if (th->ack)
                            SND.timestamp = read_int(on + sizeof(U32), sizeof(U32), true);
                        break;

                    default:
                        on = end;
                        break;
                }

                on += len;
            }
            else
                on = end;
        }
    }
}
SnTcp::SnTcp()
{
    state           = TCP_CLOSED;
    optMaxSeg       = 1460; // TCP_MSS;
    optSak          = false;
    now             = 0;

    dubAck          = 0;

    sendTime        = 0;
    sendTries       = SNTCP_TRIES;

    memset(&SND, 0, sizeof(SND));
    memset(&RCV, 0, sizeof(RCV));
}
SnTcp::~SnTcp()
{
    if (ptcs.data)
        FREE(ptcs.data);
}
void SnTcp::close()
{
    switch (state)
    {
        case TCP_ESTABLISHED:
        case TCP_SYN_SENT:
        case TCP_SYN_RECV:
        case TCP_LISTEN:
            setState(TCP_FIN_WAIT1);
            break;

        default:
            break;
    }
}
SnTcp * SnTcp::listen(UINT      windowSize,
                      SNTCPTIME now)
{
    SnTcp *     result          = false;

    if ((result = new SnTcp) != NULL)
    {
        result->initialSeq  = now;
        result->SND.seqNext = now;

        result->packetId    = ~now;

        result->initWndScaling(windowSize);
        result->initTx(NULL, 0);

        result->setState(TCP_LISTEN);
    }

    return result;
}
SnTcp * SnTcp::connect(UINT           windowSize,
                       U32         saddr,
                       UINT           sport,
                       U32         daddr,
                       UINT           dport,
                       SNTCPTIME      now)
{
    SnTcp *     result          = false;

    if ((result = new SnTcp) != NULL)
    {
        result->initialSeq  = now;
        result->RCV.addr    = saddr;
        result->RCV.port    = htons(sport);

        result->SND.addr    = daddr;
        result->SND.port    = htons(dport);
        result->SND.seqNext = now;

        result->packetId    = ~now;

        result->initWndScaling(windowSize);
        result->initTx(NULL, 0);

        result->setState(TCP_SYN_SENT);
    }

    return result;
}
void SnTcp::initWndScaling(UINT windowSize)
{
    RCV.wndSize     = MAC_MIN(optMaxSeg, windowSize);
    RCV.wndScale    = 0;

    optWndSize      = windowSize;
    optWndScale     = 0;

//    UINT tmp        = windowSize;
//    while (tmp > 0xFFFF)
//    {
//        tmp >>= 1;
//        optWndScale++;
//    }

    if (windowSize > 0xFFFF)
    {
        optWndScale = 8;
    }
}
UINT SnTcp::getDestPort(PSNPACKET      pkt)
{
    struct tcphdr * h       = HEADERTCP(pkt);

    return ntohs(h->dest);
}
UINT SnTcp::getSourcePort(PSNPACKET      pkt)
{
    struct tcphdr * h       = HEADERTCP(pkt);

    return ntohs(h->source);
}
void SnTcp::optionsAdd(struct tcphdr* h,
                       UINT           opt,
                       PVOID          data,
                       UINT           size)
{
    PU8 on            = (PU8)h;
    *(on + (newDoff++)) = opt;

    if (opt > TCPOPT_NOP)
    {
        *(on + (newDoff++)) = size + 2;

        if (size)
        {
            memcpy(on + newDoff, data, size);
            newDoff += size;
        }
    }
}
void SnTcp::optionsBegin(struct tcphdr* h)
{
    newDoff = QUAD(TCP_HEADER_SZ);
}
void SnTcp::optionsEnd(struct tcphdr* h)
{
    UINT    padding = (newDoff & 3);

    if (padding)
    {
        padding     = sizeof(U32) - padding;
        PU8 on    = (PU8)h;
        on         += newDoff;
        newDoff    += padding;

        memset(on, 0, padding);
    }

//    printf("SnTcp::optionEnd stage newDoff = %d, diff = %d\n", newDoff, padding);

    h->doff         = newDoff >> 2;
}
void SnTcp::resetFlags(struct tcphdr* h)
{
    h->ack  = 0;
    h->doff = 0;
    h->res1 = 0;
    h->res2 = 0;
    h->urg  = 0;
    h->ack  = 0;
    h->psh  = 0;
    h->rst  = 0;
    h->syn  = 0;
    h->fin  = 0;
}
BOOL SnTcp::stuffPacket(PSNPACKET       pkt,
                        struct iphdr *  ih,
                        struct tcphdr * th,
                        UINT            dataSize)
{
    BOOL result         = false;
    UINT tcpSize        = QUAD(th->doff) + dataSize;

    ih->saddr           = RCV.addr;
    ih->daddr           = SND.addr;

    SnIp::setSize(pkt, tcpSize);
    SnIp::setIpChecksum(pkt);

    if (th->ack)
        RCV.seqAck     = RCV.seqNext;

    th->source          = RCV.port;
    th->dest            = SND.port;
    th->window          = htons(RCV.wndSize >> RCV.wndScale);

    th->seq             = htonl(SND.seqNext);
    th->ack_seq         = htonl(RCV.seqNext);
    th->urg             = 0;
    th->check           = checksum(ih, th, dataSize);
    pkt->size           = pkt->eprotoOffset + tcpSize;
    result              = true;

    return result;
}
U16 SnTcp::checksum(struct iphdr*  ih,
                    struct tcphdr* th,
                    UINT           size)
{
    U16         result      = 0;
    PSEUDO      p;
    UINT        tcpSize;

    tcpSize     = (QUAD(th->doff)) + size;
    p.saddr     = ih->saddr;
    p.daddr     = ih->daddr;
    p.zero      = 0;
    p.proto     = ih->protocol;
    p.size      = htons(tcpSize);

    th->check   = 0;

    result      = SnIp::checksum(result, &p, sizeof(p));
    result      = ~SnIp::checksum(result, th, tcpSize);

    return result;
}
void SnTcp::setState(UINT           newState)
{
    printf("STATE %s -> %s\n", _getStateName(state), _getStateName(newState));

    sendTime    = 0;
    sendTries   = SNTCP_TRIES;
    state       = newState;
}
void SnTcp::optionsAddMaxSeg(struct tcphdr * th)
{
    U16 tmp     = htons(optMaxSeg);
    optionsAdd(th, TCPOPT_MAXSEG, &tmp, sizeof(tmp));
}
void SnTcp::optionsAddTimestamp(struct tcphdr * th)
{
    U32      times   [2];

    times[0] = htonl(now);
    times[1] = RCV.timestamp;

    optionsAdd(th, TCPOPT_TIMESTAMP, &times, sizeof(times));
}
UINT SnTcp::optionsNone(struct tcphdr* h)
{
    h->doff = TCP_HEADER_SZ;

    return 0;
}
UINT SnTcp::optionsDefault(struct tcphdr* th)
{
    optionsBegin(th);
    optionsAdd(th, TCPOPT_NOP, NULL, 0);
    optionsAdd(th, TCPOPT_NOP, NULL, 0);
    optionsAddTimestamp(th);
    optionsEnd(th);

    return QUAD(th->doff - TCP_HEADER_SZ);
}
void SnTcp::optionsAddWinScaling(struct tcphdr* th)
{
    if (optWndScale)
    {
        optionsAdd(th, TCPOPT_WINDOW, &optWndScale, sizeof(U8));
    }
}
void SnTcp::optionsAddSackPermit(struct tcphdr* th)
{
    optionsAdd(th, TCPOPT_SACK_PERMITTED, NULL, 0);
}
BOOL SnTcp::isEstablished()
{
    return state == TCP_ESTABLISHED;
}
BOOL SnTcp::isSessionStartPacket(PSNPACKET      pkt)
{
    BOOL result         = false;
    struct tcphdr * th  = HEADERTCP(pkt);

    if (th->syn && !th->ack)
    {
        result = true;
    }

    return result;
}
void SnTcp::initTx(PVOID          data,
                    UINT           size)
{
    ptcs.data   = (PU8)data;
    ptcs.offset = 0;
    ptcs.size   = size;
    ptcs.ready  = (size == 0);
    ptcs.reper  = 0;
}

//INT SnTcp::getUnackedSize()
//{
//    INT     result      = 0;
//
//    result = RCV.seqNext - RCV.seqAck;
//
//    if (result)
//        printf("SnTcp::getUnackedSize stage = %d\n", result);
//
//    return result;
//}
BOOL SnTcp::setTxCopy(PCVOID         data,
                      UINT           size)
{
    BOOL            result          = false;
    PU8           on              = NULL;

    if (ptcs.data)
    {
        if (ptcs.ready)
        {
            FREE(ptcs.data);
            ptcs.data   = NULL;
            on          = (PU8)MALLOC(size);

            if (on)
                initTx(on, size);
        }
        else
        {
            UINT newSize    = ptcs.size + size;
            on              = (PU8)REALLOC(ptcs.data, newSize);

            if (on)
            {
                ptcs.data   = on;
                on         += ptcs.size;
                ptcs.size  += size;
            }
        }
    }
    else
    {
        if ((on = (PU8)MALLOC(size)) != NULL)
        {
            initTx(on, size);
        }
    }

    if (on)
    {
        memcpy(on, data, size);

        result = true;
    }

    return result;
}


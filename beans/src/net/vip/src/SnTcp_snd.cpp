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

#include <stdio.h>
#include <stdlib.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <SnIp.h>
#include <SnTcp.h>

#include <string.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define IS_TIME(a)          (!sendTime || ((sendTime + (a)) <= now))

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

BOOL SnTcp::fillAck(struct tcphdr* th)
{
    resetFlags(th);
    th->ack = true;
    optionsDefault(th);
    return true;
}
BOOL SnTcp::fillFin(struct tcphdr* th,
                    BOOL           ack)
{
    resetFlags(th);
    th->fin = true;
    th->ack = ack;
    optionsDefault(th);
    return true;
}
BOOL SnTcp::fillSyn(struct tcphdr* th,
                    BOOL           ack)
{
    resetFlags(th);
    th->syn     = true;
    th->ack     = ack;

    optionsBegin(th);
    optionsAddMaxSeg(th);
    optionsAddSackPermit(th);
    optionsAddTimestamp(th);
    optionsAddWinScaling(th);
    optionsEnd(th);

    return true;
}
BOOL SnTcp::onTimer_ESTABLISHED(struct tcphdr*  th,
                                UINT &          size)
{
    BOOL            result      = false;

    if (RCV.seqAck < RCV.seqNext)
    {
        if (IS_TIME(TIMEOUT_ACK))
        {
            result = fillAck(th);
        }
    }

    if (!result && !ptcs.ready)
    {
        result = pushOut(th, size);
    }

    return result;
}
BOOL SnTcp::onTimer_CLOSE_WAIT(struct tcphdr* h)
{
    BOOL            result      = false;

    return result;
}
BOOL SnTcp::onTimer_CLOSE(struct tcphdr* th)
{
    BOOL            result      = false;

    return result;
}
BOOL SnTcp::onTimer_CLOSING(struct tcphdr* th)
{
    BOOL            result      = false;

    if (IS_TIME(TIMEOUT_FIN))
    {
        if (sendTries--)
        {
            result = fillAck(th);
        }
        else
            setState(TCP_CLOSED);
    }

    return result;
}
BOOL SnTcp::onTimer_FIN_WAIT1(struct tcphdr*  th)
{
    BOOL            result      = false;

    if (IS_TIME(TIMEOUT_FIN))
    {
        if (sendTries--)
        {
            result = fillFin(th, true);
        }
        else
            setState(TCP_CLOSED);
    }

    return result;
}
//BOOL SnTcp::onTimer_FIN_WAIT2(struct tcphdr*  th)
//{
//    BOOL            result      = false;
//
////    if (IS_TIME(TIMEOUT_FIN))
////    {
////        if (sendTries--)
////        {
//            result = fillAck(th);
////        }
////        else
//            setState(TCP_CLOSED);
////    }
//
//    return result;
//}
BOOL SnTcp::onTimer_TIME_WAIT(struct tcphdr*  th)
{
    BOOL            result      = false;

    if (IS_TIME(TIMEOUT_FIN))
    {
        if (SNTCP_TRIES == (sendTries--))
        {
            result = fillAck(th);
        }
        else
            setState(TCP_CLOSED);
    }

    return result;
}
BOOL SnTcp::onTimer_SYN_SENT(struct tcphdr*  th)
{
    BOOL            result      = false;

    if (IS_TIME(TIMEOUT_SYN))
    {
        if (sendTries--)
        {
            result = fillSyn(th, false);
        }
        else
            setState(TCP_CLOSED);
    }

    return result;
}
BOOL SnTcp::onTimer_SYN_RECV(struct tcphdr*  th)
{
    BOOL            result      = false;

    if (IS_TIME(TIMEOUT_SYN))
    {
        if (sendTries--)
        {
            result = fillSyn(th, true);
        }
        else
            setState(TCP_CLOSED);
    }

    return result;
}
BOOL SnTcp::onTimer_LAST_ACK(struct tcphdr*  th)
{
    BOOL            result      = false;

    if (IS_TIME(TIMEOUT_FIN))
    {
        if (sendTries--)
        {
            result = fillFin(th, true);
        }
        else
            setState(TCP_CLOSED);
    }

    return result;
}
BOOL SnTcp::outgoing(PSNPACKET     pkt,
                     SNTCPTIME     now)
{
#define ONTIMER(a) case TCP_##a: result = onTimer_##a(th); break

    BOOL            result      = false;
    UINT            size        = 0;

    SnIp::setTcpPacket(pkt, packetId++);

    struct iphdr *  ih          = HEADERIP(pkt);
    struct tcphdr * th          = HEADERTCP(pkt);

    this->now                   = now;

    switch (state)
    {
        ONTIMER(CLOSE_WAIT);
        ONTIMER(CLOSE);
        ONTIMER(SYN_SENT);
        ONTIMER(FIN_WAIT1);
//        ONTIMER(FIN_WAIT2);
        ONTIMER(CLOSING);
        ONTIMER(TIME_WAIT);
        ONTIMER(SYN_RECV);
        ONTIMER(LAST_ACK);

        case TCP_ESTABLISHED:
            if (onTimer_ESTABLISHED(th, size))
            {
                memcpy(((PU8)th) + QUAD(th->doff),
                       ptcs.data + ptcs.offset,
                       size);

                result = true;
            }
            break;

        default:
            break;
    }

    if (result)
    {
        sendTime = now;

        if (stuffPacket(pkt, ih, th, size))
        {
            if (size)
                pushOutCommit(size);

            result = true;
        }
    }

    return result;

#undef  ONTIMER
}
void SnTcp::pushOutCommit(UINT           size)
{
    ptcs.offset += size;
    SND.seqNext   = ptcs.reper + ptcs.offset;
    SND.wndSize  -= size;

//    printf("NEXTSEQ %u \n", (UINT)SND.seqNext);
}
BOOL SnTcp::pushOut(struct tcphdr* th,
                    UINT &         size)
{
    BOOL            result      = false;
    UINT            left;
    UINT            max;

    if (!ptcs.reper)
        ptcs.reper  = SND.seqNext;

    if (dubAck > SNTCP_FRT_THRS)
    {
        dubAck          = 0;
        ptcs.offset    = SND.seqAck - ptcs.reper;
        printf("@@@@@@@@@@@@FRT! st %d\n", ptcs.offset);
    }

    left = ptcs.size - ptcs.offset;

    if (left)
    {
        if (SND.wndSize > 0)
        {
            resetFlags(th);
            th->ack = true;

            max     = optMaxSeg - optionsDefault(th);

            size    = MAC_MIN(SND.wndSize, left);
            size    = MAC_MIN(max, size);

            if ((ptcs.offset + size) == (UINT)ptcs.size)
            {
                th->psh = true;
            }

            SND.seqNext  = ptcs.reper + ptcs.offset;
            result       = true;

//            printf("SEQ %u %s", (UINT)SND.seqNext, th->psh ? "PSH " : "");
        }
    }
    else
        ptcs.ready = (SND.seqAck == SND.seqNext);

    return result;
}

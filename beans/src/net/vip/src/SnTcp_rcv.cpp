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
#include <linux/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <SnIp.h>
#include <SnTcp.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define FORCE_ACK           RCV.seqAck = 0


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

void SnTcp::onRecv_LISTEN(struct tcphdr* h)
{
    if (h->syn)
    {
        setState(TCP_SYN_RECV);

        RCV.seqNext = ntohl(h->seq) + 1;
    }
}
void SnTcp::onRecv_SYN_SENT(struct tcphdr*  th)
{
    if (th->syn && th->ack)
    {
        setState(TCP_ESTABLISHED);

        RCV.seqNext     = ntohl(th->seq) + 1;
        SND.seqNext++;

        FORCE_ACK;
    }
    else if (th->rst)
    {
        setState(TCP_CLOSED);
    }
}
void SnTcp::onRecv_SYN_RECV(struct tcphdr* th)
{
    if (th->ack)
    {
        SND.seqNext = ntohl(th->ack_seq);
        setState(TCP_ESTABLISHED);
    }
}
void SnTcp::onRecv_CLOSE_WAIT(struct tcphdr* th)
{
}
void SnTcp::onRecv_CLOSE(struct tcphdr* th)
{
}
void SnTcp::onRecv_CLOSING(struct tcphdr* th)
{
    if (th->fin && th->ack)
    {
        setState(TCP_TIME_WAIT);
    }
}
void SnTcp::onRecv_LAST_ACK(struct tcphdr* h)
{
    if (h->ack)
    {
        setState(TCP_CLOSED);
    }
}
void SnTcp::onRecv_FIN_WAIT1(struct tcphdr* th)
{
    if (th->fin)
    {
        if (th->ack)
        {
            setState(TCP_TIME_WAIT);

            RCV.seqNext++;
            SND.seqNext++;
        }
        else
            setState(TCP_CLOSING);
    }
}
//void SnTcp::onRecv_FIN_WAIT2(struct tcphdr* th,
//                             UINT           size)
//{
//    BOOL            result      = false;
//
//    if (th->fin)
//    {
//        setState(TCP_CLOSED);
//
//        result = fillAck(th);
//    }
//
//    return result;
//}
void SnTcp::onRecv_TIME_WAIT(struct tcphdr* th)
{
    if (th->fin && th->ack)
    {
        FORCE_ACK;
    }
}
BOOL SnTcp::onRecv_ESTABLISHED(struct tcphdr* th,
                               PSNTCS         data)
{
    BOOL            result  = false;
    UINT            seq;

    if (data->size)
    {
        seq = ntohl(th->seq);

//        printf("SnTcp::onRecv_ESTABLISHED stage seq = %u RCV.seqNext = %u\n", seq, RCV.seqNext);

        if (seq == RCV.seqNext)
        {
            RCV.seqNext    += data->size;
            RCV.wndSize    -= data->size;

            if (th->psh)
            {
                data->ready = true;

                FORCE_ACK;
            }

            result  = true;
        }
        else
        {
            // Retransmission
            RCV.seqAck--;
        }
    }

    if (th->fin)
    {
        setState(TCP_LAST_ACK);

        RCV.seqNext++;
    }
    else if (th->rst)
    {

    }
    else if (th->syn)
    {
        if (th->ack)
        {
            FORCE_ACK;
        }
    }

    return result;
}
BOOL SnTcp::incoming(PSNPACKET      pkt,
                     PSNTCS         data,
                     AXUTIME         now)
{
#define ONSTATE(a)  case TCP_##a: onRecv_##a(th); break

    BOOL            result      = false;
    struct iphdr *  ih          = HEADERIP(pkt);
    struct tcphdr * th          = HEADERTCP(pkt);
    UINT            cs;

    this->now       = now;
    data->offset    = pkt->eprotoOffset + QUAD(th->doff);
    data->data      = ((PU8)SNPKTDATA(pkt)) + data->offset;
    data->size      = pkt->size - data->offset;
    cs              = th->check;

    if (checksum(ih, th, data->size) == cs)
    {
//        _showPkt(pkt);

        handleHeader(th);

        switch (state)
        {
            ONSTATE(SYN_SENT);
            ONSTATE(SYN_RECV);
            ONSTATE(LAST_ACK);
            ONSTATE(CLOSE);
            ONSTATE(CLOSING);
            ONSTATE(CLOSE_WAIT);
            ONSTATE(FIN_WAIT1);
//            ONSTATE(FIN_WAIT2);
            ONSTATE(TIME_WAIT);

            case TCP_ESTABLISHED:
                result = onRecv_ESTABLISHED(th, data);
                break;

            case TCP_LISTEN:
                RCV.addr   = ih->daddr;
                RCV.port   = th->dest;
                SND.addr   = ih->saddr;
                SND.port   = th->source;

                onRecv_LISTEN(th);
                break;

            default:
                break;
        }
    }
    else
    {
//        printf("<<<<< RECV <<<<<<<<<<<<<<<<<<<<<<<<\n");
//        printf("!!! WRONG TCP CHECKSUM %x vs %x\n", checksum(ih, th, dataSize), cs);
//        la6_dump_print("WRONG ", pkt + 1, pkt->size);
//        _showPkt(pkt);
    }

    return result;

#undef  ONSTATE
}

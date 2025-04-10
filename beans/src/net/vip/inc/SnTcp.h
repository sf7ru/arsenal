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

#ifndef __SKYNETTCP_H__
#define __SKYNETTCP_H__

#include <axtime.h>

#include <SnPacket.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

const   UINT                TCP_CLOSED          = -1;

const   UINT                SNTCP_TRIES         = 5;
const   UINT                SNTCP_FRT_THRS      = 3;

const   UINT                TIMEOUT_SYN         = 2 * 256;
const   UINT                TIMEOUT_FIN         = 2 * 256;
const   UINT                TIMEOUT_ACK         = 0;


// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define HEADERIP(a)         (struct iphdr*) (((PU8)((a) + 1)) + (a)->protoOffset)
#define HEADERTCP(a)        (struct tcphdr*)(((PU8)((a) + 1)) + (a)->eprotoOffset)
#define TCP_HEADER_SZ       5

#define QUAD(a)             ((a) << 2)


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

struct tcphdr;
struct iphdr;

typedef AXUTIME              SNTCPTIME;

typedef struct __tag_SNTCPTCB
{
    UINT                    wndSize;        // window size
    UINT                    wndScale;
    U32                  addr;
    UINT                    port;
    U32                  seqNext;         //
    U32                  seqAck;
    U32                  timestamp;
} SNTCPTCB, * PSNTCPTCB;

typedef struct __tag_SNTCS
{
    PU8       data;
    INT         offset;
    INT         size;
    INT         reper;
    BOOL        ready;
} SNTCS, * PSNTCS;


// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------


class SnTcp
{
        SNTCPTCB        SND;
        SNTCPTCB        RCV;

        U32          initialSeq;

        UINT            state;
        UINT            newDoff;
        UINT            dubAck;

        U16             packetId;
        SNTCPTIME       now;

        SNTCPTIME       sendTime;
        UINT            sendTries;

        UINT            optMaxSeg;
        UINT            optWndSize;
        UINT            optWndScale;
        BOOL            optSak;

        SNTCS           ptcs;

        void            initWndScaling       (UINT           windowSize);

        void            optionsAdd              (struct tcphdr* h,
                                                 UINT           opt,
                                                 PVOID          data,
                                                 UINT           size);

        void            optionsAddMaxSeg        (struct tcphdr* th);
        void            optionsAddSackPermit    (struct tcphdr* th);
        void            optionsAddTimestamp     (struct tcphdr* th);
        void            optionsAddWinScaling    (struct tcphdr* th);

        void            optionsBegin            (struct tcphdr* th);

        void            optionsEnd              (struct tcphdr* th);

        UINT            optionsNone             (struct tcphdr* th);

        UINT            optionsDefault          (struct tcphdr* th);

        void            onRecv_LISTEN           (struct tcphdr* th);
        void            onRecv_CLOSE            (struct tcphdr* th);
        void            onRecv_CLOSING          (struct tcphdr* th);
        void            onRecv_SYN_SENT         (struct tcphdr* th);
        void            onRecv_SYN_RECV         (struct tcphdr* th);
        void            onRecv_CLOSE_WAIT       (struct tcphdr* th);
        void            onRecv_LAST_ACK         (struct tcphdr* th);
        void            onRecv_FIN_WAIT1        (struct tcphdr* th);
        void            onRecv_FIN_WAIT2        (struct tcphdr* th);
        void            onRecv_TIME_WAIT        (struct tcphdr* th);

        BOOL            onRecv_ESTABLISHED      (struct tcphdr* th,
                                                 PSNTCS         data);

        BOOL            onTimer_CLOSE_WAIT      (struct tcphdr* th);
        BOOL            onTimer_CLOSE           (struct tcphdr* th);
        BOOL            onTimer_CLOSING         (struct tcphdr* th);
        BOOL            onTimer_SYN_SENT        (struct tcphdr* th);
        BOOL            onTimer_FIN_WAIT1       (struct tcphdr* th);
        BOOL            onTimer_FIN_WAIT2       (struct tcphdr* th);
        BOOL            onTimer_TIME_WAIT       (struct tcphdr* th);
        BOOL            onTimer_LAST_ACK        (struct tcphdr* th);
        BOOL            onTimer_SYN_RECV        (struct tcphdr* th);

        BOOL            onTimer_ESTABLISHED     (struct tcphdr* th,
                                                 UINT &         size);

        BOOL            pushOut                 (struct tcphdr* th,
                                                 UINT &         size);

        void            pushOutCommit           (UINT           size);


        void            resetFlags              (struct tcphdr* th);

        void            handleHeader            (struct tcphdr* th);

        U16             checksum                (struct iphdr*  iph,
                                                 struct tcphdr* tcph,
                                                 UINT           size);

        BOOL            stuffPacket             (PSNPACKET      pkt,
                                                 struct iphdr*  ih,
                                                 struct tcphdr* th,
                                                 UINT           dataSize);

        BOOL            reversePacket           (PSNPACKET      pkt,
                                                 struct iphdr*  ih,
                                                 struct tcphdr* th,
                                                 UINT           dataSize);

        void            setState                (UINT           newState);

        BOOL            fillAck                 (struct tcphdr* th);

        BOOL            fillFin                 (struct tcphdr* th,
                                                 BOOL           ack);

        BOOL            fillSyn                 (struct tcphdr* th,
                                                 BOOL           ack);

public:

                        SnTcp                   ();
                        ~SnTcp                  ();

static  void            _showPkt                (PSNPACKET      pkt);

        void            close                   ();

static  BOOL            isSessionStartPacket     (PSNPACKET      pkt);

static  SnTcp *         listen                  (UINT           windowSize,
                                                 SNTCPTIME      now);

static  SnTcp *         connect                 (UINT           windowSize,
                                                 U32         saddr,
                                                 UINT           sport,
                                                 U32         daddr,
                                                 UINT           dport,
                                                 SNTCPTIME      now);

static  UINT            getDestPort             (PSNPACKET      pkt);

static  UINT            getSourcePort           (PSNPACKET      pkt);

        BOOL            outgoing                (PSNPACKET      pkt,
                                                 SNTCPTIME      now);

        BOOL            incoming                (PSNPACKET      pkt,
                                                 PSNTCS         data,
                                                 SNTCPTIME      now);

        BOOL            isClosed                ()
        { return state == TCP_CLOSED; }

        void            dataHandled             (PSNTCS         data)
        { RCV.wndSize += data->size; }

        BOOL            isEstablished           ();


        BOOL            isTxComplete            ()
        {
            return ptcs.ready;
        }

        BOOL            setTxCopy               (PCVOID         data,
                                                 UINT           size);

//        void            setTransfer             (PSNTCS         tcs)
//        { ptcs = tcs; }

        void            initTx                 (PVOID          data,
                                                 UINT           size);

//        INT             getUnackedSize          ();
};

#endif // __SKYNETTCP_H__

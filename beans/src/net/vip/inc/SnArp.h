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

#ifndef __SKYNETARP_H__
#define __SKYNETARP_H__

#include <axtime.h>

#include <SnPacket.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define SNARP_ADDR_MAX_SZ   8

#define SNARP_OP_QUERY      1
#define SNARP_OP_ANSWER     2

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

#pragma pack(1)
typedef AXPACKED(struct) tag_SNARPHDR
{
    U16                     ar_hrd;     // format of hardware address
    U16                     ar_pro;     // format of protocol address
    U8                      ar_hln;     // length of hardware address
    U8                      ar_pln;     // length of protocol address
    U16                     ar_op;      // ARP opcode (command)
} SNARPHDR, * PSNARPHDR;
#pragma pack()

typedef struct __tag_SNARPPKTPROPS
{
    UINT                    hwType;
    UINT                    hwAddrLen;
    UINT                    protoType;
    UINT                    protoAddrLen;

    UINT                    operation;

    PU8                     senderHwAddr;
    PU8                     senderProtoAddr;

    PU8                     targetHwAddr;
    PU8                     targetProtoAddr;

} SNARPPKTPROPS, * PSNARPPKTPROPS;

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------


class SnArp
{
public:
static  BOOL            disassemble             (PSNPACKET      pkt,
                                                 PSNARPPKTPROPS props);

static  BOOL            setTargetMac            (PSNPACKET      pkt,
                                                 PU8            mac);

static  BOOL            getTargetMac            (PSNPACKET      pkt,
                                                 PU8            mac);

static  BOOL            setDestAndTargetMac     (PSNPACKET      pkt,
                                                 PU8            mac);

static  BOOL            setSenderMac            (PSNPACKET      pkt,
                                                 PU8            mac);

static  BOOL            getSenderMac            (PSNPACKET      pkt,
                                                 PU8            mac);

static  BOOL            setSrcAndSenderMac      (PSNPACKET      pkt,
                                                 PU8            mac);

static  BOOL            makePacket              (PSNPACKET      pkt,
                                                 PSNARPPKTPROPS props);

//static  BOOL            makePacket              (PSNPACKET      pkt,
//                                                 UINT           operation,
//                                                 PVOID          senderMac,
//                                                 PVOID          senderIp,
//                                                 PVOID          targetMac,
//                                                 PVOID          targetIp);
};

#endif // __SKYNETARP_H__

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

#ifndef __SKYNETETH_H__
#define __SKYNETETH_H__

#include <SnPacket.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define SNETH_ADDR_LEN      6
#define SNETH_TYPE_LEN      2
#define SNETH_TAG_LEN       4
#define SNETH_FCS_LEN       4

#define SNETH_HDR_Q_SZ      4
#define SNETH_HDR_SZ        14
#define SNETH_QHDR_SZ       (SNETH_HDR_SZ + SNETH_HDR_Q_SZ)
#define SNETH_MAX_PAYL_SZ   1500
#define SNETH_MAX_FRAME_SZ  (SNETH_HDR_SZ +   SNETH_MAX_PAYL_SZ)
#define SNETH_MAX_QFRAME_SZ (SNETH_QHDR_SZ +  SNETH_MAX_PAYL_SZ)
#define SNETH_MIN_PAYL_SZ   46
#define SNETH_MIN_FRAME_SZ  (SNETH_HDR_SZ +   SNETH_MIN_PAYL_SZ)


#define SNETH_MAX_AATPL_LEN  (SNETH_ADDR_LEN + SNETH_ADDR_LEN + SNETH_TYPE_LEN + SNETH_MAX_PAYL_LEN)

#define SNETH_P_IP          0x0800      // Internet Protocol packet
#define SNETH_P_ARP         0x0806      // Address Resolution packet
#define SNETH_P_IP          0x0800      // Internet Protocol packet
#define SNETH_P_8021Q       0x8100      // 802.1Q VLAN Extended Header

#define SNETH_P_TEST        0xF1F1      // Test Protocol packet
#define SNETH_P_PRERUNNER   0xF2F2      // Prerunner packet

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

#pragma pack(1)
typedef AXPACKED(struct) tag_SNETHHDR
{
    U8                      h_dest      [ SNETH_ADDR_LEN ]; // destination eth addr
    U8                      h_source    [ SNETH_ADDR_LEN ]; // source ether addr
    U16                     h_proto;                        // packet type ID field
} SNETHHDR, * PSNETHHDR;
#pragma pack()

#pragma pack(1)
typedef AXPACKED(struct) tag_SNETHHDRQ
{
    U8                      h_dest      [ SNETH_ADDR_LEN ]; // destination eth addr
    U8                      h_source    [ SNETH_ADDR_LEN ]; // source ether addr
    U16                     h_protoQ;                        // packet type ID field = 802.1Q

    U16                     h_cfi;

    U16                     h_proto;                        // packet type ID field
} SNETHHDRQ, * PSNETHHDRQ;
#pragma pack()

typedef struct __tag_SNETHPKTPROPS
{
    PU8         psmac;
    PU8         pdmac;
} SNETHPKTPROPS, * PSNETHPKTPROPS;

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class SnEth
{
public:
                        SnEth                   ();

static  const U8        BROADCAST_ADDR          [ SNETH_ADDR_LEN ];

static  BOOL            getIpOffset             (PSNPACKET      pkt);

static  BOOL            getDestMac              (PU8            buff,
                                                 PSNPACKET      pkt);

static  BOOL            getSourceMac            (PU8            buff,
                                                 PSNPACKET      pkt);

static  void            setMacs                 (PSNPACKET      pkt,
                                                 PVOID          smac,
                                                 PVOID          dmac);

static  void            setDestMac              (PSNPACKET      pkt,
                                                 PU8            mac);

static  void            setSourceMac            (PSNPACKET      pkt,
                                                 PU8            mac);

static  void            reverse                 (PSNPACKET      pkt);

static  BOOL            disassemble             (PSNPACKET      pkt,
                                                 PSNETHPKTPROPS props);

static  BOOL            macIsBroadcast          (PVOID          mac);

static  BOOL            addressCompare          (PVOID          mac1,
                                                 PVOID          mac2);

static  BOOL            addressIsBroadcast      (PVOID          mac);

static  BOOL            addressIsZero           (PVOID          mac);

static  void            addressToString         (PSTR           buff,
                                                 UINT           len,
                                                 PVOID          addr);

static  BOOL            stringToAddress         (PVOID          addr,
                                                 UINT           size,
                                                 PCSTR          string);

static  BOOL            makePacket              (PSNPACKET      pkt,
                                                 PVOID          smac,
                                                 PVOID          dmac,
                                                 UINT           proto);

static  BOOL            makeFcs                 (PSNPACKET      pkt);

//static  BOOL            beginMakingPacket       (PSNPACKET      pkt,
//                                                 PVOID          smac,
//                                                 PVOID          dmac,
//                                                 UINT           proto);
//
//static  BOOL            finalizeMakingPacket    (PSNPACKET      pkt);

};

#endif // __SKYNETETH_H__

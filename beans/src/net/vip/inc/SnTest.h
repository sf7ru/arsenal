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

#ifndef __SKYNETTEST_H__
#define __SKYNETTEST_H__

#include <axtime.h>

#include <SnPacket.h>
//#include <SnCounting.h>
#include <SnEth.h>
#include <SnIp.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define SNARP_ADDR_MAX_SZ   8

#define SNARP_OP_QUERY      1
#define SNARP_OP_ANSWER     2


#define SN_TEST_MAGIC       0x1F2E3D4C6B7C8B9A

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct __tag_SNTESTPROPS
{
    U64                     magic;
    U32                     peerId;
    U32                     session;
    SNCOUNTINT              number;
} SNTESTPROPS, * PSNTESTPROPS;

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------


class SnTest
{
public:
static  BOOL            disassemble             (PSNPACKET      pkt,
                                                 PSNTESTPROPS   props);

static  BOOL            setNumber               (PSNPACKET      pkt,
                                                 PSNIPPKTPROPS  ipProps,
                                                 SNCOUNTINT     number);

BOOL                    setPayload              (PSNPACKET      pkt,
                                                 PSNIPPKTPROPS  ipProps,
                                                 PVOID          data);


static  BOOL            makePacket              (PSNPACKET      pkt,
                                                 PSNETHPKTPROPS ethProps,
                                                 PSNIPPKTPROPS  ipProps,
                                                 PSNTESTPROPS   testProps,
                                                 UINT           extraSize,
                                                 UINT           protoType   = 0);
};

#endif // __SKYNETTEST_H__

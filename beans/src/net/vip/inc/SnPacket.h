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

#ifndef __SKYNETPACKET_H__
#define __SKYNETPACKET_H__

#include <arsenal.h>
#include <axtime.h>

#include <SnTypes.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define SNPKTFLAG_none      0x00000000  //
#define SNPKTFLAG_ARP_Q     0x00001000  // Query
#define SNPKTFLAG_ARP_A     0x00002000  //
//#define SNPKTFLAG_VLAN_TAG  0x00004000  // VLAN TAG present
#define SNPKTFLAG_APP       0x00010000  // First packet flag for application purposes




#define SNPKT_DATA(a)       ((a)->data ? (a)->data : ((a) + 1))

#pragma pack(1)
typedef AXPACKED(struct) __tag_SNPACKET
{
    U64         serial;

    U64         time;

    UINT        protoVersion;   // General proto version
    UINT        protoType;      // General proto type
    UINT        protoOffset;    // Offset of protocol header, usualy IP, ARP, etc.

    UINT        vlan;

    UINT        eprotoVersion;  // Encapsulated proto version
    UINT        eprotoType;     // Encapsulated proto type
    UINT        eprotoOffset;   // Offset of encapsulated protocol header, usualy TCP

    U32         flags;

    U32         hash;

    // NOTE this ones must be last members!
    PVOID       buff;
    UINT        buffSize;
    UINT        buffOffset;
    AXTIME      timestamp;

    PVOID       data;
    UINT        size;       // Size of following data
} SNPACKET, *PSNPACKET;
#pragma pack()



// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

void                snpacket_reset              (PSNPACKET          pkt);

PSNPACKET           snpacket_destroy            (PSNPACKET          pkt);

PSNPACKET           snpacket_create             (UINT               size);


BOOL                snpacket_fill               (PSNPACKET          pkt,
                                                 PVOID              data,
                                                 UINT               size,
                                                 UINT               buffSize,
                                                 UINT               buffOffset);

#endif // __SKYNETPACKET_H__

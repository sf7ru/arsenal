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

#ifndef __ETHBASEH__
#define __ETHBASEH__

#include <stdio.h>

#include <arsenal.h>
#include <axstring.h>
#include <axthreads.h>

#include <SnCounting.h>
#include <SnEth.h>
#include <SnIp.h>


// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define TRICK_MAC_PLUS_SRC  0x0001
#define TRICK_MAC_PLUS_DST  0x0002
#define TRICK_MARK_CHANNELS 0x0004
#define TRICK_MCAST_IP      0x0008
#define TRICK_SPOOF_IP      0x0010

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|(9)----------------|(30)---------------------------------------------

#define ETH_MAX_VLANS       10

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef BOOL  (*ETHRECVCB)(PVOID user_ptr, PVOID packetData, UINT packetSize, UINT vlan, U32 hash);

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------



class Eth
{
protected:
        BOOL            flagGateMacSet;
        BOOL            flagGateIpSet;
        BOOL            flagSelfIpSet;

        WAYCOUNTS       counts;
        HAXMUTEX        countsLCK;

        UINT            vlanNum;
        Eth *           vlans                   [ ETH_MAX_VLANS ];

        U8              ifmac                   [ SNETH_ADDR_LEN ];

        CHAR            iface                   [ 17 ];

        CHAR            ifmacString             [ AXLSHORT ];

        U8              selfIp                  [ SNIP_ADDR_LEN  ];

        U8              defaultGateMac          [ SNETH_ADDR_LEN ];
        U8              defaultGateIp           [ SNIP_ADDR_LEN  ];

public:
        U16             lanId;
        UINT            devIndex;

        PVOID           userPtr;

                        Eth                     (void);
virtual                 ~Eth                    (void);

        INT             incInCounts             (INT            result);

        INT             incOutCounts            (INT            result,
                                                 INT            size);


virtual int             recv                    (PVOID          buffer,
                                                 INT            size,
                                                 UINT           TO)
        = 0;

virtual void            recvCallback            (ETHRECVCB      cb,
                                                 PVOID          userPtr,
                                                 UINT           TO,
                                                 INT            max = -1)
        {}

virtual int             send                    (PVOID          buffer,
                                                 INT            size,
                                                 UINT           TO = -1)
        = 0;

virtual int             send                    (PSNPACKET      pkt,
                                                 UINT           TO = -1)
        { return send(pkt->data, pkt->size, TO); }

virtual BOOL            setSendBuffSize         (int            size)
        { return false; }

virtual BOOL            setRecvBuffSize         (int            size)
        { return false; }

        PU8             getGateMac              (void)
        { return defaultGateMac; }

        PU8             getGateIp               (void)
        { return defaultGateIp;  }

        PU8             getSelfIp               (void)
        { return flagSelfIpSet ? selfIp : nil;  }

        BOOL            isGateMacSet            (void)
        { return flagGateMacSet; }

        BOOL            isGateIpSet             (void)
        { return flagGateIpSet; }

        BOOL            setGateMacString        (PSTR           macString);

        BOOL            setGateMac              (PU8            mac);

        BOOL            setSelfIpString         (PCSTR          addr);

        BOOL            setSelfIp               (PVOID          addr);

        BOOL            getGateMacString        (PSTR           buff,
                                                 UINT           len)
        { SnEth::addressToString(buff, len, defaultGateMac); return true; }

        BOOL            getGateIpString         (PSTR           buff,
                                                 UINT           len)
        { SnIp::addressToString(buff, len, defaultGateIp); return true; }

        BOOL            getSelfIpString         (PSTR           buff,
                                                 UINT           len)
        { SnIp::addressToString(buff, len, selfIp); return true; }

        BOOL            setGateIpString         (PCSTR          ipString);


        PU8             getMac                  (void)
        { return ifmac; }

        void            getMac                  (PU8            mac);

        PCSTR           getMacString            (void)
        { return ifmacString; }

        PCSTR           getName                 (void)
        { return iface; }

        void            getCounts               (PWAYCOUNTS     cnts);

        void            resetCounts             ();

virtual void            close                   ()
        = 0;

        Eth *           addVlan                 (UINT           vlanId);

        Eth *           delVlan                 (Eth *          vlan);

//        Eth *           delVlan                 (UINT           vlanId);

virtual PVOID           setVlanTag              (PVOID          packet,
                                                 PUINT          size,
                                                 U16            vlan_tci)
        { printf("NOT SET VLAN %d\n", vlan_tci); return packet; }

        UINT            getVlansNum             ()
        { return vlanNum; }

        Eth *           getVlanByIdx            (UINT           idx);

        Eth *           getVlanById             (UINT           id);

virtual Eth *           getRoot                 ()
        { return nil; }
};

#endif //  #define __ETHBASEH__

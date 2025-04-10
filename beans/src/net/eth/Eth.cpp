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

#define SN_USE_SYSTEM_NTOH

#include <stdio.h>

#include <string.h>

#include <Eth.h>

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|(9)------------|(25)-------------------(|(50)----------|(65)---------

class EthVlan: public Eth
{
        Eth *           root;

public:
        UINT            vlanIndex;

                        EthVlan                 (Eth *          nroot,
                                                 UINT           nVlanId)
                        {
                            this->vlanIndex = AXII;
                            this->root      = nroot;
                            this->lanId     = nVlanId;
                        }

                        ~EthVlan                ()
                        {}

        void            recvCallback            (ETHRECVCB      cb,
                                                 PVOID          userPtr,
                                                 UINT           TO,
                                                 INT            max = -1)
        { root->recvCallback(cb, userPtr, TO, max); }

        int             recv                    (PVOID          buffer,
                                                 INT            size,
                                                 UINT           TO)
        { return incInCounts(root->recv(buffer, size, TO)); }


        int             send                    (PSNPACKET      pkt,
                                                 UINT           TO = -1)
        {
            pkt->vlan   = lanId;
            pkt->data   = root->setVlanTag(pkt->data, &pkt->size, lanId);

            return incOutCounts(root->send(pkt->data, pkt->size, TO), pkt->size);
        }

        int             send                    (PVOID          buffer,
                                                 INT            size,
                                                 UINT           TO = -1)
        { printf("DO NOT USE send(data, size) on VLAN!\n");return -1; }

        void            close                   ()
        { root->delVlan(this); }

        Eth *           getRoot                 ()
        { return root; }
};


// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      Eth::Eth
// PURPOSE
//      Class construction
// PARAMETERS
//        void -- None
// ***************************************************************************
Eth::Eth(void)
{
    *ifmacString    = 0;

    lanId           = 0;

    vlanNum         = 0;
    devIndex        = 0;

    userPtr         = nil;

    countsLCK       = nil;

    flagGateMacSet  = false;
    flagGateIpSet   = false;
    flagSelfIpSet   = false;

    memset(&counts, 0, sizeof(counts));

    memset(&ifmac,          -1, SNETH_ADDR_LEN);
    memset(&defaultGateMac, -1, SNETH_ADDR_LEN);

    memset(vlans, 0, sizeof(Eth*) * ETH_MAX_VLANS);
}
// ***************************************************************************
// TYPE
//      Destructor
// FUNCTION
//      Eth::~Eth
// PURPOSE
//      Class destruction
// PARAMETERS
//        void -- None
// ***************************************************************************
Eth::~Eth(void)
{
    SAFEDESTROY(axmutex,    countsLCK);

    if (vlanNum)
    {
        for (UINT i = 0; i < vlanNum; i++)
        {
            SAFEDELETE(vlans[i]);
        }
    }
}
// ***************************************************************************
// FUNCTION
//      Eth::getCounts
// PURPOSE
//
// PARAMETERS
//      PWAYCOUNTS cnts --
// RESULT
//      void --
// ***************************************************************************
void Eth::getCounts(PWAYCOUNTS     cnts)
{
    if (cnts)
    {
        axmutex_lock(countsLCK, true);
        memcpy(cnts, &counts, sizeof(counts));
        axmutex_unlock(countsLCK);

        cnts->lanId = lanId;
    }
}
// ***************************************************************************
// FUNCTION
//      Eth::incInCounts
// PURPOSE
//
// PARAMETERS
//      INT result --
// RESULT
//      void --
// ***************************************************************************
INT Eth::incInCounts(INT  result)
{
    if (result > 0)
    {
        AXUTIME     now = axutime_get();

        axmutex_lock(countsLCK, true);
        counts.in.packets++;
        counts.in.size += result;
        counts.in.last  = now;
        axmutex_unlock(countsLCK);
    }

    return result;
}
// ***************************************************************************
// FUNCTION
//      Eth::incOutCounts
// PURPOSE
//
// PARAMETERS
//      INT result --
//      INT size   --
// RESULT
//      void --
// ***************************************************************************
INT Eth::incOutCounts(INT  result,
                       INT  size)
{
    AXUTIME     now = axutime_get();

    axmutex_lock(countsLCK, true);

    if (result > 0)
    {
        counts.out.packets++;
        counts.out.size += result;
        counts.out.last  = now;
    }
    else
    {
        counts.drop.packets++;
        counts.drop.size += size;
        counts.drop.last  = now;
    }
    axmutex_unlock(countsLCK);

    return result;
}
// ***************************************************************************
// FUNCTION
//      Eth::resetCounts
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      void --
// ***************************************************************************
void Eth::resetCounts()
{
    axmutex_lock(countsLCK, true);
    memset(&counts, 0, sizeof(counts));
    axmutex_unlock(countsLCK);
}
// ***************************************************************************
// FUNCTION
//      Eth::setGateMacString
// PURPOSE
//
// PARAMETERS
//      PSTR macString --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL Eth::setGateMacString(PSTR           macString)
{
    BOOL            result          = false;

    ENTER(true);

    if (SnEth::stringToAddress(&defaultGateMac, SNETH_ADDR_LEN, macString))
    {
        flagGateMacSet  = true;
        result          = true;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      Eth::setGateIpString
// PURPOSE
//
// PARAMETERS
//      PCSTR ipString --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL Eth::setGateIpString(PCSTR          ipString)
{
    BOOL            result          = false;

    ENTER(true);

    if (SnIp::stringToAddress(&defaultGateIp, SNIP_ADDR_LEN, ipString))
    {
        result          = true;
        flagGateIpSet   = true;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      Eth::setGateMac
// PURPOSE
//
// PARAMETERS
//      PU8 mac --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL Eth::setGateMac(PU8          mac)
{
    BOOL            result          = false;

    ENTER(true);

    memcpy(defaultGateMac, mac, SNETH_ADDR_LEN);

    result          = true;
    flagGateMacSet  = true;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      Eth::setSelfIpString
// PURPOSE
//
// PARAMETERS
//      PCSTR addr --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL Eth::setSelfIpString(PCSTR          addr)
{
    BOOL            result          = false;

    ENTER(true);

    if (SnIp::stringToAddress(&selfIp, SNIP_ADDR_LEN, addr))
    {
        flagSelfIpSet   = true;
        result          = true;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      Eth::setSelfIp
// PURPOSE
//
// PARAMETERS
//      PVOID addr --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL Eth::setSelfIp(PVOID          addr)
{
    BOOL            result          = false;

    ENTER(true);

    memcpy(selfIp, addr, SNIP_ADDR_LEN);

    flagSelfIpSet   = true;
    result          = true;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      Eth::getMac
// PURPOSE
//
// PARAMETERS
//      PU8 mac --
// RESULT
//      void --
// ***************************************************************************
void Eth::getMac(PU8            mac)
{
    memcpy(mac, ifmac, SNETH_ADDR_LEN);
}
// ***************************************************************************
// FUNCTION
//      Eth::addVlan
// PURPOSE
//
// PARAMETERS
//      UINT nVlanId --
// RESULT
//      Eth * --
// ***************************************************************************
Eth * Eth::addVlan(UINT           nVlanId)
{
    EthVlan *               result          = nil;

    ENTER(true);

    if ((result = new EthVlan(this, nVlanId)) != nil)
    {
        vlans[vlanNum]      = result;
        result->vlanIndex   = vlanNum++;

        if (flagGateMacSet)
            result->setGateMac(defaultGateMac);

        memcpy(result->ifmac, ifmac, SNETH_ADDR_LEN);

//        if (flagGateIpSet)
//            result->setGateMac(defaultGateMac);
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      Eth::delVlan
// PURPOSE
//
// PARAMETERS
//      UINT vlanId --
// RESULT
//      Eth * --
// ***************************************************************************
//Eth * Eth::delVlan(UINT           vlanId)
//{
//    EthVlan *               result          = nil;
//
//    ENTER(true);
//
//    if ((result = vlans[])) != nil)
//    {
//        result->vlanId      = vlanId;
//        result->vlanIndex   = vlanNum++;
//    }
//
//    RETURN(result);
//}
// ***************************************************************************
// FUNCTION
//      Eth::delVlan
// PURPOSE
//
// PARAMETERS
//      Eth * vlan --
// RESULT
//      Eth * --
// ***************************************************************************
Eth * Eth::delVlan(Eth *          vlan)
{
    EthVlan *               result          = nil;
    EthVlan *               on              = (EthVlan *)vlan;
    UINT                    idx;

    ENTER(vlan);

    idx = on->vlanIndex;

    if ((result = (EthVlan*)vlans[idx]) != nil)
    {
        vlans[idx] = nil;
        delete result;

        if ((idx + 1) == vlanNum)
        {
            vlan--;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      Eth::getVlanByIdx
// PURPOSE
//
// PARAMETERS
//      UINT idx --
// RESULT
//      Eth * --
// ***************************************************************************
Eth * Eth::getVlanByIdx(UINT           idx)
{
    EthVlan *       result      = nil;

    ENTER(true);

    if (idx < vlanNum)
    {
        result = (EthVlan*)vlans[idx];
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      Eth::getVlanById
// PURPOSE
//
// PARAMETERS
//      UINT id --
// RESULT
//      Eth * --
// ***************************************************************************
Eth * Eth::getVlanById(UINT           id)
{
    Eth *           result      = nil;

    ENTER(true);

    for (UINT idx = 0; !result && (idx < vlanNum); idx++)
    {
        if (vlans[idx]->lanId == id)
        {
            result = vlans[idx];
        }
    }

    RETURN(result);
}

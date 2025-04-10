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

#include <string.h>
#include <stdio.h>

#include <arsenal.h>

#include <axstring.h>
#include <axhashing.h>

#include <SnTypes.h>

//#include <Eth.h>
#include <SnEth.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|(9)----------------|(30)---------------------------------------------

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

const U8 SnEth::BROADCAST_ADDR[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

SnEth::SnEth()
{
}
// ***************************************************************************
// FUNCTION
//      SnEth::getIpOffset
// PURPOSE
//
// PARAMETERS
//      PSNPACKET pkt --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SnEth::getIpOffset(PSNPACKET pkt)
{
    BOOL                result          = false;
    PSNETHHDR           hdr;

    hdr = (PSNETHHDR)SNPKT_DATA(pkt);

    if (ntohs(hdr->h_proto) == SNETH_P_IP)
    {
        pkt->protoOffset    = sizeof(SNETHHDR);
        result              = true;
    }
//    else
//        printf("hdr->h_proto = %d\n", hdr->h_proto);

    return result;
}
// ***************************************************************************
// FUNCTION
//      SnEth::getSourceMac
// PURPOSE
//
// PARAMETERS
//      PU8     buff --
//      PSNPACKET pkt  --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SnEth::getSourceMac(PU8      buff,
                         PSNPACKET pkt)
{
    BOOL            result          = false;
    PSNETHHDR eh;

    ENTER(true);

    eh      = (PSNETHHDR)SNPKT_DATA(pkt);
    result  = true;

    memcpy(buff, eh->h_source, SNETH_ADDR_LEN);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SnEth::getDestMac
// PURPOSE
//
// PARAMETERS
//      PU8     buff --
//      PSNPACKET pkt  --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SnEth::getDestMac(PU8      buff,
                       PSNPACKET     pkt)
{
    BOOL            result          = false;
    PSNETHHDR eh;

    ENTER(true);

    eh      = (PSNETHHDR)SNPKT_DATA(pkt);
    result  = true;

    memcpy(buff, eh->h_dest, SNETH_ADDR_LEN);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SnEth::setDestMac
// PURPOSE
//
// PARAMETERS
//      PSNPACKET pkt --
//      PU8     mac --
// RESULT
//      void --
// ***************************************************************************
void SnEth::setDestMac(PSNPACKET      pkt,
                       PU8          mac)
{
    PSNETHHDR eh = (PSNETHHDR)SNPKT_DATA(pkt);

    ENTER(true);

    memcpy(eh->h_dest, mac, SNETH_ADDR_LEN);

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      SnEth::setSourceMac
// PURPOSE
//
// PARAMETERS
//      PSNPACKET pkt --
//      PU8     mac --
// RESULT
//      void --
// ***************************************************************************
void SnEth::setSourceMac(PSNPACKET      pkt,
                         PU8          mac)
{
    PSNETHHDR eh = (PSNETHHDR)SNPKT_DATA(pkt);

    ENTER(true);

    memcpy(eh->h_source, mac, SNETH_ADDR_LEN);

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      SnEth::reverse
// PURPOSE
//
// PARAMETERS
//      PSNPACKET pkt --
// RESULT
//      void --
// ***************************************************************************
void SnEth::reverse(PSNPACKET      pkt)
{
    PSNETHHDR eh;
    U8              tmp     [ SNETH_ADDR_LEN ];

    ENTER(true);

    eh = (PSNETHHDR)SNPKT_DATA(pkt);

    memcpy(tmp,             eh->h_source,   SNETH_ADDR_LEN);
    memcpy(eh->h_source,    eh->h_dest,     SNETH_ADDR_LEN);
    memcpy(eh->h_dest,      tmp,            SNETH_ADDR_LEN);

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      SnEth::setMacs
// PURPOSE
//
// PARAMETERS
//      PSNPACKET pkt  --
//      PVOID     smac --
//      PVOID     dmac --
// RESULT
//      void --
// ***************************************************************************
void SnEth::setMacs(PSNPACKET      pkt,
                    PVOID          smac,
                    PVOID          dmac)
{
    PSNETHHDR       eh;

    ENTER(true);

    eh = (PSNETHHDR)SNPKT_DATA(pkt);

    memcpy(eh->h_source,    smac,   SNETH_ADDR_LEN);
    memcpy(eh->h_dest,      dmac,   SNETH_ADDR_LEN);

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      SnEth::makePacket
// PURPOSE
//
// PARAMETERS
//      PSNPACKET pkt   --
//      PVOID     smac  --
//      PVOID     dmac  --
//      UINT      proto --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SnEth::makePacket(PSNPACKET      pkt,
                       PVOID          smac,
                       PVOID          dmac,
                       UINT           proto)
{
    BOOL            result          = false;
    PSNETHHDR       eh;

    ENTER(true);

    eh                  = (PSNETHHDR)SNPKT_DATA(pkt);
    eh->h_proto         = htons(proto);

    memcpy(eh->h_source,    smac,   SNETH_ADDR_LEN);
    memcpy(eh->h_dest,      dmac,   SNETH_ADDR_LEN);

    pkt->time           = 0;
    pkt->protoType      = proto;
    pkt->protoVersion   = 0;
    pkt->protoOffset    = sizeof(SNETHHDR);
    pkt->eprotoVersion  = 0;
    pkt->eprotoType     = 0;
    pkt->eprotoOffset   = 0;
    pkt->flags          = 0;
    pkt->size           = pkt->protoOffset;

    result              = true;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SnEth::makeFcs
// PURPOSE
//
// PARAMETERS
//      PSNPACKET pkt --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SnEth::makeFcs(PSNPACKET      pkt)
{
    BOOL            result      = false;
    PU8             on;
    PU32            pfcs;
    U32             fcs;

    ENTER(true);

    on          = (PU8)SNPKT_DATA(pkt);
    fcs         = crc32_calc(on, pkt->size);
    pfcs        = (PU32)(on + pkt->size);
    *pfcs       = fcs; //htonl(fcs);
    pkt->size  += sizeof(U32);
    result      = true;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SnEth::disassemble
// PURPOSE
//
// PARAMETERS
//      PSNPACKET   pkt   --
//      PSNETHPROPS props --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SnEth::disassemble(PSNPACKET           pkt,
                        PSNETHPKTPROPS      props)
{
    BOOL            result          = false;
    PSNETHHDR       eh              = (PSNETHHDR)SNPKT_DATA(pkt);
    PSNETHHDRQ      ehq             = (PSNETHHDRQ)SNPKT_DATA(pkt);

    ENTER(true);

    pkt->protoType      = ntohs(eh->h_proto);

    switch (pkt->protoType)
    {
        case SNETH_P_8021Q:
            pkt->protoType      = ntohs(ehq->h_proto);
            pkt->vlan           = ntohs(ehq->h_cfi) & 0xFFF;
            pkt->protoOffset    = sizeof(SNETHHDRQ);
            break;

        default:
            pkt->protoOffset    = sizeof(SNETHHDR);
            break;
    }

    if (props)
    {
        props->psmac        = eh->h_source;
        props->pdmac        = eh->h_dest;
    }

    result              = true;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SnEth::macIsBroadcast
// PURPOSE
//
// PARAMETERS
//      PVOID mac --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SnEth::macIsBroadcast(PVOID          mac)
{
    BOOL            result          = true;
    PU8             on              = (PU8)mac;

    ENTER(true);

    for (int idx = 0; result && (idx < SNETH_ADDR_LEN); idx++)
    {
        if (on[idx] != 0xFF)
        {
            result = false;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SnEth::macCmp
// PURPOSE
//
// PARAMETERS
//      PVOID mac1 --
//      PVOID mac2 --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SnEth::addressCompare(PVOID          mac1,
                   PVOID          mac2)
{
    BOOL            result          = true;
    PU8             on1             = (PU8)mac1;
    PU8             on2             = (PU8)mac2;

    ENTER(true);

    for (int idx = 0; result && (idx < SNETH_ADDR_LEN); idx++)
    {
        if (on1[idx] != on2[idx])
        {
            result = false;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SnEth::addressIsBroadcast
// PURPOSE
//
// PARAMETERS
//      PVOID mac --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SnEth::addressIsBroadcast(PVOID          mac)
{
    BOOL            result          = true;
    PU8             on1             = (PU8)mac;

    ENTER(true);

    for (int idx = 0; result && (idx < SNETH_ADDR_LEN); idx++)
    {
        if (on1[idx] != 0xFF)
        {
            result = false;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SnEth::addressIsBroadcast
// PURPOSE
//
// PARAMETERS
//      PVOID mac --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SnEth::addressIsZero(PVOID          mac)
{
    BOOL            result          = true;
    PU8             on1             = (PU8)mac;

    ENTER(true);

    for (int idx = 0; result && (idx < SNETH_ADDR_LEN); idx++)
    {
        if (on1[idx] != 0)
        {
            result = false;
        }
    }

    RETURN(result);
}
#ifndef USE_NOSTRING
// ***************************************************************************
// FUNCTION
//      SnEth::addressToString
// PURPOSE
//
// PARAMETERS
//      PSTR  buff --
//      UINT  len  --
//      PVOID addr --
// RESULT
//      void --
// ***************************************************************************
void SnEth::addressToString(PSTR           buff,
                            UINT           len,
                            PVOID          addr)
{
    PU8     on          = (PU8)addr;

    ENTER(true);

    sprintf(buff, "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",
             on[0],
             on[1],
             on[2],
             on[3],
             on[4],
             on[5]);
    QUIT;
}
#endif
// ***************************************************************************
// FUNCTION
//      SnEth::stringToAddress
// PURPOSE
//
// PARAMETERS
//      PVOID addr   --
//      UINT  size   --
//      PCSTR string --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SnEth::stringToAddress(PVOID          addr,
                            UINT           size,
                            PCSTR          string)
{
    BOOL            result          = false;
    UINT            len;
    UINT            idx             = 0;
    PU8             onAddr          = (PU8)addr;
    PSTR            on;
    CHAR            tmp             [ AXLSHORT ];

    ENTER(true);

    if (size >= SNETH_ADDR_LEN)
    {
        strz_cpy(tmp, string, AXLSHORT);

        on      = tmp;
        len     = strlen(on);
        result  = true;

        while (  result                                                     &&
                 (  ((on = strz_substrs_get_u(on, &len, ':')) != nil)   ||
                    ((on = strz_substrs_get_u(on, &len, '-')) != nil)   )   )
        {
            on = strz_trim(on);

            if (on && *on && (idx < SNETH_ADDR_LEN))
            {
                strz_hex2bin(onAddr + idx, 1, on);
                idx++;
            }
            else
                result = false;
        }

        if (result && (idx != SNETH_ADDR_LEN))
        {
            result = false;
        }
    }

    RETURN(result);
}

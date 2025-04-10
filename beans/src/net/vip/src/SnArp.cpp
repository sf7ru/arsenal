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

//#include <sys/socket.h>
//#include <linux/if_arp.h>
//#include <arpa/inet.h>

#include <stdio.h>
#include <string.h>

#include <SnEth.h>
#include <SnArp.h>
#include <SnIp.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      SnArp::disassemble
// PURPOSE
//
// PARAMETERS
//      PSNPACKET   pkt   --
//      PSNARPPROPS props --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SnArp::disassemble(PSNPACKET      pkt,
                        PSNARPPKTPROPS props)
{
    BOOL            result          = false;
    PU8             on;
    PSNARPHDR       ah              = (PSNARPHDR)(((PU8)SNPKT_DATA(pkt)) + pkt->protoOffset);

    ENTER(true);

    memset(props, 0, sizeof(SNARPPKTPROPS));

    props->hwType          = ntohs(ah->ar_hrd);
    props->hwAddrLen       = ah->ar_hln;

    props->protoType       = ntohs(ah->ar_pro);
    props->protoAddrLen    = ah->ar_pln;

    props->operation       = ntohs(ah->ar_op);

    on                     = (PU8)((&ah->ar_op) + 1);

    if (  (props->hwAddrLen    <= SNARP_ADDR_MAX_SZ)   &&
          (props->protoAddrLen <= SNARP_ADDR_MAX_SZ)   )
    {
        if (props->hwAddrLen)
        {
            props->senderHwAddr       = on;
            on                    += props->hwAddrLen;
        }

        if (props->protoAddrLen)
        {
            props->senderProtoAddr    = on;
            on                    += props->protoAddrLen;
        }

        if (props->hwAddrLen)
        {
            props->targetHwAddr       = on;
            on                    += props->hwAddrLen;
        }

        if (props->protoAddrLen)
        {
            props->targetProtoAddr     = on;
            on                     += props->protoAddrLen;
        }

        result = true;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SnArp::setTargetMac
// PURPOSE
//
// PARAMETERS
//      PSNPACKET pkt --
//      PU8       mac --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL SnArp::setTargetMac(PSNPACKET      pkt,
                         PU8            mac)
{
    BOOL            result          = false;
    SNARPPKTPROPS   props;

    ENTER(true);

    disassemble(pkt, &props);

    if (props.targetHwAddr)
    {
        memcpy(props.targetHwAddr, mac, props.hwAddrLen);

        result = true;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SnArp::getTargetMac
// PURPOSE
//
// PARAMETERS
//      PSNPACKET pkt --
//      PU8       mac --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL SnArp::getTargetMac(PSNPACKET      pkt,
                         PU8            mac)
{
    BOOL            result          = false;
    SNARPPKTPROPS   props;

    ENTER(true);

    disassemble(pkt, &props);

    if (props.targetHwAddr)
    {
        memcpy(mac, props.targetHwAddr, props.hwAddrLen);

        result = true;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SnArp::setDestAndTargetMac
// PURPOSE
//
// PARAMETERS
//      PSNPACKET pkt --
//      PU8    mac --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SnArp::setDestAndTargetMac(PSNPACKET      pkt,
                                PU8         mac)
{
    BOOL            result          = false;

    ENTER(true);

    SnEth::setDestMac(pkt, mac);

    result = setTargetMac(pkt, mac);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SnArp::setSenderMac
// PURPOSE
//
// PARAMETERS
//      PSNPACKET pkt --
//      PU8       mac --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL SnArp::setSenderMac(PSNPACKET      pkt,
                         PU8         mac)
{
    BOOL            result          = false;
    SNARPPKTPROPS   props;

    ENTER(true);

    disassemble(pkt, &props);

    if (props.senderHwAddr)
    {
        memcpy(props.senderHwAddr, mac, props.hwAddrLen);

        result = true;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SnArp::getSenderMac
// PURPOSE
//
// PARAMETERS
//      PSNPACKET pkt --
//      PU8       mac --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL SnArp::getSenderMac(PSNPACKET      pkt,
                         PU8         mac)
{
    BOOL            result          = false;
    SNARPPKTPROPS   props;

    ENTER(true);

    disassemble(pkt, &props);

    if (props.senderHwAddr)
    {
        memcpy(mac, props.senderHwAddr, props.hwAddrLen);

        result = true;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SnArp::setSrcAndSenderMac
// PURPOSE
//
// PARAMETERS
//      PSNPACKET pkt --
//      PU8    mac --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SnArp::setSrcAndSenderMac(PSNPACKET      pkt,
                               PU8         mac)
{
    BOOL            result          = false;

    ENTER(true);

    SnEth::setSourceMac(pkt, mac);

    result = setSenderMac(pkt, mac);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SnArp::makePacket
// PURPOSE
//
// PARAMETERS
//      PSNPACKET pkt       --
//      UINT      operation --
//      PVOID     senderMac --
//      PVOID     senderIp  --
//      PVOID     targetMac --
//      PVOID     targetIp  --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
//BOOL SnArp::makePacket(PSNPACKET      pkt,
//                       UINT           operation,
//                       PVOID          senderHwAddr,
//                       PVOID          senderProtoAddr,
//                       PVOID          targetHwAddr,
//                       PVOID          targetProtoAddr)
//{
//    BOOL            result          = false;
//    PU8             on;
//    PSNARPHDR       ah;
//
//    ENTER(true);
//
//    if (SnEth::makePacket(pkt, senderHwAddr, targetHwAddr, SNETH_P_ARP))
//    {
//        ah          = (PSNARPHDR)(((PU8)SNPKT_DATA(pkt)) + pkt->protoOffset);
//        ah->ar_hrd  = htons(1);
//        ah->ar_hln  = SNETH_ADDR_LEN;
//        ah->ar_pro  = htons(SNETH_P_IP);
//        ah->ar_pln  = SNIP_ADDR_LEN;
//        ah->ar_op   = htons(operation);
//
//        on          = (PU8)((&ah->ar_op) + 1);
//
//        memcpy(on, senderHwAddr, SNETH_ADDR_LEN);
//
//        on         += SNETH_ADDR_LEN;
//
//        memcpy(on, senderProtoAddr, SNIP_ADDR_LEN);
//
//        on         += SNIP_ADDR_LEN;
//
//        memcpy(on, targetHwAddr, SNETH_ADDR_LEN);
//
//        on         += SNETH_ADDR_LEN;
//
//        memcpy(on, targetProtoAddr, SNIP_ADDR_LEN);
//
//        pkt->size  += (sizeof(SNARPHDR) + (SNETH_ADDR_LEN << 1) + (SNIP_ADDR_LEN << 1));
//        result      = true;
//    }
//
//    RETURN(result);
//}
// ***************************************************************************
// FUNCTION
//      SnArp::makePacket
// PURPOSE
//
// PARAMETERS
//      PSNPACKET      pkt   --
//      PSNARPPKTPROPS props --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL SnArp::makePacket(PSNPACKET      pkt,
                       PSNARPPKTPROPS props)
{
    BOOL            result          = false;
    PU8             on;
    PSNARPHDR       ah;

    ENTER(true);

    if (SnEth::makePacket(pkt, props->senderHwAddr, props->targetHwAddr, SNETH_P_ARP))
    {
        ah          = (PSNARPHDR)(((PU8)SNPKT_DATA(pkt)) + pkt->protoOffset);
        ah->ar_hrd  = htons(1);
        ah->ar_hln  = SNETH_ADDR_LEN;
        ah->ar_pro  = htons(SNETH_P_IP);
        ah->ar_pln  = SNIP_ADDR_LEN;
        ah->ar_op   = htons(props->operation);

        on          = (PU8)((&ah->ar_op) + 1);

        memcpy(on, props->senderHwAddr, SNETH_ADDR_LEN);

        on         += SNETH_ADDR_LEN;

        memcpy(on, props->senderProtoAddr, SNIP_ADDR_LEN);

        on         += SNIP_ADDR_LEN;

        memcpy(on, props->targetHwAddr, SNETH_ADDR_LEN);

        on         += SNETH_ADDR_LEN;

        memcpy(on, props->targetProtoAddr, SNIP_ADDR_LEN);

        pkt->size  += (sizeof(SNARPHDR) + (SNETH_ADDR_LEN << 1) + (SNIP_ADDR_LEN << 1));
        result      = true;
    }

    RETURN(result);
}


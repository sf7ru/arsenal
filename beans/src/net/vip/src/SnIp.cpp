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
#include <string.h>

#include <axstring.h>

#include <SnIp.h>
#include <SnEth.h>

// diff with 0c1426d

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define MIN_IP_PAYLOAD      48


#define IP_HEADER_MIN_SZ    5
#define IH_SIZE(a)          (sizeof(SNIPHDR) + (((a)->ihl - IP_HEADER_MIN_SZ) << 2))

#if (TARGET_ARCH == __A7_arm__)
#define SET_INT(a,b)        write_int(&a, b, sizeof(b), false)
#define GET_INT(a, b)       a = read_int(&b, sizeof(b), false)
#else
#define SET_INT(a,b)        a = b
#define GET_INT(a, b)       a = b
#endif

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------


// ***************************************************************************
// FUNCTION
//      SnIp::checksum
// PURPOSE
//
// PARAMETERS
//      PU8  buf    --
//      UINT nbytes --
//      U32  sum    --
// RESULT
//      U32 --
// ***************************************************************************
U32 SnIp::checksum(PU8      buf,
                   UINT     nbytes,
                   U32      sum)
{
    uint i;

    /* Checksum all the pairs of bytes first... */
    for (i = 0; i < (nbytes & ~1U); i += 2) {
        sum += (U16)ntohs(*((U16 *)(buf + i)));
        if (sum > 0xFFFF)
            sum -= 0xFFFF;
    }

    /*
     * If there's a single byte left over, checksum it, too.
     * Network byte order is big-endian, so the remaining byte is
     * the high byte.
     */
    if (i < nbytes) {
        sum += buf[i] << 8;
        if (sum > 0xFFFF)
            sum -= 0xFFFF;
    }

    return (sum);
}
// ***************************************************************************
// FUNCTION
//      SnIp::wrapChecksum
// PURPOSE
//
// PARAMETERS
//      U32 sum --
// RESULT
//      U32 --
// ***************************************************************************
U32 SnIp::wrapChecksum(U32 sum)
{
    sum = ~sum & 0xFFFF;
    return (htons(sum));
}
// ***************************************************************************
// FUNCTION
//      SnIp::setSize
// PURPOSE
//
// PARAMETERS
//      PSNPACKET pkt  --
//      UINT      size --
// RESULT
//      void --
// ***************************************************************************
void SnIp::setSize(PSNPACKET      pkt,
                   UINT           size)
{
    PSNIPHDR ih     = (PSNIPHDR)(((PU8)SNPKT_DATA(pkt)) + pkt->protoOffset);
    ih->tot_len     = ntohs((pkt->eprotoOffset - pkt->protoOffset) + size);
}
// ***************************************************************************
// FUNCTION
//      SnIp::setChecksum
// PURPOSE
//
// PARAMETERS
//      PSNPACKET pkt --
// RESULT
//      void --
// ***************************************************************************
void SnIp::setIpChecksum(PSNPACKET      pkt)
{
    PSNIPHDR ih     = (PSNIPHDR)(((PU8)SNPKT_DATA(pkt)) + pkt->protoOffset);
    ih->check       = 0;
    //ih->check       = ~checksum(0, ih, IH_SIZE(ih));
    ih->check       = wrapChecksum(checksum((unsigned char*)ih, IH_SIZE(ih), 0));
}
// ***************************************************************************
// FUNCTION
//      SnIp::addressCompare
// PURPOSE
//
// PARAMETERS
//      PU8 addr1 --
//      PU8 addr2 --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SnIp::addressCompare(PVOID          addr1,
                          PVOID          addr2)
{
    BOOL            result          = false;

    ENTER(true);

    if (addr1 && addr2)
    {
        result = !memcmp(addr1, addr2, SNIP_ADDR_LEN);
    }

    RETURN(result);
}
#ifndef USE_NOSTRING
// ***************************************************************************
// FUNCTION
//      SnIp::addressToString
// PURPOSE
//
// PARAMETERS
//      PSTR  buff --
//      UINT  len  --
//      PVOID addr --
// RESULT
//      void --
// ***************************************************************************
void SnIp::addressToString(PSTR           buff,
                           UINT           len,
                           PVOID          addr)
{
    PU8             on              = (PU8)addr;

    ENTER(true);

    sprintf(buff, "%u.%u.%u.%u",
             on[0],
             on[1],
             on[2],
             on[3]);

    QUIT;
}
#endif
// ***************************************************************************
// FUNCTION
//      SnIp::stringToAddress
// PURPOSE
//
// PARAMETERS
//      PVOID addr   --
//      UINT  len    --
//      PCSTR string --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SnIp::stringToAddress(PVOID          addr,
                           UINT           size,
                           PCSTR          string)
{
    BOOL            result          = false;
    UINT            idx             = 0;
    UINT            len;
    UINT            val;
    PU8             onAddr          = (PU8)addr;
    PSTR            on;
    CHAR            tmp             [ AXLMED ];

    ENTER(true);

    if (size >= SNIP_ADDR_LEN)
    {
        strz_cpy(tmp, string, AXLMED);

        on      = tmp;
        len     = strlen(on);
        result  = true;

        while (result && ((on = strz_substrs_get_u(on, &len, '.')) != nil))
        {
            on = strz_trim(on);

            if (on && *on && (idx < SNIP_ADDR_LEN))
            {
                if ((val = atoi(on)) < 256)
                {
                    onAddr[idx] = (U8)val;
                    idx++;
                }
                else
                    result = false;
            }
            else
                result = false;
        }

        if (result && (idx != SNIP_ADDR_LEN))
        {
            result = false;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//
// PURPOSE
//
// PARAMETERS
//      PSNPACKET     pkt   --
//      PSNIPPKTPROPS props --
// RESULT
//      BOOL SnIp::disassemble             --
// ***************************************************************************
BOOL SnIp::disassemble(PSNPACKET      pkt,
                       PSNIPPKTPROPS  props,
                       UINT           flags)
{
    BOOL            result          = false;
    PU8             on;
    PSNIPHDR        hdr             = (PSNIPHDR)(((PU8)SNPKT_DATA(pkt)) + pkt->protoOffset);
    INT             hdrSize;

    ENTER(true);

//    memset(props, 0, sizeof(SNIPPKTPROPS));

    pkt->protoVersion   = hdr->version;
    pkt->eprotoType     = hdr->protocol;
    props->ttl          = hdr->ttl;
    props->tos          = hdr->tos;
    on                  = (PU8)hdr;
    hdrSize             = IH_SIZE(hdr);
    on                 += hdrSize;

    switch(pkt->protoVersion)
    {
        case SNIP_V_4:
            if (flags & SNIP_PROPS_SADDR)
                memcpy(props->sourceAddr, &hdr->saddr, SNIP_ADDR_LEN);

            if (flags & SNIP_PROPS_DADDR)
                memcpy(props->destinationAddr, &hdr->daddr, SNIP_ADDR_LEN);

//            GET_INT(props->sourceAddr, hdr->saddr);
//            GET_INT(props->destinationAddr, hdr->daddr);

            pkt->eprotoOffset   = pkt->protoOffset + IH_SIZE(hdr);
            props->payloadData  = on;
            props->payloadSize  = pkt->size - pkt->eprotoOffset;

            if (flags & SNIP_PROPS_EPROTO)
            {
                switch (pkt->eprotoType)
                {
                    case SNIP_P_ICMP:
                        result = disassembleIcmp(pkt, props);
                        break;

                    case SNIP_P_TCP:
                        result = disassembleTcp(pkt, props);
                        break;

                    case SNIP_P_UDP:
                        result = disassembleUdp(pkt, props);
                        break;

                    case SNIP_P_PRIOR:
                        result = disassemblePrior(pkt, props);
                        break;

                    default:
                        result = true;
                        break;
                }
            }
            else
                result = true;
            break;

        case SNIP_V_6:
//            result = false;
            break;

        default:
            //printf("Packet51::disassemble stage ERROR 2\n");
            break;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SnIp::disassembleUdp
// PURPOSE
//
// PARAMETERS
//      PSNPACKET     pkt   --
//      PU8           data  --
//      UINT          size  --
//      PSNIPPKTPROPS props --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SnIp::disassembleUdp(PSNPACKET      pkt,
                          PSNIPPKTPROPS  props)
{
    BOOL            result          = false;
    PSNUDPHDR       uh              = (PSNUDPHDR)props->payloadData;

    ENTER(true);

    props->destinationPort  = ntohs(uh->dest);
    props->sourcePort       = ntohs(uh->source);
    props->payloadData      = (uh + 1);

    props->payloadSize      = ntohs(uh->len) - sizeof(SNUDPHDR);
    result                  = true;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SnIp::disassembleTcp
// PURPOSE
//
// PARAMETERS
//      PSNPACKET     pkt   --
//      PU8           data  --
//      UINT          size  --
//      PSNIPPKTPROPS props --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SnIp::disassembleTcp(PSNPACKET      pkt,
                          PSNIPPKTPROPS  props)
{
    BOOL            result          = true;
    PSNTCPHDR       hdr             = (PSNTCPHDR)props->payloadData;

    ENTER(true);

    props->destinationPort  = ntohs(hdr->dest);
    props->sourcePort       = ntohs(hdr->source);

    //UINT hs                 = (int)(sizeof(SNETHHDR) + sizeof(SNIPHDR) + (hdr->th_off * 4));
    UINT hs                 = (hdr->th_off * 4);

    props->payloadData      = ((PU8)props->payloadData) + hs;
    props->payloadSize      = props->payloadSize - hs;

    result                  = true;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SnIp::disassembleIcmp
// PURPOSE
//
// PARAMETERS
//      PSNPACKET     pkt   --
//      PU8           data  --
//      UINT          size  --
//      PSNIPPKTPROPS props --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SnIp::disassembleIcmp(PSNPACKET      pkt,
                           PSNIPPKTPROPS  props)
{
    BOOL                result          = false;
    PSNICMPHDR          ih              = (PSNICMPHDR)props->payloadData;

    ENTER(true);

    props->type     = ih->type;
    props->code     = ih->code;
    props->number   = ih->un.echo.sequence;
    result          = true;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SnIp::setUdpChecksum
// PURPOSE
//
// PARAMETERS
//      PSNIPHDR  ih       --
//      PSNUDPHDR uh       --
//      UINT      dataSize --
// RESULT
//      void --
// ***************************************************************************
void SnIp::setUdpChecksum(PSNIPHDR       ih,
                          PSNUDPHDR      uh,
                          UINT           dataSize)
{
    ENTER(true);

    uh->check  = 0;

    uh->check  = wrapChecksum(checksum((unsigned char *)uh, sizeof(SNUDPHDR),
                                       checksum((unsigned char *)(uh + 1), dataSize, checksum((unsigned char *)&ih->saddr,
                                                                                                  2 * SNIP_ADDR_LEN,
                                                                                                  SNIP_P_UDP + (U32)sizeof(SNUDPHDR) + dataSize))));
    QUIT;
}
// ***************************************************************************
// FUNCTION
//      SnIp::setIcmpChecksum
// PURPOSE
//
// PARAMETERS
//      PSNIPHDR   ih       --
//      PSNICMPHDR ch       --
//      UINT       dataSize --
// RESULT
//      void --
// ***************************************************************************
void SnIp::setIcmpChecksum(PSNIPHDR       ih,
                           PSNICMPHDR     ch,
                           UINT           dataSize)
{
    ENTER(true);

    ch->checksum    = 0;

    ch->checksum    = wrapChecksum(checksum((unsigned char *)ch, sizeof(SNICMPHDR),
                                       checksum((unsigned char *)(ch + 1), dataSize, checksum((unsigned char *)&ih->saddr,
                                                                                                  2 * SNIP_ADDR_LEN,
                                                                                                  SNIP_P_ICMP + (U32)sizeof(SNICMPHDR) + dataSize))));
    QUIT;
}
// ***************************************************************************
// FUNCTION
//      SnIp::makePacket
// PURPOSE
//
// PARAMETERS
//      PSNPACKET     pkt   --
//      PSNIPPKTPROPS props --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SnIp::makePacket(PSNPACKET      pkt,
                      PSNIPPKTPROPS  props)
{
    BOOL            result          = false;
    PSNIPHDR        ih              = (PSNIPHDR)(((PU8)SNPKT_DATA(pkt)) + pkt->protoOffset);
    PSNUDPHDR       uh;
    UINT            ihSize;
    UINT            size;
    PU8             data;

    ENTER(true);

    size                = MAC_MAX(props->payloadSize, MIN_IP_PAYLOAD);

    if (!props->id)
        props->id = rand();

    ih->version         = SNIP_V_4;
    ih->ihl             = IP_HEADER_MIN_SZ;

    ihSize              = IH_SIZE(ih);
    pkt->eprotoOffset   = pkt->protoOffset + ihSize;

    ih->protocol        = pkt->eprotoType;
    ih->tos             = 0;
    ih->id              = htons(props->id);
    ih->frag_off        = 0x00;
    ih->ttl             = 255;

    memcpy(&ih->daddr, &props->destinationAddr, SNIP_ADDR_LEN);
    memcpy(&ih->saddr, &props->sourceAddr,      SNIP_ADDR_LEN);

    data                = (((PU8)ih) + ihSize);

    switch (ih->protocol)
    {
        case SNIP_P_UDP:
            uh          = (PSNUDPHDR)data;

            uh->dest    = htons(props->destinationPort);
            uh->source  = htons(props->sourcePort);

            size       += sizeof(SNUDPHDR);

            uh->len     = htons(props->payloadSize + sizeof(SNUDPHDR));

            memcpy((uh + 1), props->payloadData, props->payloadSize);

            setUdpChecksum(ih, uh, props->payloadSize);

            result      = true;
            break;

            // NOT TESTED
//        case SNIP_P_ICMP:
//            ch                      = (PSNICMPHDR)data;
//            ch->type                = props->type;
//            ch->code                = props->code;
//            ch->un.echo.sequence    = props->number;
//
//            size                   += sizeof(SNICMPHDR);
//
//            if (props->payloadSize)
//            {
//                memcpy((ch + 1), props->payloadData, props->payloadSize);
//            }
//
//            setIcmpChecksum(ih, ch, props->payloadSize);
//
//            result      = true;
//            break;

        default:
            memcpy(data, props->payloadData, props->payloadSize);
            result      = true;
            break;
    }

    if (props->paddingSize)
    {
        memset(data + size, 0xFF, props->paddingSize);

        size += props->paddingSize;
    }

    size               += ihSize;
    pkt->size          += size;
    ih->tot_len         = htons(size);

    setIpChecksum(pkt);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SnIp::defaultPktProps
// PURPOSE
//
// PARAMETERS
//      PSNIPPKTPROPS props --
// RESULT
//      void --
// ***************************************************************************
void SnIp::defaultPktProps(PSNIPPKTPROPS  props)
{
    ENTER(true);

    memset(props, 0, sizeof(SNIPPKTPROPS));

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      SnIp::setProps
// PURPOSE
//
// PARAMETERS
//      PSNPACKET     pkt   --
//      PSNIPPKTPROPS props --
//      UINT          flags --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SnIp::setProps(PSNPACKET      pkt,
                    PSNIPPKTPROPS  props,
                    UINT           flags)
{
    BOOL            result          = false;
    PSNIPHDR        ih              = (PSNIPHDR)(((PU8)SNPKT_DATA(pkt)) + pkt->protoOffset);
//    PSNUDPHDR       uh;
//    UINT            ihSize;
//    UINT            size;

    ENTER(true);

    if (flags)
    {
//        switch (ih->protocol)
//        {
//            case SNIP_P_UDP:
//                uh          = (PSNUDPHDR)(((PU8)ih) + ihSize);
//
//                uh->dest    = htons(props->destinationPort);
//                uh->source  = htons(props->sourcePort);
//
//                size       += sizeof(SNUDPHDR);
//
//                uh->len     = htons(props->payloadSize + sizeof(SNUDPHDR));
//
//                memcpy((uh + 1), props->payloadData, props->payloadSize);
//
//                setUdpChecksum(ih, uh, props->payloadSize);
//
//                result      = true;
//                break;
//
//            default:
//                break;
//        }

        if (flags & SNIP_PROPS_ID)
        {
            ih->id              = htons(props->id);
        }

        if (flags & SNIP_PROPS_SADDR)
        {
            memcpy(&ih->saddr, props->sourceAddr,      SNIP_ADDR_LEN);
        }

        if (flags & SNIP_PROPS_DADDR)
        {
            memcpy(&ih->daddr, props->destinationAddr, SNIP_ADDR_LEN);
        }

        result  = true;
    }

    if (flags & SNIP_PROPS_CHECKSUM)
    {
        setIpChecksum(pkt);
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SnIp::icmpTypeToString
// PURPOSE
//
// PARAMETERS
//      UINT type --
// RESULT
//      PCSTR  --
// ***************************************************************************
PCSTR  SnIp::icmpTypeToString(UINT           type)
{
    PCSTR               result          = "Unknown";

    ENTER(true);

    switch (type)
    {
        case SNICMP_ping_req:
            result = "request";
            break;

        case SNICMP_ping_replay:
            result = "replay";
            break;

        case SNICMP_prior:
            result = "prior";
            break;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SnIp::disassemblePrior
// PURPOSE
//
// PARAMETERS
//      PSNPACKET     pkt   --
//      PSNIPPKTPROPS props --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL SnIp::disassemblePrior(PSNPACKET      pkt,
                            PSNIPPKTPROPS  props)
{
    BOOL            result          = false;
    PSNIPPRIOR      pr              = (PSNIPPRIOR)props->payloadData;

    ENTER(true);

    props->number           = pr->number;
    props->checksum         = pr->checksum;
    props->checksumSize     = pr->coveredSize;
    props->altSize          = pr->size;
    result                  = true;

    RETURN(result);
}

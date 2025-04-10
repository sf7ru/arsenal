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

#ifndef __SKYNETIP_H__
#define __SKYNETIP_H__

#include <arsenal.h>
#include <SnPacket.h>



// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define SNIP_ADDR_LEN       4

#define SNIP_P_ICMP         1
#define SNIP_P_TCP          6
#define SNIP_P_UDP          17
#define SNIP_P_PRIOR        252
#define SNIP_P_TEST         253

#define SNIP_V_4            4
#define SNIP_V_6            6

#define SNIP_PROPS_SADDR    0x0001
#define SNIP_PROPS_DADDR    0x0002
#define SNIP_PROPS_SPORT    0x0004
#define SNIP_PROPS_DPORT    0x0008
#define SNIP_PROPS_ID       0x0010
#define SNIP_PROPS_EPROTO   0x4000
#define SNIP_PROPS_CHECKSUM 0x8000

#define SNICMP_ping_replay  0
#define SNICMP_ping_req     8

#define SNICMP_prior        125

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

//typedef U32                 SNIP4ADDRINT;

#pragma pack(1)
typedef AXPACKED(struct) __tag_SNIPPRIOR
{
    U64             number;
    U32             checksum;
    U32             coveredSize;
    U32             size;
    U32             count;
    U32             reserved        [2];
} SNIPPRIOR, * PSNIPPRIOR;
#pragma pack()




#pragma pack(1)
typedef AXPACKED(struct) __tag_SNIPHDR
{
#if (NATIVE_LE)
    U8                      ihl             :   4;
    U8                      version         :   4;
#else
    U8                      version         :   4;
    U8                      ihl             :   4;
#endif

    U8                      tos;
    U16                     tot_len;
    U16                     id;
    U16                     frag_off;
    U8                      ttl;
    U8                      protocol;
    U16                     check;
    U32                     saddr;
    U32                     daddr;

} SNIPHDR, * PSNIPHDR;
#pragma pack()

#pragma pack(1)
typedef AXPACKED(struct) __tag_SNTCPHDR
{
    __extension__ union
    {
      struct
      {
    U16 th_sport;     /* source port */
    U16 th_dport;     /* destination port */
    U32 th_seq;     /* sequence number */
    U32 th_ack;     /* acknowledgement number */
#if (NATIVE_LE)
    U8 th_x2:4;       /* (unused) */
    U8 th_off:4;      /* data offset */
#else
    U8 th_off:4;      /* data offset */
    U8 th_x2:4;       /* (unused) */
#endif
    U8 th_flags;
# define TH_FIN 0x01
# define TH_SYN 0x02
# define TH_RST 0x04
# define TH_PUSH    0x08
# define TH_ACK 0x10
# define TH_URG 0x20
    U16 th_win;       /* window */
    U16 th_sum;       /* checksum */
    U16 th_urp;       /* urgent pointer */
      };
      struct
      {
    U16 source;
    U16 dest;
    U32 seq;
    U32 ack_seq;
#if (NATIVE_LE)
    U16 res1:4;
    U16 doff:4;
    U16 fin:1;
    U16 syn:1;
    U16 rst:1;
    U16 psh:1;
    U16 ack:1;
    U16 urg:1;
    U16 res2:2;
#else
    U16 doff:4;
    U16 res1:4;
    U16 res2:2;
    U16 urg:1;
    U16 ack:1;
    U16 psh:1;
    U16 rst:1;
    U16 syn:1;
    U16 fin:1;
# endif
    U16 window;
    U16 check;
    U16 urg_ptr;
      };
    };
} SNTCPHDR, * PSNTCPHDR;
#pragma pack()

#pragma pack(1)
typedef AXPACKED(struct) __tag_SNUDPHDR
{
  __extension__ union
  {
    struct
    {
      U16 uh_sport;       /* source port */
      U16 uh_dport;       /* destination port */
      U16 uh_ulen;        /* udp length */
      U16 uh_sum;     /* udp checksum */
    };
    struct
    {
      U16 source;
      U16 dest;
      U16 len;
      U16 check;
    };
  };
} SNUDPHDR, * PSNUDPHDR;
#pragma pack()

#pragma pack(1)
typedef AXPACKED(struct) __tag_SNICMPHDR
{
  U8 type;        /* message type */
  U8 code;        /* type sub-code */
  U16 checksum;
  union
  {
    struct
    {
      U16 id;
      U16 sequence;
    } echo;         /* echo datagram */
    U32   gateway;    /* gateway address */
    struct
    {
      U16 __glibc_reserved;
      U16 mtu;
    } frag;         /* path mtu discovery */
  } un;
} SNICMPHDR, * PSNICMPHDR;
#pragma pack()

typedef struct __tag_SNIPPKTPROPS
{
    U8                      sourceAddr          [ SNIP_ADDR_LEN ];
    UINT                    sourcePort;

    U8                      destinationAddr     [ SNIP_ADDR_LEN ];
    UINT                    destinationPort;

    U8                      senderAddr          [ SNIP_ADDR_LEN ];
    U8                      targetAddr          [ SNIP_ADDR_LEN ];

    UINT                    type;
    UINT                    code;
    U64                     number;

    UINT                    id;
    UINT                    ttl;
    UINT                    tos;

    UINT                    altSize;
    UINT                    checksumSize;
    U32                     checksum;

    PVOID                   payloadData;
    UINT                    payloadSize;
    UINT                    paddingSize;
} SNIPPKTPROPS, * PSNIPPKTPROPS;

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class SnIp
{
        UINT            placeholder;

static  BOOL            disassembleIcmp         (PSNPACKET      pkt,
                                                 PSNIPPKTPROPS  props);


static  BOOL            disassemblePrior        (PSNPACKET      pkt,
                                                 PSNIPPKTPROPS  props);


static  BOOL            disassembleTcp          (PSNPACKET      pkt,
                                                 PSNIPPKTPROPS  props);

static  BOOL            disassembleUdp          (PSNPACKET      pkt,
                                                 PSNIPPKTPROPS  props);

static  void            setUdpChecksum          (PSNIPHDR       ih,
                                                 PSNUDPHDR      uhm,
                                                 UINT           dataSize);

static  void            setIcmpChecksum         (PSNIPHDR       ih,
                                                 PSNICMPHDR     uh,
                                                 UINT           dataSize);

static  U32             checksum                (PU8            buf,
                                                 UINT           nbytes,
                                                 U32            sum);

static  U32             wrapChecksum            (U32            sum);

public:

static  UINT            getProtocol             (PSNPACKET      pkt);

//static  U16             checksum                (U16            initial,
//                                                 PVOID          data,
//                                                 INT            len);

static  void            setSize                 (PSNPACKET      pkt,
                                                 UINT           size);

static  void            setIpChecksum             (PSNPACKET      pkt);

static  BOOL            addressCompare          (PVOID          addr1,
                                                 PVOID          addr2);

static  void            addressToString         (PSTR           buff,
                                                 UINT           len,
                                                 PVOID          addr);

static  BOOL            setProps                (PSNPACKET      pkt,
                                                 PSNIPPKTPROPS  props,
                                                 UINT           flags);

static  BOOL            stringToAddress         (PVOID          addr,
                                                 UINT           size,
                                                 PCSTR          string);

static  BOOL            disassemble             (PSNPACKET      pkt,
                                                 PSNIPPKTPROPS  props,
                                                 UINT           flags = -1);

static  BOOL            makePacket              (PSNPACKET      pkt,
                                                 PSNIPPKTPROPS  props);

static  void            defaultPktProps         (PSNIPPKTPROPS  props);


static  PCSTR           icmpTypeToString        (UINT           type);

static  BOOL            addrIsMcast             (PVOID          addr)
        { return ((*((PU8)addr) & 224) == 224); }

};

#endif // __SKYNETIP_H__

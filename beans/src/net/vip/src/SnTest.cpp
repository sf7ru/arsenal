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
#include <SnTest.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      SnTest::disassemble
// PURPOSE
//
// PARAMETERS
//      PSNPACKET    pkt   --
//      PSNTESTPROPS props --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SnTest::disassemble(PSNPACKET      pkt,
                         PSNTESTPROPS   props)
{
    BOOL            result          = false;
    SNETHPKTPROPS   ethProps;
    SNIPPKTPROPS    ipProps;
    PSNTESTPROPS    on;

    ENTER(true);

    if (  SnEth::disassemble(pkt, &ethProps)    &&
          (SNETH_P_IP == pkt->protoType)        &&
          SnIp::disassemble(pkt, &ipProps)      )
    {
        on  = (PSNTESTPROPS)ipProps.payloadData;

        if (  (ipProps.payloadSize >= sizeof(SNTESTPROPS))  &&
              (SN_TEST_MAGIC == on->magic)                  )
        {
            memcpy(props, on, sizeof(SNTESTPROPS));
            result = true;
        }
    }

    RETURN(result);
}
//// ***************************************************************************
//// FUNCTION
////      SnTest::setDestAndTargetMac
//// PURPOSE
////
//// PARAMETERS
////      PSNPACKET pkt --
////      PU8    mac --
//// RESULT
////      BOOL
////            true:   Ok
////            false:  Error
//// ***************************************************************************
//BOOL SnTest::setDestAndTargetMac(PSNPACKET      pkt,
//                                PU8         mac)
//{
//    BOOL            result          = false;
//    SNTESTPROPS   props;
//
//    ENTER(true);
//
//    disassemble(pkt, &props);
//
//    SnEth::setDestMac(pkt, mac);
//
//    if (props.targetHwAddr)
//    {
//        memcpy(props.targetHwAddr, mac, props.hwAddrLen);
//
//        result = true;
//    }
//
//    RETURN(result);
//}
// ***************************************************************************
// FUNCTION
//      SnTest::makePacket
// PURPOSE
//
// PARAMETERS
//      PSNPACKET      pkt        --
//      PSNETHPKTPROPS ethProps   --
//      PSNIPPKTPROPS  ipProps    --
//      PSNTESTPROPS   testProps  --
//      UINT           packetSize --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL SnTest::makePacket(PSNPACKET      pkt,
                        PSNETHPKTPROPS ethProps,
                        PSNIPPKTPROPS  ipProps,
                        PSNTESTPROPS   testProps,
                        UINT           extraSize,
                        UINT           protoType)
{
    BOOL            result          = false;

    ENTER(true);

    if (SnEth::makePacket(pkt, ethProps->psmac, ethProps->pdmac, SNETH_P_IP))
    {
        pkt->eprotoType         = protoType ? protoType : SNIP_P_TEST;
        testProps->magic        = SN_TEST_MAGIC;
        ipProps->payloadData    = testProps;
        ipProps->payloadSize    = sizeof(SNTESTPROPS);
        ipProps->paddingSize    = extraSize;
        result                  = SnIp::makePacket(pkt, ipProps);
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SnTest::setNumber
// PURPOSE
//
// PARAMETERS
//      PSNPACKET  pkt    --
//      SNCOUNTINT number --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SnTest::setNumber(PSNPACKET      pkt,
                       PSNIPPKTPROPS  ipProps,
                       SNCOUNTINT     number)
{
    BOOL            result          = false;
    PSNTESTPROPS    on;

    ENTER(true);

    on          = (PSNTESTPROPS)ipProps->payloadData;
    on->number  = number;

    SnIp::setIpChecksum(pkt);

    result      = true;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SnTest::setNumber
// PURPOSE
//
// PARAMETERS
//      PSNPACKET     pkt     --
//      PSNIPPKTPROPS ipProps --
//      SNCOUNTINT    number  --
//      U32           param   --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL SnTest::setPayload(PSNPACKET      pkt,
                        PSNIPPKTPROPS  ipProps,
                        PVOID          data)
{
    BOOL            result          = false;
//    PSNTESTPROPS    on;

    ENTER(true);

    memcpy(ipProps->payloadData, data, ipProps->payloadSize);

    SnIp::setIpChecksum(pkt);

    result      = true;

    RETURN(result);
}

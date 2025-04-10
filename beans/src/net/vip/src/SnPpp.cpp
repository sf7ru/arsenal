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

#include <stdlib.h>
#include <string.h>

#include <axhashing.h>

#include <SnPpp.h>
#include <SnTypes.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define PPPGOODFCS16        0xf0b8

#define FLAG                0x7E
#define ESC                 0x7D
#define ESC_BIT             0x20
#define ADDR_ALL            0xFF
#define CTRL_PPP            0x03

#define ID_CODE_LEN_SZ      4
#define OPT_HEAD_SZ         2

// LCP Code
#define LCP_CONF_REQ        1
#define LCP_CONF_ACK        2
#define LCP_CONF_NAK        3
#define LCP_CONF_REJ        4
#define LCP_TERM_REQ        5
#define LCP_TERM_ACK        6
#define LCP_CODE_REJ        7
#define LCP_PRTO_REJ        8
#define LCP_ECHO_REQ        9
#define LCP_ECHO_ACK        10

// LCP Option type
#define CONF_OPT_MRU        1
#define CONF_OPT_ACCM       2
#define CONF_OPT_AUTH       3
#define CONF_OPT_MAGIC      5
#define CONF_OPT_PFC        7
#define CONF_OPT_ACFC       8
#define CONF_OPT_CB         13

#define AUTH_PAP            0xC023


#define PROTO_LCP           0x21C0
#define PROTO_PAP           0x23C0
#define PROTO_CHAP          0x23C2
#define PROTO_IPCP          0x2180
#define PROTO_CCP           0xFD80


enum
{
    STAGE_flag              = 0,
    STAGE_addr,
    STAGE_ctrl,
    STAGE_proto1,
    STAGE_proto2,
    STAGE_code,
    STAGE_id,
    STAGE_len1,
    STAGE_len2,
    STAGE_body,
    STAGE_fcs1,
    STAGE_fcs2,
    STAGE_fcs3,
    STAGE_fcs4
};

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

SnPpp::SnPpp()
{
    dev     = NULL;
    gotEscape  = false;

    TO      = 1000;
}
SnPpp::~SnPpp()
{
    close();
}
void SnPpp::close()
{
    if (frame)
    {
        FREE(frame);
        frame = NULL;
    }
}
// ***************************************************************************
// FUNCTION
//      SnPpp::open
// PURPOSE
//
// PARAMETERS
//      A7Device * dev    --
//      PCSTR      user   --
//      PCSTR      passwd --
//      UINT       mru    --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SnPpp::open(AXDevice *     dev,
                 PCSTR          user,
                 PCSTR          passwd,
                 UINT           mru)
{
    BOOL            b_result        = false;

    ENTER(true);

    if ((this->frame = (PU8)MALLOC(mru)) != NULL)
    {
        this->dev       = dev;
        this->user      = user;
        this->passwd    = passwd;
        this->onFrame   = frame;
        this->stage     = STAGE_flag;
        this->nextId    = 1;

        cliCfg.mru      = mru;
        cliCfg.sendCtrl = true;
        cliCfg.sendAddr = true;
        cliCfg.accm     = -1;

        memcpy(&svrCfg, &cliCfg, sizeof(svrCfg));


        b_result        = true;
    }

    RETURN(b_result);
}
//BOOL SnPpp::getFrame()
//{
//    BOOL            result          = false;
//
//    ENTER(true);
//
//    while (dev->read())
//    {
//
//    }
//
////    if ((result = serial.read(buff, AXLLONG, TO)) > 0)
////    {
////        *(buff + result) = 0;
////        //printf("%s\n", (PSTR)buff);
////        la6_dump_print((PSTR)"DATA ", buff, result);
////    }
//
//    RETURN(result);
//}

// ***************************************************************************
// FUNCTION
//      SnPpp::readEscaped
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SnPpp::readEscaped()
{
    BOOL result = false;

    while (!result && (dev->read(onFrame, 1, TO) > 0))
    {
        if (gotEscape)
        {
            *onFrame   ^= ESC_BIT;
            result      = true;
            gotEscape   = false;
        }
        else if (ESC == *onFrame)
        {
            gotEscape   = true;
        }
        else
            result      = true;
    }

    return result;
}
// ***************************************************************************
// FUNCTION
//      SnPpp::readFrame
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SnPpp::readFrame()
{
    BOOL        result          = false;

    ENTER(true);

    while (!result && readEscaped())
    {
        ////printf("%.2X ", *onFrame);

        if (STAGE_flag == stage)
        {
            onFrame     = frame;
            onChecksum  = FCS16_INITIAL;
            frameSize   = 0;
        }
        else
            onChecksum    = fcs16_update(onChecksum, onFrame, 1);

        switch (stage)
        {
            case STAGE_flag:
                if (FLAG == *onFrame)
                {
                    //printf("- FLAG\n");
                    stage++;
                }
                break;

            case STAGE_addr:
                if (ADDR_ALL == *onFrame)
                {
                    //printf("- ADDR\n");
                    stage++;
                }
                else if (FLAG != *onFrame)
                {
                    stage = STAGE_flag;
                }
                else
                    //printf("- 2nd FLAG\n");
                break;

            case STAGE_ctrl:
                if (CTRL_PPP == *onFrame)
                {
                    //printf("- CTRL\n");
                    stage++;
                }
                else
                    stage = STAGE_flag;
                break;

            case STAGE_proto1:
                stage++;
                onFrame++;
                break;

            case STAGE_proto2:
                frameProto = *(PU16)(onFrame - 1);

                switch (frameProto)
                {
                    case PROTO_CHAP:
                    case PROTO_PAP:
                    case PROTO_LCP:
                        //printf("- PROTO\n");
                        onFrame = frame;
                        stage++;
                        break;

                    default:
                        stage = STAGE_flag;
                        break;
                }
                break;

            case STAGE_code:
                //printf("- CODE\n");
                frameCode = *onFrame;
                stage++;
                break;

            case STAGE_id:
                //printf("- ID\n");
                frameId = *onFrame;
                stage++;
                break;

            case STAGE_len1:
                stage++;
                onFrame++;
                break;

            case STAGE_len2:
                frameSize   = (*(PU16)(onFrame - 1));
                frameSize   = ntohs(frameSize) - ID_CODE_LEN_SZ;
                onLeft      = frameSize;
                onFrame     = frame;
                //printf("- BODY LEN (%d bytes)\n", onLeft);
                stage++;
                break;

            case STAGE_body:
                if (!onLeft || !(--onLeft))
                {
                    //printf("- BODY\n");
                    stage++;
                }
                onFrame++;
                break;

            case STAGE_fcs1:
                stage++;
                onFrame++;
                break;

            case STAGE_fcs2:
                //printf("- CHECKSUM (%.4X vs %.4X)\n", (*(PU16)(onFrame - 1)), onChecksum);

                if (PPPGOODFCS16 == onChecksum)
                {
                    result  = true;
                }
                stage   = STAGE_flag;
                break;
//            case STAGE_fcs3:
//            case STAGE_fcs4:


            default:
                stage   = STAGE_flag;
                break;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SnPpp::sendEscaped
// PURPOSE
//
// PARAMETERS
//      PVOID data --
//      UINT  size --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL SnPpp::sendEscaped(PVOID   data,
                        UINT    size)
{
    BOOL        result      = true;
    //U8        esc         = ESC;
    PU8       on          = (PU8)data;
    PU8       end;
//    U8        code;

    for (end = on + size; result && (on < end); on++)
    {
//        if (  (*on == FLAG) || (*on == ESC)                     ||
//              ((*on < ESC_BIT) && (svrCfg.accm & (1 << *on)))   )
//        {
//            code    = *on ^ ESC_BIT;
//            result = (dev->write(&esc,  1, TO) > 0)  &&
//                     (dev->write(&code, 1, TO) > 0)  ;
//        }
//        else
            result = (dev->write(on, 1, TO) > 0);
    }

    return true;
}
BOOL SnPpp::sendFrame(UINT           proto,
                      UINT           code,
                      UINT           id,
                      PVOID          data,
                      UINT           len)
{
    BOOL            result          = false;
    U8            flag            = FLAG;

#pragma pack(1)
    struct
    {
        U8        addr;
        U8        ctrl;
        U16      proto;
        U8        code;
        U8        id;
        U16      len;
    } hd;
#pragma pack()

    ENTER(true);

    hd.addr     = ADDR_ALL;
    hd.ctrl     = CTRL_PPP;
    hd.proto    = proto;
    hd.code     = code;
    hd.id       = id;
    hd.len      = htons(len + ID_CODE_LEN_SZ);

    if (  (dev->write(&flag, 1, TO) > 0)    &&
          sendEscaped(&hd, sizeof(hd))      &&
          sendEscaped(data, len)            &&
          (dev->write(&flag, 1, TO) > 0)    )
    {
        result = true;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      SnPpp::handleLcpConfigReq
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      void --
// ***************************************************************************
void SnPpp::handleLcpConfigReq()
{
    UINT        code    = 0;
    UINT        left    = frameSize;
    PU8       on      = frame;
    PU8       onOpt;
    UINT        opt;
    UINT        len;
    UINT        val;
    PPPCONFIG   newCfg;

//    //printf("SnPpp::lcpConfig stage %d\n", frameSize);
//    la6_dump_print((PSTR)"body ", on, left);

    memset(&newCfg, 0, sizeof(newCfg));

    newCfg.accm = -1;

    while (!code && (left > OPT_HEAD_SZ))
    {
        onOpt   = on;
        opt     = *(on++);
        len     = *(on++);

        if (left >= len)
        {
            left -= len;
            len  -= OPT_HEAD_SZ;

//            //printf("opt = %d, len = %d\n", opt, len);
//            la6_dump_print((PSTR)"val ", on, len);

            switch (opt)
            {
                case CONF_OPT_MRU:
                    newCfg.mru = read_int(on, len, true);
                    break;

                case CONF_OPT_ACCM:
                    newCfg.accm = read_int(on, len, true);
                    break;

                case CONF_OPT_AUTH:
                    val = read_int(on, len, true);
                    switch (val)
                    {
                        case AUTH_PAP:
                            newCfg.auth = val;
                            break;

                        default:
                            code = LCP_CONF_NAK;
                            break;
                    }
                    break;

                case CONF_OPT_PFC:
                case CONF_OPT_ACFC:
                    code = LCP_CONF_NAK;
                    break;


                 default:
                     code = LCP_CONF_REJ;
                     break;
            }

            on += len;
        }
        else
        {
            //printf("wrong len!!!\n");
            code = LCP_CONF_REJ;
        }
    }

    if (!code)
    {
        //memcpy(&svrCfg, &newCfg, sizeof(svrCfg));
        sendFrame(PROTO_LCP, LCP_CONF_ACK, frameId, frame, frameSize);
    }
    else
        sendFrame(PROTO_LCP, code, frameId, onOpt, len + OPT_HEAD_SZ);
}
// ***************************************************************************
// FUNCTION
//      SnPpp::read
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      INT --
// ***************************************************************************
INT SnPpp::read()
{
    INT             result          = 0;

    ENTER(true);

    if (readFrame())
    {
//        //printf("SnPpp::read stage 1 frameProto = %X\n", frameProto);

        switch (frameProto)
        {
            case PROTO_LCP:
                switch (frameCode)
                {
                    case LCP_CONF_REQ:
                        handleLcpConfigReq();
                        break;

                    default:
                        break;
                }
                break;
        }
    }

    RETURN(result);
}

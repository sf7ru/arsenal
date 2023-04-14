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

#include <Mqtt.h>
#include <string.h>
#include <stdio.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define PROTOCOL_NAME       "MQIsdp"    //
#define PROTOCOL_VERSION    3U          // version 3.0 of MQTT
#define CLEAN_SESSION       (1U<<1)
#define KEEPALIVE           30U         // specified in seconds
#define MESSAGE_ID          1U // not used by QoS 0 - value must be > 0

/* Macros for accessing the MSB and LSB of a U16 */
#define MSB(A) ((U8)((A & 0xFF00) >> 8))
#define LSB(A) ((U8)(A & 0x00FF))


#define SET_MESSAGE(M) ((U8)(M << 4))
#define GET_MESSAGE(M) ((U8)(M >> 4))

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef enum
{
    CONNECT = 1,
    CONNACK,
    PUBLISH,
    PUBACK,
    PUBREC,
    PUBREL,
    PUBCOMP,
    SUBSCRIBE,
    SUBACK,
    UNSUBSCRIBE,
    UNSUBACK,
    PINGREQ,
    PINGRESP,
    DISCONNECT 
} connect_msg_t;

typedef enum
{
    Connection_Accepted,
    Connection_Refused_unacceptable_protocol_version,
    Connection_Refused_identifier_rejected,
    Connection_Refused_server_unavailable,
    Connection_Refused_bad_username_or_password,
    Connection_Refused_not_authorized
} connack_msg_t;


typedef struct fixed_header_t
{
    U16     retain             : 1;
    U16     Qos                : 2;
    U16     DUP                : 1;
    U16     connect_msg_t      : 4;
    U16     remaining_length   : 8;    
}fixed_header_t;
BOOL Mqtt::_connect(UINT           TO)
{
    BOOL        result          = false;

    ENTER(true);

    if ((sock = axssocket_connect(server_ip, port, *iface ? iface : nil, 0, TO)) != nil)
    {
        U8 var_header[] =
               {
                   0,                         // MSB(strlen(PROTOCOL_NAME)) but 0 as messages must be < 127
                   (U8)strlen(PROTOCOL_NAME),     // LSB(strlen(PROTOCOL_NAME)) is redundant
                   'M','Q','I','s','d','p',
                   PROTOCOL_VERSION,
                   CLEAN_SESSION,
                   MSB(KEEPALIVE),
                   LSB(KEEPALIVE)
               };

        // set up payload for connect message
        int payloadSize = 2 + (int)strlen(clientid);

        // fixed header: 2 bytes, big endian
        U8 fixed_header[] = { SET_MESSAGE(CONNECT), U8(sizeof(var_header) + payloadSize) };
//        printf("fixed_header[] = %X %X\n", fixed_header[0], fixed_header[1]);
        
//      fixed_header_t  fixed_header = { .QoS = 0, .connect_msg_t = CONNECT, .remaining_length = sizeof(var_header)+strlen(broker->clientid) };

        //U8 packet[sizeof(fixed_header)+sizeof(var_header) + payloadSize];
        int packetSize = sizeof(fixed_header) + sizeof(var_header) + payloadSize;

        //memset(packet,0,sizeof(packet));
        memcpy(tmpBuff,fixed_header,sizeof(fixed_header));
        memcpy(tmpBuff+sizeof(fixed_header),var_header,sizeof(var_header));

        PU8 onPayload = tmpBuff+sizeof(fixed_header)+sizeof(var_header);

        onPayload[0] = 0;
        onPayload[1] = (U8)strlen(clientid);
        memcpy(&onPayload[2],clientid,strlen(clientid));

        // send Connect message
        if (axdev_write(sock, tmpBuff, packetSize, TO) == packetSize)
        {
            U8 buffer[4];
            // It's OK read here
            long sz = axdev_read(sock, buffer, sizeof(buffer), TO);  // wait for CONNACK
                    // printf("_connect buffer size is %ld\n",sz);
                    // printf("%2x:%2x:%2x:%2x\n",(U8)buffer[0],(U8)buffer[1],(U8)buffer[2],(U8)buffer[3]);

            if (sz > 0)
            {
                if ( (GET_MESSAGE(buffer[0]) == CONNACK) && ((sz-2)==buffer[1]) && (buffer[3] == Connection_Accepted) )
                {
//                    printf("-- Connected to MQTT Server at %s:%4d\n",server_ip, port );

                    wrapper->setDev(sock);
                    
                    lastKA      = axtime_get_monotonic();
                    result      = true;
                }
                else
                    disconnect();
            }
            else
                disconnect();
        }
        else
            disconnect();
    }

    RETURN(result);
}

BOOL Mqtt::connect(PCSTR          client,
                   PCSTR          iface,
                   PCSTR          server_ip,
                   UINT           port,
                   UINT           TO)
{
    BOOL        result          = false;

    ENTER(true);

    this->port = port;

    if (iface)
        strz_cpy(this->iface, iface, sizeof(this->iface));

    strz_cpy(this->server_ip, server_ip, sizeof(this->server_ip));
    strz_cpy(this->clientid, client, sizeof(this->clientid));

    if (_connect(TO))
    {
        if ((wrapper = new PAXDEVWrapper(sock)) != nil)
        {
            result = inBuff.open2(wrapper, inBuffData, sizeof(inBuffData), EOLMODE_any);
        }
    }

    RETURN(result);
}
BOOL Mqtt::_subscribe(PMQTTSUBSRIBTION sub,
                      UINT             TO)
{
    BOOL        result          = false;
    UINT        topicLen;

    ENTER(true);

    U8 var_header[] = { MSB(MESSAGE_ID), LSB(MESSAGE_ID) };    // appended to end of PUBLISH message

    topicLen = (UINT)strlen(sub->topic);

    // utf topic
    //U8 utf_topic[2 + topicLen + 1 ]; // 2 for message size
    int  topicSize = 2 + topicLen + 1;

    U8 fixed_header[] = { SET_MESSAGE(SUBSCRIBE) | 0x2, U8(sizeof(var_header) + topicSize) };
    
    //    fixed_header_t  fixed_header = { .QoS = 0, .connect_msg_t = SUBSCRIBE, .remaining_length = sizeof(var_header)+strlen(utf_topic) };

    int packetSize  = sizeof(fixed_header) + sizeof(var_header) + topicSize;

    memset(tmpBuff, 0, packetSize);
    memcpy(tmpBuff, &fixed_header, sizeof(fixed_header));
    memcpy(tmpBuff + sizeof(fixed_header), var_header, sizeof(var_header));

    PU8 onTopic = tmpBuff + sizeof(fixed_header) + sizeof(var_header);

    // set up topic payload
    onTopic[0] = 0;                       // MSB(topicLen);
    onTopic[1] = LSB(topicLen);
    memcpy((char *) &onTopic[2], sub->topic, topicLen);
    onTopic[2 + topicLen] = sub->qos;

//    memcpy(tmpBuff + sizeof(fixed_header) + sizeof(var_header), utf_topic, sizeof(utf_topic));

    //strz_dump("SUB REQ ", packet, sizeof(packet));

    if (axdev_write(sock, tmpBuff, packetSize, TO) == (INT)packetSize)
    {
        //printf("sent SUBSCRIBE for %s\n", sub->topic);
        result = true;
    }

    RETURN(result);
}
BOOL Mqtt::subscribe(PMQTTSUBSRIBTION subssArray,
                     UINT           arraySize)
{
    BOOL        result          = false;

    ENTER(true);
    
    subscribeIndex      = 0;
    subscribeSent       = false;
    subscriptions       = subssArray;
    subscriptionsSize   = arraySize;
    result              = true; //_subscribe(TO);

    RETURN(result);
}
static UINT readSize(PU8             on, 
                    unsigned long &  size)
{
    UINT        result = 0;

    do
    {
        result <<= 7;
        result  |= (*on & 0x7F);

        size++;

    } while (*(on++) & 0x80);

    return result;
}

INT Mqtt::_parse_PUBLISH(PMQTTMESSAGE   msg,
                         long &         sz)
{
    INT             result      = 0;
    U32             topicSize;
    unsigned long   i           = 1;
    PU8             buff        = (PU8)inBuff.getData();

    result      = readSize(buff + i, i);
    topicSize   = (buff[2] << 8) + buff[3];

    int topicLen = MAC_MIN(topicSize, (sizeof(msg->topic) - 1));
    memcpy(msg->topic, &buff[4], topicLen);
    *(msg->topic + topicLen) = 0;

    i       += (2 + topicSize);
    result  -= (2 + topicSize);

    if (((buff[0] >> 1) & 0x03) > QoS0)
    {
        msg->messageId = (buff[4 + topicSize] << 8) + buff[4 + topicSize + 1];
        //printf("Message ID is %d\n", messageId);
        i += 2; // 2 extra for msgID
        // if QoS1 the send PUBACK with message ID
        U8 puback[4] = { SET_MESSAGE(PUBACK), 2, buff[4 + topicSize], buff[4 + topicSize + 1] };

        if (axdev_write(sock, puback, sizeof(puback), 0) == (INT) sizeof(puback))
        {
            // das gut
        }
    }
    else
        msg->messageId = 0;

    memcpy(msg->message, &buff[i], result);

    *(msg->message + result) = 0;

    sz = i + result;

    return result;
}
INT Mqtt::_parse_PINGRESP(PMQTTMESSAGE   msg,
                          long &         sz)
{
    sz = 2;

    return 0;
}
INT Mqtt::_parse_PINGREQ(PMQTTMESSAGE   msg,
                         long &         sz)
{
    U8 fixed_header[] = { SET_MESSAGE(PINGRESP), 0 };

    if (axdev_write(sock, fixed_header, sizeof(fixed_header), 100) == (INT)sizeof(fixed_header))
    {
    }

    sz = 2;
     
    return 0;
}
INT Mqtt::_parse_SUBACK(PMQTTMESSAGE   msg,
                        long &         sz)
{
// if ((GET_MESSAGE(buffer[0]) == SUBACK) && ((sz - 2) == buffer[1]) && (buffer[2] == MSB(MESSAGE_ID)) &&
//     (buffer[3] == LSB(MESSAGE_ID)))
// {
//     printf("Subscribed to MQTT Service %s with QoS %d\n", sub->topic, sub->qos);

//     result = true;
// }

    PU8     buff = (PU8)inBuff.getData();

    if (((sz - 2) >= buff[1]) && (buff[2] == MSB(MESSAGE_ID)) && (buff[3] == LSB(MESSAGE_ID)))
    {
        if (subscribeIndex != -1)
        {
            //printf("Subscribed to MQTT Service %s\n", subscriptions[subscribeIndex].topic);

            subscribeIndex++;
            subscribeSent = false;

            if ((UINT)subscribeIndex >= subscriptionsSize)
            {
                //printf("@@@@@@@@@ Subscription over\n");
                subscribeIndex = -1;
            }
        }
    }

    sz = 5;

    return 0;
}
INT Mqtt::_parse_PUBACK(PMQTTMESSAGE   msg,
                        long &         sz)
{
        // if (qos == QoS1)
        // {
        //     // expect PUBACK with MessageID
        //     U8   buffer[4];

        //     // FIXME move this block to 
        //     long sz = axdev_read(sock, buffer, sizeof(buffer), TO);  // wait for SUBACK

        //     //    printf("buffer size is %ld\n",sz);
        //     //    printf("%2x:%2x:%2x:%2x:%2x\n",(U8)buffer[0],(U8)buffer[1],(U8)buffer[2],(U8)buffer[3],(U8)buffer[4]);

        //     if ((GET_MESSAGE(buffer[0]) == PUBACK) && ((sz - 2) == buffer[1]) && (buffer[2] == MSB(pubMsgID)) &&
        //         (buffer[3] == LSB(pubMsgID)))
        //     {
        //         //printf("Published to MQTT Service %s with QoS1\n", topic);

        //         result = 1;
        //     }
        //     else
        //         result = 0;
        // }
        // else
        //     result = 1;

    sz = 4;

    return 0;
}


INT Mqtt::turn(PMQTTMESSAGE   msg,
               UINT           TO)
{
#define MAC_MSG(a)  case (a): result = _parse_##a(msg, sz); break

    INT         result          = 0;
    long        sz;

    ENTER(true);

    if (sock)
    {
        if ((subscribeIndex != -1) && !subscribeSent)
        {
            subscribeSent = _subscribe(&subscriptions[subscribeIndex], 100);
        }

        while ((sz = inBuff.read(TO)) > 0)
        {
            PU8 buff    = (PU8)inBuff.getData();
            //int was     = sz;

            //strz_dump("RECV: ", buff, sz);
            lastKA = axtime_get_monotonic();

            switch (GET_MESSAGE(buff[0]))
            {
                MAC_MSG(PUBLISH);
                MAC_MSG(PINGRESP);
                MAC_MSG(PINGREQ);
                MAC_MSG(SUBACK);
                MAC_MSG(PUBACK);

                default:
                    result  = 0;
                    break;
            }

            //printf("sz  = %d, was = %d\n", (int)sz, (int)was);
            inBuff.purge(sz);
        }

        if (sz == -1)
        {
            perror("Mqtt::receive ");
            result = -1;
        }
    }

    RETURN(result);

#undef  MAC_MSG    
}

INT Mqtt::publish(PCSTR          topic,
                  PCSTR          msg,
                  QoS            qos,
                  UINT           flags,
                  UINT           TO)
{
#define VAR_SIZE_SZ(l)    ((l > 127) ? 2 : 1)
#define VAR_SIZE_0(l)     (U8)((l > 127) ? (l | 0x80) : len)
#define VAR_SIZE_1(l)     (U8)((l > 127) ? (l >> 7) : 0)

    INT         result          = -1;

    ENTER(true);

    int msgLen = strlen(msg);
    int topicLen = strlen(topic);

    //printf("< '%s' '%s'\n", topic, msg);

    if (qos == QoS0)
    {
        // utf topic
        //U8 utf_topic[2 + topicLen]; // 2 for message size QoS-0 does not have msg ID
        int utf_topicSize = 2 + topicLen; 

        unsigned len        =  utf_topicSize + msgLen;
        unsigned headerLen  = sizeof(U8);
                    headerLen += VAR_SIZE_SZ(len);

        U8 fixed_header[] = { U8(SET_MESSAGE(PUBLISH) | 
                                            (qos << 1) | 
                            (flags & MQTT_FLAG_RETAIN ? 1 : 0)), VAR_SIZE_0(len), VAR_SIZE_1(len) };
        //    fixed_header_t  fixed_header = { .QoS = 0, .connect_msg_t = PUBLISH, .remaining_length = sizeof(utf_topic)+msgLen };

        int packetLen = headerLen + utf_topicSize + msgLen;

        if (MQTT_BUFF_SIZE >= packetLen)
        {
            memset(outBuff, 0, packetLen);
            memcpy(outBuff, &fixed_header, headerLen);
            //memcpy(outBuff + headerLen, utf_topic, sizeof(utf_topic));
            PU8 onTopic = outBuff + headerLen;

            // set up topic payload
            onTopic[0] = 0;                       // MSB(topicLen);
            onTopic[1] = LSB(topicLen);
            memcpy((char *) &onTopic[2], topic, topicLen);

            memcpy(outBuff + headerLen + utf_topicSize, msg, msgLen);

            result = axdev_write(sock, outBuff, packetLen, TO) == packetLen;
        }
        else
        {
            printf("MQTT packet too long  %d vs %d max\n", packetLen, MQTT_BUFF_SIZE);
        }
    }
    else
    {
        pubMsgID++;
        // utf topic
        //U8 utf_topic[2 + topicLen + 2]; // 2 extra for message size > QoS0 for msg ID
        int utf_topicSize = 2 + topicLen + 2; 


        unsigned len        =  utf_topicSize + msgLen;
        unsigned headerLen  = (sizeof(U8) + VAR_SIZE_SZ(len));
        U8 fixed_header[] = { U8(SET_MESSAGE(PUBLISH) | (qos << 1)), VAR_SIZE_0(len), VAR_SIZE_1(len) };

        int packetLen = headerLen + utf_topicSize + msgLen;

        if (MQTT_BUFF_SIZE >= packetLen)
        {
            memset(outBuff, 0, packetLen);
            memcpy(outBuff, &fixed_header, headerLen);
            //memcpy(outBuff + headerLen, utf_topic, sizeof(utf_topic));
            PU8 onTopic = outBuff + headerLen;
            // set up topic payload
            onTopic[0] = 0;                       // MSB(topicLen);
            onTopic[1] = LSB(topicLen);
            memcpy((char *) &onTopic[2], topic, topicLen);
            onTopic[utf_topicSize - 2] = MSB(pubMsgID);
            onTopic[utf_topicSize - 1] = LSB(pubMsgID);

            memcpy(outBuff + headerLen + utf_topicSize, msg, msgLen);

            if (axdev_write(sock, outBuff, packetLen, TO) == (INT) packetLen)
            {
                // Das ist gut
            }
        }
        else
            printf("MQTT packet too long  %d vs %d max\n", packetLen, MQTT_BUFF_SIZE);
    }

    RETURN(result);
}

void Mqtt::disconnect()
{
    U8 fixed_header[] = { SET_MESSAGE(DISCONNECT), 0};

    if (sock)
    {
        if (subscriptions)
        {
            axdev_write(sock, fixed_header, sizeof(fixed_header), 1000);
        }

        sock = axssocket_close(sock);
    }
}
BOOL Mqtt::reconnect(UINT TO)
{
    BOOL        result          = false;

    ENTER(true);

    disconnect();

    if (_connect(TO))
    {
        if (subscriptions)
        {
            subscribeIndex = 0;
        }

        result = true;
    }

    RETURN(result);
}
BOOL Mqtt::keepAlive(UINT           TO)
{
    BOOL        result          = false;
    AXTIME      now;
//    int         rd;

    ENTER(true);

    if (sock)
    {
        now = axtime_get_monotonic();

        if ((now - lastKA) >= (kaTimeout * 1000))
        {
            if ((now - lastKA) <= ((kaTimeout * 2) * 1000))
            {
                U8 fixed_header[] = { SET_MESSAGE(PINGREQ), 0 };

                axdev_write(sock, fixed_header, sizeof(fixed_header), TO);
                
                result = true;
            }
            // else
            //     printf("ERR TO %d %d\n", (int)now, (int)lastKA);

        }
        else
            result = true;
    }
    // else
    //     printf("ERR sock is null\n");

    // if (!result)
    // {
    //     printf("@@@@@@@@@@@@@@@@@2 KA !!!!!!!!!!!!\n");
    // }

    RETURN(result);
}
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
                   strlen(PROTOCOL_NAME),     // LSB(strlen(PROTOCOL_NAME)) is redundant
                   'M','Q','I','s','d','p',
                   PROTOCOL_VERSION,
                   CLEAN_SESSION,
                   MSB(KEEPALIVE),
                   LSB(KEEPALIVE)
               };

        // set up payload for connect message
        U8 payload[2+strlen(clientid)];
        payload[0] = 0;
        payload[1] = strlen(clientid);
        memcpy(&payload[2],clientid,strlen(clientid));

        // fixed header: 2 bytes, big endian
        U8 fixed_header[] = { SET_MESSAGE(CONNECT), sizeof(var_header)+sizeof(payload) };
//      fixed_header_t  fixed_header = { .QoS = 0, .connect_msg_t = CONNECT, .remaining_length = sizeof(var_header)+strlen(broker->clientid) };

        U8 packet[sizeof(fixed_header)+sizeof(var_header)+sizeof(payload)];

        memset(packet,0,sizeof(packet));
        memcpy(packet,fixed_header,sizeof(fixed_header));
        memcpy(packet+sizeof(fixed_header),var_header,sizeof(var_header));
        memcpy(packet+sizeof(fixed_header)+sizeof(var_header),payload,sizeof(payload));

        // send Connect message
        if (axdev_write(sock, packet, sizeof(packet), TO) == (INT)sizeof(packet))
        {
            U8 buffer[4];
            long sz = axdev_read(sock, buffer, sizeof(buffer), TO);  // wait for CONNACK
//                    printf("buffer size is %ld\n",sz);
//                    printf("%2x:%2x:%2x:%2x\n",(U8)buffer[0],(U8)buffer[1],(U8)buffer[2],(U8)buffer[3]);

            if ( (GET_MESSAGE(buffer[0]) == CONNACK) && ((sz-2)==buffer[1]) && (buffer[3] == Connection_Accepted) )
            {
                //printf("Connected to MQTT Server at %s:%4d\n",server_ip, port );

                lastKA      = axtime_get_monotonic();
                result      = true;
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

    result = _connect(TO);

    RETURN(result);
}
BOOL Mqtt::_subscribe(UINT           TO)
{
    BOOL        result          = false;

    ENTER(true);

    if (sock && !topicLen)
    {
        U8 var_header[] = { MSB(MESSAGE_ID), LSB(MESSAGE_ID) };    // appended to end of PUBLISH message

        // utf topic
        U8 utf_topic[2 + strlen(subscription) + 1]; // 2 for message size + 1 for QoS

        // set up topic payload
        utf_topic[0] = 0;                       // MSB(strlen(topic));
        utf_topic[1] = LSB(strlen(subscription));
        memcpy((char *) &utf_topic[2], subscription, strlen(subscription));
        utf_topic[sizeof(utf_topic) - 1] = subscriptionQoS;

        U8 fixed_header[] = { SET_MESSAGE(SUBSCRIBE), sizeof(var_header) + sizeof(utf_topic) };
        //    fixed_header_t  fixed_header = { .QoS = 0, .connect_msg_t = SUBSCRIBE, .remaining_length = sizeof(var_header)+strlen(utf_topic) };

        U8 packet[sizeof(fixed_header) + sizeof(var_header) + sizeof(utf_topic)];

        memset(packet, 0, sizeof(packet));
        memcpy(packet, &fixed_header, sizeof(fixed_header));
        memcpy(packet + sizeof(fixed_header), var_header, sizeof(var_header));
        memcpy(packet + sizeof(fixed_header) + sizeof(var_header), utf_topic, sizeof(utf_topic));

        if (axdev_write(sock, packet, sizeof(packet), TO) == (INT)sizeof(packet))
        {
            U8   buffer[5];
            long sz = axdev_read(sock, buffer, sizeof(buffer), TO);  // wait for SUBACK

            //    printf("buffer size is %ld\n",sz);
            //    printf("%2x:%2x:%2x:%2x:%2x\n",(U8)buffer[0],(U8)buffer[1],(U8)buffer[2],(U8)buffer[3],(U8)buffer[4]);

            if ((GET_MESSAGE(buffer[0]) == SUBACK) && ((sz - 2) == buffer[1]) && (buffer[2] == MSB(MESSAGE_ID)) &&
                (buffer[3] == LSB(MESSAGE_ID)))
            {
                //printf("Subscribed to MQTT Service %s with QoS %d\n", subscription, buffer[4]);

                topicLen = strlen(subscription);

                result = true;
            }
        }
    }

    RETURN(result);
}
BOOL Mqtt::subscribe(PCSTR          topic,
                     QoS            qos,
                     UINT           TO)
{
    BOOL        result          = false;

    ENTER(true);

    strz_cpy(subscription, topic, sizeof(subscription));
    subscriptionQoS = qos;

    result = _subscribe(TO);

    RETURN(result);
}

INT Mqtt::receive(PMQTTMESSAGE   msg,
                  UINT           TO)
{
    INT         result          = -1;
    long    sz;
    U8 buffer[128];
    U32 topicSize;
    unsigned long i;
//    U8 puback[4];

    ENTER(true);

    if (sock && topicLen)
    {
        //printf("message size is %ld\n",sz);
        // if more than ack - i.e. data > 0
        if ((sz = axdev_read(sock, buffer, sizeof(buffer), 0)) > 0)
        {
            //printf("message size is %ld\n",sz);
            if (GET_MESSAGE(buffer[0]) == PUBLISH)
            {
                //printf("Got PUBLISH message with size %d\n", (U8)buffer[1]);
                topicSize = (buffer[2] << 8) + buffer[3];

                if (topicSize < sizeof(msg->topic))
                {
                    //printf("topic size is %d\n", topicSize);
                    memcpy(msg->topic, &buffer[4], topicSize);
                    *(msg->topic + topicSize) = 0;

                    i = 4 + topicSize;
                    if (((buffer[0] >> 1) & 0x03) > QoS0)
                    {
                        msg->messageId = (buffer[4 + topicSize] << 8) + buffer[4 + topicSize + 1];
                        //printf("Message ID is %d\n", messageId);
                        i += 2; // 2 extra for msgID
                        // if QoS1 the send PUBACK with message ID
                        U8 puback[4] = { SET_MESSAGE(PUBACK), 2, buffer[4 + topicSize], buffer[4 + topicSize + 1] };

                        if (axdev_write(sock, puback, sizeof(puback), 0) == (INT) sizeof(puback))
                        {
                        }
                    }
                    else
                        msg->messageId = 0;

                    result = (int) (sz - i);
                    memcpy(msg->message, &buffer[i], result);

                    *(msg->message + result) = 0;

                    //            for ( ; i < sz; ++i) {
                    //                print(buffer[i]);
                    //            }
                }
                else
                    result = 0;
            }
            else
                result = 0;
        }
        else
        {
            if (sz == -1)
            {
                perror("Mqtt::receive ");
            }

            result = (int) sz;
        }

    }

//    printf("mqtt_get_message result = %d\n", result);


    RETURN(result);
}

INT Mqtt::publish(PCSTR          topic,
                  PCSTR          msg,
                  QoS            qos,
                  UINT           TO)
{
    INT         result          = -1;

    ENTER(true);

    if (sock)
    {
        if (qos == QoS0)
        {
            // utf topic
            U8 utf_topic[2 + strlen(topic)]; // 2 for message size QoS-0 does not have msg ID

            // set up topic payload
            utf_topic[0] = 0;                       // MSB(strlen(topic));
            utf_topic[1] = LSB(strlen(topic));
            memcpy((char *) &utf_topic[2], topic, strlen(topic));

            U8 fixed_header[] = { SET_MESSAGE(PUBLISH) | (qos << 1), sizeof(utf_topic) + strlen(msg) };
            //    fixed_header_t  fixed_header = { .QoS = 0, .connect_msg_t = PUBLISH, .remaining_length = sizeof(utf_topic)+strlen(msg) };

            U8 packet[sizeof(fixed_header) + sizeof(utf_topic) + strlen(msg)];

            memset(packet, 0, sizeof(packet));
            memcpy(packet, &fixed_header, sizeof(fixed_header));
            memcpy(packet + sizeof(fixed_header), utf_topic, sizeof(utf_topic));
            memcpy(packet + sizeof(fixed_header) + sizeof(utf_topic), msg, strlen(msg));

            result = axdev_write(sock, packet, sizeof(packet), TO) == sizeof(packet);
        }
        else
        {
            pubMsgID++;
            // utf topic
            U8 utf_topic[2 + strlen(topic) + 2]; // 2 extra for message size > QoS0 for msg ID

            // set up topic payload
            utf_topic[0] = 0;                       // MSB(strlen(topic));
            utf_topic[1] = LSB(strlen(topic));
            memcpy((char *) &utf_topic[2], topic, strlen(topic));
            utf_topic[sizeof(utf_topic) - 2] = MSB(pubMsgID);
            utf_topic[sizeof(utf_topic) - 1] = LSB(pubMsgID);

            U8 fixed_header[] = { SET_MESSAGE(PUBLISH) | (qos << 1), sizeof(utf_topic) + strlen(msg) };

            U8 packet[sizeof(fixed_header) + sizeof(utf_topic) + strlen(msg)];

            memset(packet, 0, sizeof(packet));
            memcpy(packet, &fixed_header, sizeof(fixed_header));
            memcpy(packet + sizeof(fixed_header), utf_topic, sizeof(utf_topic));
            memcpy(packet + sizeof(fixed_header) + sizeof(utf_topic), msg, strlen(msg));

            if (axdev_write(sock, packet, sizeof(packet), TO) == (INT) sizeof(packet))
            {
                if (qos == QoS1)
                {
                    // expect PUBACK with MessageID
                    U8   buffer[4];
                    long sz = axdev_read(sock, buffer, sizeof(buffer), TO);  // wait for SUBACK

                    //    printf("buffer size is %ld\n",sz);
                    //    printf("%2x:%2x:%2x:%2x:%2x\n",(U8)buffer[0],(U8)buffer[1],(U8)buffer[2],(U8)buffer[3],(U8)buffer[4]);

                    if ((GET_MESSAGE(buffer[0]) == PUBACK) && ((sz - 2) == buffer[1]) && (buffer[2] == MSB(pubMsgID)) &&
                        (buffer[3] == LSB(pubMsgID)))
                    {
                        printf("Published to MQTT Service %s with QoS1\n", topic);

                        result = 1;
                    }
                    else
                        result = 0;
                }
                else
                    result = 1;

            }
        }
    }

    RETURN(result);
}

void Mqtt::disconnect()
{
    U8 fixed_header[] = { SET_MESSAGE(DISCONNECT), 0};

    if (sock)
    {
        if (topicLen)
        {
            axdev_write(sock, fixed_header, sizeof(fixed_header), 1000);
            topicLen = 0;
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
        if (*subscription)
        {
            result = _subscribe(TO);
        }
        else
            result = true;
    }

    RETURN(result);
}
BOOL Mqtt::keepAlive(UINT           TO)
{
    BOOL        result          = false;
    AXTIME      now;

    ENTER(true);

    if (sock)
    {
        now = axtime_get_monotonic();

        if ((now - lastKA) >= (kaTimeout * 1000))
        {
            U8 fixed_header[] = { SET_MESSAGE(PINGREQ), 0 };

            if (axdev_write(sock, fixed_header, sizeof(fixed_header), TO) == (INT)sizeof(fixed_header))
            {
                //printf("Mqtt.cpp(451): stage sent PINGREQ\n");
                U8   buffer[5];
                long sz = axdev_read(sock, buffer, sizeof(buffer), TO);  // wait for SUBACK

                //    printf("buffer size is %ld\n",sz);
                //    printf("%2x:%2x:%2x:%2x:%2x\n",(U8)buffer[0],(U8)buffer[1],(U8)buffer[2],(U8)buffer[3],(U8)buffer[4]);

                if (sz > 0)
                {
                    if ((GET_MESSAGE(buffer[0]) == PINGRESP))
                    {
                        //printf("Mqtt.cpp(451): stage GOT PINGRESP\n");

                        lastKA = now;
                        result = true;
                    }
                }
            }
        }

        result = true;
    }

    RETURN(result);
}
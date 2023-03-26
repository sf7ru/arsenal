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

#ifndef __Mqtt_H__
#define __Mqtt_H__

#include <axnet.h>
#include <customboard.h>
#include <AXBuffer.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#ifndef CBRD_MQTT_MESSAGE_MAX
#define CBRD_MQTT_MESSAGE_MAX 1024
#endif

#define MQTT_BUFF_SIZE      (CBRD_MQTT_MESSAGE_MAX + 64 + 129)

#define MQTT_FLAG_RETAIN    1

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef enum
{
    QoS0,
    QoS1,
    QoS2
} QoS;

typedef struct __tag_MQTTMESSAGE
{
    U32     messageId;
    CHAR    topic       [ 129 ];
    CHAR    message     [ 129 ];
} MQTTMESSAGE, * PMQTTMESSAGE;

typedef struct __tag_MQTTSUBSRIBTION
{
    QoS     qos;
    CHAR    topic       [ 129 ];
} MQTTSUBSRIBTION, * PMQTTSUBSRIBTION;


// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class Mqtt
{
protected:
        int             socket;
        U16             port;
        char            iface                   [16];  // based on xxx.xxx.xxx.xxx format
        char            server_ip               [16];  // based on xxx.xxx.xxx.xxx format
        char            clientid                [24];  // max 23 charaters long
        U16             pubMsgID;
        U16             subMsgID;

        U8              inBuffData              [ MQTT_BUFF_SIZE ];
        U8              outBuff                 [ MQTT_BUFF_SIZE ];

        AXBuffer        inBuff;

        AXTIME          lastKA;

        PAXDEVWrapper * wrapper;
        PAXDEV          sock;

        INT             subscribeIndex;
        BOOL            subscribeSent;
        UINT            subscriptionsSize;
        PMQTTSUBSRIBTION subscriptions;

        BOOL            _connect                (UINT           TO);

        BOOL            _subscribe              (PMQTTSUBSRIBTION sub,
                                                 UINT             TO);

        INT             _parse_PUBLISH          (PMQTTMESSAGE   msg,
                                                 long &         sz);

        INT             _parse_PINGRESP         (PMQTTMESSAGE   msg,
                                                 long &         sz);

        INT             _parse_PINGREQ          (PMQTTMESSAGE   msg,
                                                 long &         sz);

        INT             _parse_SUBACK           (PMQTTMESSAGE   msg,
                                                 long &         sz);

        INT             _parse_PUBACK           (PMQTTMESSAGE   msg,
                                                 long &         sz);


public:
        UINT            kaTimeout;

                        Mqtt                    ()
                        {
                            sock                = nil;
                            *iface              = 0;
                            subscribeIndex      = -1;
                            subscriptionsSize   = 0;
                            pubMsgID            = 0;
                            subMsgID            = 0;
                            subscriptions       = 0;

                            lastKA              = 0;
                            kaTimeout           = 10;
                        }

        BOOL            connect                 (PCSTR          client,
                                                 PCSTR          iface,
                                                 PCSTR          server_ip,
                                                 UINT           port,
                                                 UINT           TO);

        void            disconnect              ();

        BOOL            reconnect               (UINT           TO);

        INT             publish                 (PCSTR          topic,
                                                 PCSTR          msg,
                                                 QoS            qos,
                                                 UINT           flags,
                                                 UINT           TO);

        BOOL            subscribe               (PMQTTSUBSRIBTION subssArray,
                                                 UINT           arraySize);

        INT             turn                    (PMQTTMESSAGE   msg,
                                                 UINT           TO);

        BOOL            keepAlive               (UINT           TO);
};

#endif // #define __Mqtt_H__

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

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct __tag_MQTTMESSAGE
{
    U32     messageId;
    CHAR    topic       [ 129 ];
    CHAR    message     [ 129 ];
} MQTTMESSAGE, * PMQTTMESSAGE;

typedef enum
{
    QoS0,
    QoS1,
    QoS2
} QoS;

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
        int             topicLen;
        U16             pubMsgID;
        U16             subMsgID;

        AXTIME          lastKA;

        PAXDEV          sock;

        UINT            subscriptionQoS;
        CHAR            subscription            [ 129 ];

        BOOL            _connect                (UINT           TO);

        BOOL            _subscribe              (UINT           TO);

public:
        UINT            kaTimeout;

                        Mqtt                    ()
                        {
                            sock                = nil;
                            *iface              = 0;
                            topicLen            = 0;
                            pubMsgID            = 0;
                            subMsgID            = 0;
                            *subscription       = 0;

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
                                                 UINT           TO);

        BOOL            subscribe               (PCSTR          topic,
                                                 QoS            qos,
                                                 UINT           TO);

        INT             receive                 (PMQTTMESSAGE   msg,
                                                 UINT           TO);

        BOOL            keepAlive               (UINT           TO);
};

#endif // #define __Mqtt_H__

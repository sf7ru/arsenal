// ***************************************************************************
// TITLE
//
// PROJECT
//
// ***************************************************************************
//
// FILE
//
// HISTORY
//
// ***************************************************************************

#ifndef __EGOISPPP_H__
#define __EGOISPPP_H__

#include <arsenal.h>

#include <AXDevice.h>
#include <AXBuffer.h>
//#include <B3Modem.h>

typedef struct __tag_PPPCONFIG
{
    UINT        mru;
    UINT        auth;
    UINT        magic;
    BOOL        sendCtrl;
    BOOL        sendAddr;
    U32      accm;
} PPPCONFIG;


// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class SnPpp
{
        AXDevice *      dev;
        PCSTR           user;
        PCSTR           passwd;

        PPPCONFIG       cliCfg;
        PPPCONFIG       svrCfg;

        PU8             frame;

        BOOL            gotEscape;

        UINT            stage;

        UINT            TO;

        UINT            nextId;

        UINT            frameProto;
        UINT            frameCode;
        UINT            frameId;
        UINT            frameSize;

        PU8             onFrame;
        UINT            onLeft;
        U16             onChecksum;

        BOOL            getFrame                ();

        BOOL            readFrame               ();

        BOOL            sendEscaped             (PVOID          data,
                                                 UINT           size);

        BOOL            readEscaped             ();

        BOOL            sendFrame               (UINT           proto,
                                                 UINT           code,
                                                 UINT           id,
                                                 PVOID          data,
                                                 UINT           len);

        void            handleLcpConfigReq      ();

public:

                        SnPpp                   ();
                        ~SnPpp                  ();

        void            close                   ();

        BOOL            open                    (AXDevice *     dev,
                                                 PCSTR          user,
                                                 PCSTR          passwd,
                                                 UINT           mru);

        INT             read                    ();
};

#endif // __EGOISPPP_H__

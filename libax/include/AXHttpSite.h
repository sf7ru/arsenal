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

#ifndef __AXHTTPSITE_H__
#define __AXHTTPSITE_H__

#include <arsenal.h>
#include <AXHttp.h>
#include <axthreads.h>
#include <axnet.h>

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct _tag_SESSIONPARAMS
{
    AXHttpServer *  server;
    AXHttpReq *     baseReq;
} SESSIONPARAMS, * PSESSIONPARAMS;


// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class AXHttpSite
{
        PAXDEV          listen;
        HAXMUTEX        h_LCK;
        HAXTHREADX      thread;
        HAXTHREADGROUP  sessions;

        AXHttpReq *     baseReq;

        PVOID           mCertificate;
        PVOID           mPrivateKey;

                        AXHttpSite              ();

static  int             siteThread              (HAXTHREADX     h_ctrl,
                                                 AXHttpSite *   pcl);

static  int             sessionThread           (HAXTHREADX     h_ctrl,
                                                 PSESSIONPARAMS params);

        AXHttpServer *  newSession              (PAXDEV         dev);


public:

static  AXHttpSite *    create                  (PCSTR          iface,
                                                 UINT           port,
                                                 AXHttpReq *    req,
                                                 UINT           maxThreads = 0);

static  AXHttpSite *    create                  (PAXDEV         dev,
                                                 AXHttpReq *    req,
                                                 UINT           maxThreads = 0);


        BOOL            setSsl                  (PVOID          certificate,
                                                 PVOID          privateKey);

                        ~AXHttpSite             ();

};


#endif // __R3HTTPSITE_H__

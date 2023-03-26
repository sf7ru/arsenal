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
#include <stdio.h>
#include <AXHttpSite.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define TO_ACCEPT           3000
#define TO_READ             500
#define TO_OC               (TO_ACCEPT * 3)


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      Destructor
// FUNCTION
//      AXHttpSite::~AXHttpSite
// PURPOSE
//      Class destruction
// PARAMETERS
//          --
// ***************************************************************************
AXHttpSite::~AXHttpSite()
{
    ENTER(true);

     if (thread)
         axthreadX_destroy(thread, TO_OC);

     if (sessions)
     {
         axthreadgroup_stop(sessions, TO_OC);
         axthreadgroup_destroy(sessions);
     }

     if (h_LCK)
         axmutex_destroy(h_LCK);

     if (listen)
         axdev_close(listen, TO_OC);

     if (baseReq)
         delete baseReq;

#ifdef AXHTTP_SSL
     AXHttpSslWrapper::globalDeinit();
#endif

     QUIT;
}
// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      AXHttpSite::AXHttpSite
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
AXHttpSite::AXHttpSite()
{
    h_LCK           = NULL;
    thread          = NULL;
    listen          = NULL;
    sessions        = NULL;
    baseReq         = NULL;

    mCertificate    = nil;
    mPrivateKey     = nil;

}
// ***************************************************************************
// FUNCTION
//      AXHttpSite::create
// PURPOSE
//
// PARAMETERS
//      PCSTR       iface --
//      UINT        port  --
//      AXHttpReq * req   --
// RESULT
//      AXHttpSite * --
// ***************************************************************************
AXHttpSite * AXHttpSite::create(PCSTR          iface,
                                UINT           port,
                                AXHttpReq *    req,
                                UINT           maxThreads)
{
    AXHttpSite * result          = NULL;
    PAXDEV       dev;

    ENTER(true);

    if ((dev = axssocket_listen((PSTR)(iface ? iface : NULL), port, 1)) != NULL)
    {
        if ((result = create(dev, req, maxThreads)) == nil)
        {
            axssocket_close(dev);
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXHttpSite::create
// PURPOSE
//
// PARAMETERS
//      PAXDEV      dev --
//      AXHttpReq * req --
// RESULT
//      AXHttpSite * --
// ***************************************************************************
AXHttpSite * AXHttpSite::create(PAXDEV         dev,
                                AXHttpReq *    req,
                                UINT           maxThreads)
{
    AXHttpSite * result          = NULL;

    ENTER(true);

#ifdef AXHTTP_SSL
    AXHttpSslWrapper::globalInit();
#endif


    if ((result = new AXHttpSite) != NULL)
    {
        result->baseReq = req;

        if ( !( ((result->h_LCK    = axmutex_create())  != NULL) &&
                ((result->listen   = dev)               != NULL) &&
                ((result->sessions = axthreadgroup_create(maxThreads, NULL, true)) != NULL) &&
                ((result->thread   = axthreadX_create(0, (PFNAXTHREADX)
                                  AXHttpSite::siteThread, result,
                                  TO_OC))            != NULL) ))
        {
            delete result;
            result = NULL;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXHttpSite::siteThread
// PURPOSE
//
// PARAMETERS
//      HAXTHREADX   h_ctrl --
//      AXHttpSite * pcl    --
// RESULT
//      int --
// ***************************************************************************
int AXHttpSite::siteThread(HAXTHREADX     h_ctrl,
                           AXHttpSite *   pcl)
{
    AXSIG           v_sig;
    PAXDEV          client;
    PSESSIONPARAMS  params;

    TX_ENTER(h_ctrl, pcl);

    while (!MAC_THREADX_WAIT_QUIT(v_sig, nil, 0))
    {
        if ((client = axdev_accept(pcl->listen, TO_ACCEPT)) != NULL)
        {
            //printf("SITE: incoming connection \n");

            if ((params = new SESSIONPARAMS) != nil)
            {
                params->baseReq  = pcl->baseReq;

                if ((params->server = pcl->newSession(client)) != NULL)
                {
                    if ( !( axthreadgroup_reuse(pcl->sessions, params)  ||
                            (axthreadgroup_add(pcl->sessions,
                                 (PFNAXTHREADX)AXHttpSite::sessionThread,
                                 params, 0,
                                 false, TO_ACCEPT) != AXII)             ))
                    {
                        //printf("SITE ERR: cannot start thread\n");

                        delete params->server;
                        delete params;
                    }
                }
                else
                {
                    //printf("SITE ERR: cannot create session\n");

                    delete params;
                }
            }
            else
            {
                //printf("SITE ERR: cannot create params\n");
            }
        }
    }

    TX_EXIT(h_ctrl, 0);
}
// ***************************************************************************
// FUNCTION
//      AXHttpSite::newSession
// PURPOSE
//
// PARAMETERS
//      PAXDEV dev --
// RESULT
//      AXHttpServer * --
// ***************************************************************************
AXHttpServer * AXHttpSite::newSession(PAXDEV      dev)
{
    AXHttpServer *      server  = NULL;
    AXDevice *          wrapper;

    ENTER(true);


#ifdef AXHTTP_SSL
    if (mCertificate && mPrivateKey)
        wrapper = AXHttpSslWrapper::accept(dev, mCertificate, mPrivateKey);
    else
#endif
        wrapper = new PAXDEVWrapper(dev);

    if (wrapper != nil)
    {
        if ((server = new AXHttpServer) != NULL)
        {
            if (!server->initHttp(wrapper, 16 KB))
            {
                delete wrapper;
                delete server;
            }
        }
    }
    else
        axdev_close(dev, 0);

    RETURN(server);
}
// ***************************************************************************
// FUNCTION
//      AXHttpSite::sessionThread
// PURPOSE
//
// PARAMETERS
//      HAXTHREADX     h_ctrl --
//      PSESSIONPARAMS params --
// RESULT
//      int --
// ***************************************************************************
int AXHttpSite::sessionThread(HAXTHREADX        h_ctrl,
                              PSESSIONPARAMS    params)
{
    AXSIG           v_sig;
    AXHttpServer *  server;
    AXHttpReq *     req;
    AXHttpReq *     baseReq;
    UINT            index           = 0;

    TX_ENTER(h_ctrl, true);

    axthreadgroup_get_parent(h_ctrl, &index, 1000);

    do
    {
        server  = params->server;
        baseReq = params->baseReq;

         //printf(">>  client session TH#%d\n", index);

        //while (!MAC_THREADX_WAIT_QUIT(v_sig, nil, 0))
        if (!MAC_THREADX_WAIT_QUIT(v_sig, nil, 0))
        {
            if ((req = baseReq->clone()) != NULL)
            {
                if (server->give(req, TO_READ) != HTTPRC_OK)
                {
                    axsleep(200);
                    break;
                }

                delete req;
            }
        }

        server->closeDevice();
        delete server;
        delete params;

         //printf("<<  client session TH#%d\n", index);

    } while ((params = (PSESSIONPARAMS)axthreadgroup_wait_reuse(h_ctrl)) != nil);

    TX_EXIT(h_ctrl, 0);
}
// ***************************************************************************
// FUNCTION
//      AXHttpSite::setSsl
// PURPOSE
//
// PARAMETERS
//      PVOID certificate --
//      PVOID privateKey  --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXHttpSite::setSsl(PVOID          certificate,
                        PVOID          privateKey)
{
    BOOL            result          = false;

    ENTER(true);

    mCertificate    = certificate;
    mPrivateKey     = privateKey;
    result          = true;

    RETURN(result);
}

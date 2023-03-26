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
#include <stdio.h>

#include <AXService.h>
#include <axstring.h>
#include <axtime.h>
#include <axsystem.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// ---------------------------------------------------------------------------

#define ACTION_run          0
#define ACTION_reg          1
#define ACTION_unreg        2
#define ACTION_help         3

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      _sig_handler
// PURPOSE
//
// PARAMETERS
//      PAXSERVICECTL   serviceCtl --
//      AXSIG            d_sig    --
// RESULT
//      BOOL  --
// ***************************************************************************
static BOOL _sig_handler(PAXSERVICECTL        serviceCtl,
                         AXSIG                 sig)
{
    BOOL                b_result        = false;
    AXService *          pcl;

    ENTER(serviceCtl && serviceCtl->p_ptr);

    pcl         = (AXService *)serviceCtl->p_ptr;
    b_result    = pcl->signalHandler(sig);

    RETURN(b_result);
}
AXService::~AXService(void)
{
// printf("AXService::~AXService(void) !!!!!!!!!!!!!\n");
}
// ***************************************************************************
// FUNCTION
//      AXService::AXService
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//        --
// ***************************************************************************
AXService::AXService(void)
{
    ENTER(true);

    action              = ACTION_run;
    stopRequest         = false;
    stopTimed           = 0;

    *name               = 0;
    *desc               = 0;

    dbgShowCrashInfo    = false;
    *resourcesFilename  = 0;

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      AXService::endashedParameter
// PURPOSE
//
// PARAMETERS
//      INT   param --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL AXService::endashedParameter(INT param)
{
    BOOL            b_result        = false;
    PCSTR           prm             = NULL;

    ENTER(true);

    if ((b_result = AXApp::endashedParameter(param)) == false)
    {
        switch (param)
        {
            case 'r':
                action   = ACTION_reg;
                b_result = true;

                if ((prm = nextParameter()) != NULL)
                {
                    strz_cpy(name, prm, AXLSHORT);

                    if ((prm = nextParameter()) != NULL)
                    {
                        strz_cpy(desc, prm, AXLLONG);
                    }
                }
                break;

            case 'u':
                action   = ACTION_unreg;
                b_result = true;

                if ((prm = nextParameter()) != NULL)
                {
                    strz_cpy(name, prm, AXLSHORT);

                    if ((prm = nextParameter()) != NULL)
                    {
                        strz_cpy(desc, prm, AXLLONG);
                    }
                }
                break;

            case 'v':
            case 'h':
                action   = ACTION_help;
                b_result = true;
                break;
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXService::main
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//      int  --
// ***************************************************************************
int AXService::appMain(void)
{
    int             result      = 0;
    BOOL            b_result;
    AXSERVICECTL   serviceCtl;

    ENTER(true);

    if (initResources(resourcesFilename))
    {
        if (!*name)
        {
            strz_cpy(name, resPSTR(1), AXLSHORT);
        }

        if (!*desc)
        {
            strz_cpy(desc, resPSTR(2), AXLLONG);
        }

        initSysLog(name);

        switch (action)
        {
            case ACTION_run:
                if (setDefaultConfig() && init())
                {
                    memset(&serviceCtl, 0, sizeof(AXSERVICECTL));

                    serviceCtl.p_ptr            = this;
                    serviceCtl.psz_name         = name;
                    serviceCtl.psz_desc         = desc;
                    serviceCtl.d_features       = AXDMN_CAN_STOP;
                    serviceCtl.pfn_entry_point  = (AXDMN_ENTRY_POINT)&AXService::service;
                    serviceCtl.pfn_sig_handler  = (AXDMN_SIG_HANDLER)_sig_handler;

                    if (debug)
                    {
                        // FIXME
                        //axservice_sighandler(&serviceCtl);

                        service(&serviceCtl);
                    }
                    else
                    {
                        if (!axservice_reborn(&serviceCtl))
                        {
                            printf("Cannot spawn to system service\n");
                            result      = -1;
                        }
                    }
                }
                break;

            case ACTION_reg:
                b_result = axservice_register(
                                name, desc, NULL, NULL);

                printf("Registering as system service \"%s\" (%s) -  %s\n",
                        name, desc,
                        (b_result ? "Ok" : "FAULT"));
                break;

            case ACTION_unreg:
                b_result = axservice_unregister(name);

                printf("Unregistering system service \"%s\" (%s) -  %s\n",
                        name, desc,
                        (b_result ? "Ok" : "FAULT"));
                break;

            default:
                showHelp();
                break;
        }
    }
    else
        printf("Cannot load resources\n");

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXService::service
// PURPOSE
//
// PARAMETERS
//      PAXSERVICECTL serviceCtl --
// RESULT
//      U32 --
// ***************************************************************************
U32 AXService::service(PAXSERVICECTL    serviceCtl)
{
    U32          result = 0;
    AXService *     pcl;

    ENTER(true);

    pcl = (AXService *)serviceCtl->p_ptr;

    if (serviceCtl->psz_name)
    {
        // Not a debug mode
        service_console2devnull();
    }

    pcl->serviceMain();

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL AXService::isStopRequested         --
// ***************************************************************************
BOOL AXService::isStopRequested         ()
{
    BOOL            result          = false;

    ENTER(true);

    if (  stopRequest                                           ||
          (stopTimed && (axutime_get_monotonic() >= stopTimed))  )
    {
        result = true;
    }


    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXService::setTimedStop
// PURPOSE
//
// PARAMETERS
//      UINT secs --
// RESULT
//      void --
// ***************************************************************************
void AXService::setTimedStop(UINT           secs)
{
    ENTER(true);

    stopTimed = axutime_get_monotonic() + secs;

    QUIT;
}
BOOL AXService::lock(PCSTR          name)
{
    BOOL            result = true;
    UINT            mypid;
    UINT            forbearpid;
    FILE *          flck;
    CHAR            buff            [ 80 ];
    CHAR            path            [ AXLPATH ];

    ENTER(true);

    mypid = axprocess_get_id(nil);
    
    sprintf(path, "/var/run/%s.pid", name);

    if ((flck = fopen(path, "rt")) != NULL)
    {
        printf("- lock exist\n");
        if (fgets(buff, sizeof(buff) - 1, flck) > 0)
        {
            if ((forbearpid = atol(strz_clean(buff))) > 0)
            {
                if (axprocess_is_alive2(forbearpid))
                {
                    printf("- pid alive\n");
                    result = false;
                }
                else
                    printf("- pid not alive\n");
            }
            else
                printf("- pid is zero\n");
        }

        fclose(flck);
    }
    else
        printf("- lock not exist\n");

    if (result)
    {
        result = false;

        if ((flck = fopen(path, "w+t")) != NULL)
        {
            printf("- lock created\n");

            if (fprintf(flck, "%d", mypid) > 0)
            {
                printf("- lock file written %d\n", mypid);

                result = true;
            }

            fclose(flck);
        }
        else
            printf("- lock can't be created exist\n");
    }

    RETURN(result);
}
void AXService::unlock(PCSTR          name)
{
    CHAR            path            [ AXLPATH ];

    ENTER(true);

    sprintf(path, "/var/run/%s.pid", name);

    remove(path);

    QUIT;
}

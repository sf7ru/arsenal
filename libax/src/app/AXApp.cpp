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

#include <AXApp.h>

#include <stdio.h>
#include <axtime.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      AXApp::AXApp
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//        --
// ***************************************************************************
AXApp::AXApp(void)
{
    ENTER(true);

    debug               = false;
    res                 = NULL;
    logSession          = NULL;
    logLevel            = AXLOGDL_debug;
    sysLogLevel         = AXLOGDL_error;
    dbgShowCrashInfo    = true;
    markPeriod          = 0;
    lastLog             = 0;
    *configFilename     = 0;

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      AXApp::~AXApp
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//        --
// ***************************************************************************
AXApp::~AXApp(void)
{
    ENTER(true);

    //printf("AXApp::~AXApp(void) !!!!!!!!!!!!!\n");

    if (res)
    {
        croesus_close(res);
    }

    if (logSession)
        axlog_close(logSession);

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      AXApp::initResources
// PURPOSE
//
// PARAMETERS
//      PSTR   resName --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL AXApp::initResources(PCSTR resName)
{
    BOOL            b_result        = false;

    ENTER(true);

    if ((res = croesus_open(resName && *resName ? resName : NULL)) != NULL)
    {
        b_result = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXApp::initResources
// PURPOSE
//
// PARAMETERS
//      PSTR   resName --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL AXApp::initLog(PCSTR       logPath,
                    PCSTR       logName,
                    UINT        logFlags,
                    AXLOGDL     logLevel,
                    UINT        logLifetime,
                    UINT        markPeriod)
{
    BOOL            b_result        = false;

    ENTER(true);

    if ((logSession = axlog_open(axlog_file_way(logPath, logName, logLifetime, logFlags), 1024)) != NULL)
    {
        this->logLevel      = logLevel;
        this->markPeriod    = markPeriod;
        b_result        = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXApp::initResources
// PURPOSE
//
// PARAMETERS
//      PSTR   resName --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL AXApp::initLogMp(UINT        filesNum,
                      AXMPLOG     files[],
                      UINT        msgSize,
                      UINT        logFlags,
                      AXLOGDL     logLevel,
                      UINT        markPeriod)
{
    BOOL            b_result        = false;
    BOOL            wayReady;
    UINT            idx;
    HAXLOGWAY       logWay;

    ENTER(true);

    if ((logWay = axlog_mobilepark_way(logFlags)) != NULL)
    {
        for (  wayReady = true, idx = 0;
                wayReady && (idx < filesNum);
               idx++)
        {
            wayReady = axlog_mobilepark_add_file(logWay,
                           files[idx].fname, files[idx].filter);
        }

        if (wayReady)
        {
            if ((logSession = axlog_open(logWay, msgSize)) != NULL)
            {
                this->logLevel      = logLevel;
                this->markPeriod    = markPeriod;
                b_result            = true;
            }
        }
        else
            axlog_way_close(logWay);
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXApp::initLogTagged
// PURPOSE
//
// PARAMETERS
//      PCSTR   logPath     --
//      UINT    logFlags    --
//      AXLOGDL logLevel    --
//      UINT    logLifetime --
//      UINT    markPeriod  --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL AXApp::initLogTagged(PCSTR       logPath,
                          PCSTR       logNameDefault,
                          UINT        logFlags,
                          AXLOGDL     logLevel,
                          UINT        logLifetime,
                          UINT        markPeriod)
{
    BOOL            b_result        = false;
    HAXLOGWAY       logWay;

    ENTER(true);

    if ((logWay = axlog_tagway(logPath,
                               logNameDefault,
                               logLifetime,
                               logFlags)) != NULL)
    {
        if ((logSession = axlog_open(logWay, AXLHUGE)) != NULL)
        {
            this->logLevel      = logLevel;
            this->markPeriod    = markPeriod;
            b_result            = true;
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXApp::initDebug
// PURPOSE
//
// PARAMETERS
//      PSTR   params --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL AXApp::initDebug(PCSTR      params)
{
    BOOL            b_result        = false;

    ENTER(true);

    b_result = setCrashHook(this);

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXApp::resFree
// PURPOSE
//
// PARAMETERS
//      UINT id --
// RESULT
//      void --
// ***************************************************************************
void AXApp::resFree(UINT id)
{
    croesus_free(res, id);
}
// ***************************************************************************
// FUNCTION
//      AXApp::resPSTR
// PURPOSE
//
// PARAMETERS
//      UINT  id   --
//      PUINT size --
// RESULT
//      PCSTR --
// ***************************************************************************
PCSTR AXApp::resPSTR(UINT   id,
                     PUINT  size)
{
    PSTR        pst_result  = NULL;

    ENTER(res);

    pst_result = (PSTR)croesus_get(res, id, NULL, size);

    RETURN(pst_result);
}
// ***************************************************************************
// FUNCTION
//      AXApp::resBIN
// PURPOSE
//
// PARAMETERS
//      UINT  id   --
//      PUINT size --
// RESULT
//      PCVOID --
// ***************************************************************************
PCVOID AXApp::resBIN(UINT   id,
                     PUINT  size)
{
    PCVOID      pst_result  = nil;

    ENTER(res);

    pst_result = (PCVOID)croesus_get(res, id, NULL, size);

    RETURN(pst_result);
}
// ***************************************************************************
// FUNCTION
//      AXApp::nextParameter
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//      PSTR  --
// ***************************************************************************
PCSTR AXApp::nextParameter(void)
{
    PSTR            param       = NULL;

    ENTER(true);

    if (parseArgIdx < parseArgTotal)
    {
        param = parseArgVector[++parseArgIdx];
    }

    RETURN(param);
}
// ***************************************************************************
// FUNCTION
//      AXApp::main
// PURPOSE
//
// PARAMETERS
//      int      argc   --
//      char *   argv[] --
// RESULT
//      int  --
// ***************************************************************************
int AXApp::main(int            argc,
                char *         argv[])
{
    int     result              = 0;
    BOOL    ok                  = true;
    PSTR    onArg;

    ENTER(true);

    initDebug(NULL);

    if (appInit(argc, argv))
    {
        for (  parseArgIdx = 1, parseArgTotal = argc, parseArgVector = argv;
               ok                           &&
               (parseArgIdx < parseArgTotal);
               parseArgIdx++)
        {
            onArg = argv[parseArgIdx];

            if (*onArg == '-')
            {
                if (*(++onArg) == '-')
                {
                    // double endashed - not supported by now
                }
                else
                {
                    for (; ok && *onArg; onArg++)
                    {
                        ok = endashedParameter(*onArg);
                    }
                }
            }
            else
                parameter(onArg);
        }

        if (ok)
        {
            if (guiInit())
            {
                if (appInit())
                {
                    result = appMain();
                }
            }
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXApp::turn
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//      void  --
// ***************************************************************************
void AXApp::periodicTurn(void)
{
    AXTIME          now;

    ENTER(true);

    if (markPeriod && lastLog)
    {
        now = axutime_get();

        if (  ((lastLog + markPeriod) <= now)   &&
              (now % markPeriod)                )
        {
            log(I, "-- MARK --");
        }
    }

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      AXApp::log_l
// PURPOSE
//
// PARAMETERS
//      AXLOGDL        v_level    --
//      const char *   msg        --
//      va_list        st_va_list --
// RESULT
//      void  --
// ***************************************************************************
void AXApp::log_l(AXLOGDL        v_level,
                  const char *   msg,
                  va_list        st_va_list)

{
    ENTER(true);

    if ((v_level & 0xFF) <= logLevel)
    {
        if (logSession)
        {
            lastLog = axutime_get();

            axlog_msg_vl(logSession, v_level, (PSTR)msg, st_va_list);
        }
        else
        {
            vprintf(msg, st_va_list);
            printf("\n");
        }
    }

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      AXApp::log
// PURPOSE
//
// PARAMETERS
//      AXLOGDL        v_level --
//      const char *   msg     --
// RESULT
//      void  --
// ***************************************************************************
void AXApp::log(AXLOGDL      v_level,
                const char * msg,
                ...)
{
    va_list         st_va_list;

    ENTER(true);

    va_start(st_va_list, msg);

    if (v_level <= logLevel)
    {
        if (logSession)
        {
            lastLog = axutime_get();

            axlog_msg_vl(logSession, v_level, (PSTR)msg, st_va_list);
        }
        else
        {
            vprintf(msg, st_va_list);
            printf("\n");
        }
    }

    if (v_level <= sysLogLevel)
    {
        syslog.log_l(v_level, msg, st_va_list);
    }

    va_end(st_va_list);

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      AXApp::setDefaultConfig
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL AXApp::setDefaultConfig(void)
{
    BOOL            b_result        = true;

    ENTER(true);

//    printf("AXApp::setDefaultConfig stage: '%s' \n", configFilename);

    if (!*configFilename)
    {
#if (TARGET_FAMILY == __AX_unix__)

        strz_sformat(configFilename, AXLPATH,
                 "/etc/%s/%s", resPSTR(4) ? resPSTR(4) : resPSTR(1), resPSTR(3));

#else

        if (la6_module_get_path(configFilename, AXLPATH))
        {
            strz_sformat(configFilename, AXLPATH,
                 "%s/%s/%s", configFilename, resPSTR(4) ? resPSTR(4) : "." , resPSTR(3));
        }
        else
            b_result = false;

#endif
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXApp::endashedParameter
// PURPOSE
//
// PARAMETERS
//      INT param --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXApp::endashedParameter(INT param)
{
    BOOL            b_result        = false;
    PCSTR           prm             = nil;

    ENTER(true);

    switch (param)
    {
        case 'c':
            if ((prm = nextParameter()) != NULL)
            {
                strz_cpy(configFilename, prm, AXLPATH);

//                printf("AXApp::endashedParameter stage = %s\n", configFilename);
                b_result = true;
            }
            break;

        case 'd':
            debug    = true;
            b_result = true;
            break;
    }

    RETURN(b_result);
}

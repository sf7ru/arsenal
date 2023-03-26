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

#ifndef __AXAPPH__
#define __AXAPPH__

#include <arsenal.h>
#include <axlog.h>
#include <axdata.h>
#include <AXSysLog.h>
#include <axstring.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// ---------------------------------------------------------------------------

#define N               AXLOGDL_notice
#define I               AXLOGDL_info
#define D               AXLOGDL_debug
#define E               AXLOGDL_error
#define W               AXLOGDL_warning


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct __tag_AXMPLOG
{
    U32         filter;
    CHAR        fname       [ AXLPATH ];
} AXMPLOG, * PAXMPLOG;

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

#ifdef __cplusplus

class AXApp
{
        HCROESUS        res;
        AXLOGDL         logLevel;

        AXSysLog        syslog;
        AXLOGDL         sysLogLevel;

        UINT            markPeriod;
        UINT            lastLog;

        int             parseArgIdx;
        int             parseArgTotal;
        char **         parseArgVector;

static  BOOL            setCrashHook            (AXApp *       that);

protected:
        BOOL            debug;

        HAXLOG          logSession;
        CHAR            configFilename          [ AXLPATH  ];

virtual BOOL            guiInit                 ()
        { return true; }

virtual BOOL            appInit                 (int            argc,
                                                 char *         argv[])
        { return true; }


        BOOL            setDefaultConfig        (void);

public:
        BOOL            dbgShowCrashInfo;

                        AXApp                   (void);

virtual                 ~AXApp                  (void);

        BOOL            initResources           (PCSTR          resName);

        void            resFree                 (UINT           id);

        PCSTR           resPSTR                 (UINT           id,
                                                 PUINT          size = nil);

        PCVOID          resBIN                  (UINT           id,
                                                 PUINT          size = nil);

        int             main                    (int            argc,
                                                 char *         argv[]);

        BOOL            initSysLog              (PCSTR          identifier)
                        {
                            return syslog.open(identifier);
                        }

        BOOL            initLog                 (PCSTR          logPath,
                                                 PCSTR          logName,
                                                 UINT           logFlags,
                                                 AXLOGDL        logLevel,
                                                 UINT           logLifetime,
                                                 UINT           markPeriod);

        BOOL            initLogMp               (UINT           filesNum,
                                                 AXMPLOG        files[],
                                                 UINT           msgSize,
                                                 UINT           logFlags,
                                                 AXLOGDL        logLevel,
                                                 UINT           markPeriod);

        BOOL            initLogTagged           (PCSTR          logPath,
                                                 PCSTR          logNameDefault,
                                                 UINT           logFlags,
                                                 AXLOGDL        logLevel,
                                                 UINT           logLifetime,
                                                 UINT           markPeriod);

        BOOL            initLog                 (PCSTR          logPath,
                                                 PCSTR          logName,
                                                 UINT           logFlags,
                                                 AXLOGDL        logLevel)
                        {
                            return initLog(logPath, logName,
                                           logFlags, AXLOGDL_debug, 0, 0);
                        };

        BOOL            initLog                 (PCSTR          logPath,
                                                 PCSTR          logName,
                                                 UINT           logFlags)
                        {
                            return initLog(logPath, logName,
                                           logFlags, AXLOGDL_debug);
                        };

        BOOL            initDebug               (PCSTR          params);

        void            log_l                   (AXLOGDL        v_level,
                                                 const char *   msg,
                                                 va_list        st_va_list);

        void            log                     (AXLOGDL        v_level,
                                                 const char *   msg,
                                                 ...);

        PCSTR           nextParameter           (void);

        void            periodicTurn            (void);

virtual BOOL            endashedParameter       (INT            param);

virtual BOOL            parameter               (PSTR           param)
                        {
                            return true;
                        }

virtual BOOL            appInit                 (void)
                        {
                            return true;
                        }

virtual int             appMain                 (void)
                        {
                            return 0;
                        }

virtual void            onCrash                 (void)
                        {}

virtual void            setCrashDumpDir         (PCSTR   baseDir,
                                                 PCSTR   releaseTag,
                                                 PCSTR   binaryFname);

        PCSTR           getConfigFileName       ()
        { return configFilename; }
};

#endif

#endif //  #ifndef __AXAPPH__


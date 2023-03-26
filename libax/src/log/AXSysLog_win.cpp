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

//#include <stdarg.h>
//#include <string.h>

#ifndef __GNUC__

#include <windows.h>

#include <Strsafe.h>

#include <AXSysLog.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define RIGSREGEVENTSRC     "SYSTEM\\CurrentControlSet\\Services\\Eventlog\\Application"


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      registerSource
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PCSTR   identifier --
// RESULT
//      BOOL --
// ***************************************************************************
static BOOL registerSource(PCSTR    identifier)
{
    BOOL            b_result            = FALSE;
    HKEY            h_key;
    HKEY            h_subkey;
    UINT            size;
    DWORD           d_types = 7;                // Supported types
    CHAR            sz_filename         [ AXLPATH ];


    ENTER(TRUE);

    if ((size = GetModuleFileName(NULL, (LPTSTR)sz_filename, AXLPATH)) > 0)
    {
        if (RegOpenKey(HKEY_LOCAL_MACHINE, RIGSREGEVENTSRC, &h_key) == ERROR_SUCCESS)
        {
            if (RegCreateKey(h_key, identifier, &h_subkey) == ERROR_SUCCESS)
            {
                if ((RegSetValueEx(h_subkey,
                                   "EventMessageFile", 0, REG_EXPAND_SZ,
                                   (PBYTE)sz_filename,
                                   size)    == ERROR_SUCCESS)           &&
                    (RegSetValueEx(h_subkey,
                                   "TypesSupported", 0,
                                   REG_DWORD,
                                   (PBYTE)&d_types,
                                   sizeof(REG_DWORD)) == ERROR_SUCCESS) )
                {
                    b_result = TRUE;
                }

                RegCloseKey(h_subkey);
            }

            RegCloseKey(h_key);
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      cvt
// TYPE
//      inline
// PURPOSE
//
// PARAMETERS
//      AXLOGDL   a --
// RESULT
//      WORD --
// ***************************************************************************
inline WORD cvt(AXLOGDL a)
{
#define MAC_CH(a,b)     case AXLOGDL_##a: type = EVENTLOG_##b##_TYPE; break

    WORD        type    = EVENTLOG_INFORMATION_TYPE;

    ENTER(TRUE);

    switch (a)
    {
        MAC_CH(debug,   INFORMATION);
        MAC_CH(info,    INFORMATION);
        MAC_CH(notice,  INFORMATION);
        MAC_CH(warning, WARNING);
        MAC_CH(error,   ERROR);
        MAC_CH(alert,   ERROR);
        MAC_CH(crit,    ERROR);
        MAC_CH(emerg,   ERROR);
    }

    RETURN(type);
}
// ***************************************************************************
// FUNCTION
//      AXSysLog::close
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//      void  --
// ***************************************************************************
void AXSysLog::close(void)
{
    if (source)
    {
        DeregisterEventSource(source);

        source = NULL;
    }
}
// ***************************************************************************
// FUNCTION
//      AXSysLog::open
// PURPOSE
//
// PARAMETERS
//      PCSTR   identifier --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL AXSysLog::open(PCSTR identifier)
{
    BOOL            b_result        = TRUE;

    ENTER(TRUE);

    if (!source)
    {
        registerSource(identifier);

        if ((source = RegisterEventSource((LPCSTR)nil, (LPCSTR)identifier)) == nil)
        {
            b_result = FALSE;
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXSysLog::log_l
// PURPOSE
//
// PARAMETERS
//      AXLOGDL        v_level    --
//      const char *   msg        --
//      va_list        st_va_list --
// RESULT
//      void  --
// ***************************************************************************
void AXSysLog::log_l(AXLOGDL        v_level,
                     const char *   msg,
                     va_list        st_va_list)
{
    PSTR            buff;
    LPCSTR          psz_tmp;
    UINT            size        = 2048;

    ENTER(TRUE);

    if (source)
    {
        if ((buff = (PSTR)MALLOC(size)) != nil)
        {
            StringCbVPrintf(buff, size, msg, st_va_list);
            //strz_vsformat(buff, size, msg, st_va_list);

            psz_tmp = buff;

#if (TARGET_ARCH != __AX_x64__)
            ReportEvent(source, cvt(v_level), 0, 0, nil, 1, 0, &psz_tmp, nil);
#endif

            FREE(buff);
        }
    }

    QUIT;
}

#endif

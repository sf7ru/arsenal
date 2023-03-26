// ***************************************************************************
// TITLE
//      System Service for Win32 Platform
// PROJECT
//      Arsenal Library
// NOTE
//      This implemela6tion of System Service mechanism is NOT REENTIRABLE!
//      That means that you unable to use 'la6_service_reborn' more than once
// ***************************************************************************
// FILE
//      $Id: service.c,v 1.3 2003/08/06 12:25:58 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************

#include <process.h>
#include <stdlib.h>
#include <windows.h>

#include <arsenal.h>

#include <dreamix.h>

#if (TARGET_SYSTEM != __AX_wince__)


//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>
//#include <tchar.h>
//#include <iostream>
//#include <signal.h>


// ---------------------------------------------------------------------------
// ----------------------------- STATIC DATA ---------------------------------
// ---------------------------------------------------------------------------

static PLA6SERVICECTL   g_pst_ctrl   = NULL;


// ---------------------------------------------------------------------------
// -------------------------- LOCAL DEFINITIONS ------------------------------
// ---------------------------------------------------------------------------

#define STATUSHANDLE        ((SERVICE_STATUS_HANDLE)g_pst_ctrl->p_int_ctrl)

// ---------------------------------------------------------------------------
// ----------------------------- LOCAL TYPES ---------------------------------
// ---------------------------------------------------------------------------

/*
// ***************************************************************************
// STRUCTURE
//      LA6DMN_INT_CTRL
// ***************************************************************************
typedef struct __tag_LA6DMN_INT_CTRL
{
    SERVICE_STATUS          st_status;
    SERVICE_STATUS_HANDLE   h_status;
} LA6DMN_INT_CTRL, * PLA6DMN_INT_CTRL;
*/

// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      service_console2devnull
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//      int  --
// ***************************************************************************
int service_console2devnull(void)
{
    ENTER(TRUE);

    RETURN(0);
}
// ***************************************************************************
// FUNCTION
//      _set_status
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      SERVICE_STATUS_HANDLE   h_status   --
//      DWORD                   d_state    --
//      DWORD                   d_accept   --
//      DWORD                   d_exitcode --
// RESULT
//      BOOL --
// ***************************************************************************
static BOOL _set_status(SERVICE_STATUS_HANDLE       h_status,
                        DWORD                       d_state,
                        DWORD                       d_exitcode)
{
    BOOL                b_result        = FALSE;
    SERVICE_STATUS      st_status;
    DWORD               d_error;

    ENTER(TRUE);

    st_status.dwServiceType         = SERVICE_WIN32_OWN_PROCESS;
    st_status.dwWin32ExitCode       = d_exitcode;
    st_status.dwCurrentState        = d_state;
    st_status.dwControlsAccepted    = g_pst_ctrl->d_features;
    st_status.dwCheckPoint          = 0;
    st_status.dwWaitHint            = 0;

    if (SetServiceStatus(h_status, &st_status))
    {
        b_result = TRUE;
    }
    else
    {
        d_error = GetLastError();
    }

    RETURN(b_result);
}
// ***************************************************************************
// STATIC FUNCTION
//      _la6_service_WRAPPER_sig_handler
// PURPOSE
//      Main signal handler function of Service
// PARAMETERS
//      DWORD d_control -- Signal code
// RESULT
//      none
// ***************************************************************************
static void _la6_service_WRAPPER_sig_handler(DWORD d_control)
{
    A7SIG      d_sig   = A7SIG_NONE;

    if (g_pst_ctrl)
    {
        switch (d_control)
        {
            case SERVICE_CONTROL_PAUSE:     d_sig = A7SIG_INTR;    break;
            case SERVICE_CONTROL_CONTINUE:  d_sig = A7SIG_GO;      break;
            case SERVICE_CONTROL_SHUTDOWN:  d_sig = A7SIG_PWR;     break;

            case SERVICE_CONTROL_STOP:
                d_sig = A7SIG_QUIT;

                _set_status(STATUSHANDLE, SERVICE_STOP_PENDING, 0);
                break;
        }

        if (d_sig != A7SIG_NONE)
            (*g_pst_ctrl->pfn_sig_handler)(g_pst_ctrl, d_sig);
    }
}
static BOOL WINAPI _consoleHandler(DWORD CEvent)
{
    BOOL        b_result    = FALSE;
    A7SIG       d_sig       = A7SIG_NONE;

    switch(CEvent)
    {
        case CTRL_C_EVENT:          // CTRL+C
        case CTRL_BREAK_EVENT:      // CTRL+BREAK
            d_sig = A7SIG_QUIT;
            break;

        case CTRL_CLOSE_EVENT:      // Program close
        case CTRL_LOGOFF_EVENT:     // User logoff
        case CTRL_SHUTDOWN_EVENT:   // Power shutdown
            d_sig = A7SIG_PWR;
            break;
    }

    if (d_sig != A7SIG_NONE)
    {
        (*g_pst_ctrl->pfn_sig_handler)(g_pst_ctrl, d_sig);
        b_result = TRUE;
    }

    return b_result;
}
void la6_service_sighandler(PLA6SERVICECTL pst_ctrl)
{
    g_pst_ctrl = pst_ctrl;
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)_consoleHandler, TRUE);
}
// ***************************************************************************
// STATIC FUNCTION
//      _la6_service_WRAPPER_main
// PURPOSE
//      Main function of Service
// PARAMETERS
//      DWORD      d_argc    -- Number of arguments
//      LPCSTR   * ppsz_argv -- Pointer to array of  pointers to arguments
// RESULT
//      none
// ***************************************************************************
static void _la6_service_WRAPPER_main(DWORD d_argc, LPCSTR * ppsz_argv)
{
    DWORD       d_code;

    if (g_pst_ctrl)
    {
        d_code =
            ((g_pst_ctrl->d_features & LA6DMN_CAN_STOP)     ? SERVICE_ACCEPT_STOP           : 0) |
            ((g_pst_ctrl->d_features & LA6DMN_CAN_PAUSE)    ? SERVICE_ACCEPT_PAUSE_CONTINUE : 0) |
            ((g_pst_ctrl->d_features & LA6DMN_CAN_SHUTDOWN) ? SERVICE_ACCEPT_SHUTDOWN       : 0) ;

        g_pst_ctrl->d_features = d_code;

        _set_status(STATUSHANDLE, SERVICE_START_PENDING, 0);

        if ((g_pst_ctrl->p_int_ctrl = (PVOID)
                RegisterServiceCtrlHandler(g_pst_ctrl->psz_name,
                    (LPHANDLER_FUNCTION)_la6_service_WRAPPER_sig_handler))
                                            != (SERVICE_STATUS_HANDLE)0)
        {
            _set_status(STATUSHANDLE, SERVICE_RUNNING, 0);

            d_code = (*g_pst_ctrl->pfn_entry_point)(g_pst_ctrl);

            _set_status(STATUSHANDLE, SERVICE_STOPPED,
                        d_code ? ERROR_SERVICE_SPECIFIC_ERROR : NO_ERROR);

            // We should exit immidietly after SERVICE_STOPPED,
            // or will be trouble.
            _exit(0);

            //g_pst_ctrl = NULL;
        }
    }
}
// ***************************************************************************
// FUNCTION
//      la6_service_reborn
// PURPOSE
//      Spawn to Service
// PARAMETERS
//      PLA6SERVICECTL pst_ctrl -- Pointer to Control structure
// RESULT
//      BOOL -- TRUE if all is ok or FALSE if error has occured
// ***************************************************************************
BOOL la6_service_reborn(PLA6SERVICECTL pst_ctrl)
{
    BOOL                    b_result            = FALSE;

    SERVICE_TABLE_ENTRY     st_dispatch_table   [] =
    {
        { NULL, (LPSERVICE_MAIN_FUNCTION)_la6_service_WRAPPER_main   },
        { NULL, NULL                                                }
    };

    if (pst_ctrl && pst_ctrl->psz_name &&
        pst_ctrl->pfn_entry_point && pst_ctrl->pfn_sig_handler)
    {
        g_pst_ctrl = pst_ctrl;

        st_dispatch_table[0].lpServiceName = pst_ctrl->psz_name;

        StartServiceCtrlDispatcher(st_dispatch_table);
        // Unreachible point
    }

    return b_result;
}
// ***************************************************************************
// FUNCTION
//      la6_service_register
// PURPOSE
//      Register current executable as System Service
// PARAMETERS
//      PSTR psz_name   -- Name of Service
//      PSTR psz_desc   -- Description of Service
//      PSTR psz_user   -- User account name of Service's owner
//      PSTR psz_passwd -- Password for User account
// RESULT
//      BOOL -- TRUE if all is ok or FALSE if error has occured
// ***************************************************************************
BOOL la6_service_register(PSTR psz_name, PSTR psz_desc, PSTR psz_user, PSTR psz_passwd)
{
    BOOL        b_result            = FALSE;
    SC_HANDLE   schService;
    SC_HANDLE   schSCManager;
    CHAR        sz_module           [A7LPATH];


    if (psz_name &&
        la6_module_get_filename(sz_module, A7LPATH))
    {
        if ((schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE)) != NULL)
        {
            if ((schService = CreateService(
                        schSCManager,               // SCManager database
                        psz_name,                   // name of service
                        psz_desc,                   // service name to display
                        SERVICE_ALL_ACCESS,         // desired access
                        SERVICE_WIN32_OWN_PROCESS,  // service type
                        SERVICE_DEMAND_START,       // start type
                        SERVICE_ERROR_NORMAL,       // error control type
                        sz_module,                  // service's binary
                        NULL,                       // no load ordering group
                        NULL,                       // no tag identifier
                        NULL,                       // no dependencies
                        psz_user,                   // account
                        psz_passwd                  // password
                        )) != NULL)
            {
                b_result = TRUE;
                CloseServiceHandle(schService);
            }

            CloseServiceHandle(schSCManager);
        }
    }

    return b_result;
}
// ***************************************************************************
// FUNCTION
//      la6_service_unregister
// PURPOSE
//      Unregister current executable as System Service
// PARAMETERS
//      PSTR psz_name -- Name of Service
// RESULT
//      BOOL -- TRUE if all is ok or FALSE if error has occured
// ***************************************************************************
BOOL la6_service_unregister(PSTR psz_name)
{
    BOOL        b_result            = FALSE;
    SC_HANDLE   schService;
    SC_HANDLE   schSCManager;

    if (psz_name)
    {
        if ((schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE)) != NULL)
        {
            if ((schService = OpenService(schSCManager, psz_name, DELETE)) != NULL)
            {
                b_result = DeleteService(schService);
                CloseServiceHandle(schService);
            }

            CloseServiceHandle(schSCManager);
        }
    }

    return b_result;
}

#endif                                      //  #if (TARGET_SYSTEM...

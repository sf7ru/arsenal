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

#ifndef __AXSERVICEH__
#define __AXSERVICEH__

#include <axsystem.h>
#include <axthreads.h>
#include <axtime.h>
#include <axstring.h>

#include <AXApp.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define AXDMN_CAN_PAUSE     0x00000001
#define AXDMN_CAN_STOP      0x00000002
#define AXDMN_CAN_SHUTDOWN 0x00000004



// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef BOOL    (*AXDMN_SIG_HANDLER)(PVOID, AXSIG);
typedef UINT    (*AXDMN_ENTRY_POINT)(PVOID);


// ***************************************************************************
// STRUCTURE
//      AXSERVICECTL
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_AXSERVICECTL
{
    UINT                d_features;
    PVOID               p_int_ctrl;
    PSTR                psz_name;
    PSTR                psz_desc;
    PVOID               p_ptr;
    AXDMN_ENTRY_POINT   pfn_entry_point;
    AXDMN_SIG_HANDLER   pfn_sig_handler;
} AXSERVICECTL, * PAXSERVICECTL;

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

#ifdef __cplusplus

class AXService: public AXApp
{
protected:
        BOOL            stopRequest;

        int             action;
        AXTIME          stopTimed;

/*
        BOOL            unregisterService       (PSTR           name);

        BOOL            registerService         (PSTR           name,
                                                 PSTR           desc);
*/
virtual BOOL            endashedParameter       (INT            param);

static  U32             service                 (PAXSERVICECTL serviceCtl);

        int             appMain                 (void);

        BOOL            isStopRequested         ();

        void            setTimedStop            (UINT           secs);
public:

        CHAR            resourcesFilename       [ AXLPATH  ];
        CHAR            name                    [ AXLSHORT ];
        CHAR            desc                    [ AXLLONG  ];

                        AXService               (void);

virtual                 ~AXService              (void);

virtual BOOL            signalHandler           (AXSIG          sig)
                        {
                            if (sig == AXSIG_QUIT)
                            {
                                stopRequest = true;
                            }

                            return true;
                        }

virtual BOOL            init                    (void)
                        {
                            return true;
                        }

virtual int             serviceMain             (void)
                        {
                            return 0;
                        }

virtual void            showHelp()
                        { }

        BOOL            lock                    (PCSTR         name);
        
        void            unlock                  (PCSTR         name);


};

#endif


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif

int                 service_console2devnull     (void);

BOOL                axservice_register          (PSTR                   psz_name,
                                                 PSTR                   psz_desc,
                                                 PSTR                   psz_user,
                                                 PSTR                   psz_passwd);

BOOL                axservice_unregister        (PSTR                   psz_name);

void                axservice_sighandler        (PAXSERVICECTL          pst_ctrl);

BOOL                axservice_reborn            (PAXSERVICECTL          pst_ctrl);

#ifdef __cplusplus
}
#endif

#endif //  #ifndef __AXSERVICEH__


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


#ifndef __AXSYSTEMH__
#define __AXSYSTEMH__

#include <arsenal.h>
#include <axutils.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define axdev_ctl(dev,func,pparam,uparam)            (dev)->pfn_ctl(dev,func,pparam,uparam)
#define axdev_close(dev,to)             (dev)->pfn_close(dev,to)
#define axdev_read(dev,data,size,to)    (dev)->pfn_read(dev,data,size,to)
#define axdev_write(dev,data,size,to)   (dev)->pfn_write(dev,data,size,to)
#define axdev_seek(dev,b,c)             (dev)->pfn_ctl(dev,AXDEVCTL_SEEK,(PVOID)(c),b)
#define axdev_accept(a,b)               ((PAXDEV)((a)->pfn_accept != nil ? (a)->pfn_accept(a,b) : nil))

#define AXMSGQUEUE_MAX_SIZE 4056


#define AXDEVFL_READ                0x00000001
#define AXDEVFL_WRITE               0x00000002
#define AXDEVFL_FULL                (AXDEVFL_READ | AXDEVFL_WRITE)

#define AXDEVFL_NBIO                0x00000080


#define AXDEVCTL_BREAK      1
#define AXDEVCTL_SETSPEED   10
#define AXDEVCTL_ESC        27
#define AXDEVCTL_SEEK       3

#define AXDEVCTL_GETFD      100

// -------------------------- LA6PROCESSATTRIB -------------------------------

typedef UINT                    AXPROCESSATTRIB;
#define AXPSA_STDPIPES          0x00000100
#define AXPSA_HIDEN             0x00000010
#define AXPSA_DETACH            0x00000020



// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef void (*PFNAXITIMERCB)(PVOID, UINT ticks);

typedef struct __tag_HAXSEM         {UINT u_id;} * HAXSEM;
typedef struct __tag_HAXMSGQUEUE    {UINT u_id;} * HAXMSGQUEUE;
typedef struct __tag_HAXMSGQUEUEX   {UINT u_id;} * HAXMSGQUEUEX;
typedef struct __tag_HAXEVENT       {UINT u_id;} * HAXEVENT;
typedef struct __tag_HAXPROCESS     {UINT u_id;} * HAXPROCESS;
typedef struct __tag_HAXPIPE        {UINT u_id;} * HAXPIPE;


// ***************************************************************************
// ENUMERATION
//      AXSIG
// PURPOSE
//      Signals
// ***************************************************************************
typedef enum
{
        AXSIG_NONE             = 0,        // Nothing (can be used as keep-
                                            // alive signal)
        AXSIG_TERM,                        // Termination
        AXSIG_QUIT,                        // Gently quit
        AXSIG_INTR,                        // Interrupt
        AXSIG_GO,                          // Go ahead
        AXSIG_PWR,                         // Power down
        AXSIG_INIT,                        // Initialization
        AXSIG_HUP                          // Hang up (only for ttys)
} AXSIG;


// ***************************************************************************
// ENUMERATION
//      AXERR
// PURPOSE
//      Error constants
// ***************************************************************************
typedef enum
{
    AXERR_ok            = 0,
    AXERR_unknown
} AXERR;

typedef INT (*PAXDEVFNCTL)(PVOID,UINT,PVOID,UINT);
typedef INT (*PAXDEVFNCLOSE)(PVOID,UINT);
typedef INT (*PAXDEVFNREAD)(PVOID,PVOID,UINT,UINT);
typedef INT (*PAXDEVFNWRITE)(PVOID,PVOID,UINT,UINT);
typedef PVOID (*PAXDEVFNACCEPT)(PVOID,UINT);

// ***************************************************************************
// STRUCTURE
//      AXDEV
// PURPOSE
//      Device control structure
// ***************************************************************************
typedef struct __tag_AXDEV
{
    PAXDEVFNCTL     pfn_ctl;
    PAXDEVFNCLOSE   pfn_close;
    PAXDEVFNREAD    pfn_read;
    PAXDEVFNWRITE   pfn_write;
    PAXDEVFNACCEPT  pfn_accept;
} AXDEV, * PAXDEV;


// ***************************************************************************
// STRUCTURE
//      AXMSG
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_AXMSG
{
    UINT            u_type;
    UINT            u_size;
    PVOID           p_data;
    PCSTR           psz_address;
} AXMSG, * PAXMSG;


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif                                      //  #ifdef __cplusplus

PAXDEV              namedpipe_close             (PAXDEV             pst_pipe);

PAXDEV              namedpipe_connect           (PSTR               psz_name,
                                                 UINT               u_TO);

PAXDEV              namedpipe_create            (PSTR               psz_name);


// -------------------------- IPC/?/msgqueue.c -------------------------------

HAXMSGQUEUE         axmsgqueue_close            (HAXMSGQUEUE        pst_queue);

HAXMSGQUEUE         axmsgqueue_open             (PSTR               psz_key,
                                                 UINT               d_max_size,
                                                 BOOL               b_create);

BOOL                axmsgqueue_send             (HAXMSGQUEUE        pst_queue,
                                                 PAXMSG           pst_message,
                                                 BOOL               b_wait);

PAXMSG              axmsgqueue_recv             (HAXMSGQUEUE        pst_queue,
                                                 UINT               d_type,
                                                 BOOL               b_wait);

// --------------------------- IPC/msgqueueX.c -------------------------------

HAXMSGQUEUEX        axmsgqueueX_close           (HAXMSGQUEUEX       h_queue);

HAXMSGQUEUEX        axmsgqueueX_open            (PSTR               psz_key,
                                                 UINT               d_max_size,
                                                 BOOL               b_create);

BOOL                axmsgqueueX_send            (HAXMSGQUEUEX       h_queue,
                                                 PAXMSG             pst_message,
                                                 BOOL               b_wait);

PAXMSG              axmsgqueueX_recv            (HAXMSGQUEUEX       h_queue,
                                                 UINT               d_type,
                                                 BOOL               b_wait);

BOOL                axitimer_set                (PFNAXITIMERCB      pfn_cb,
                                                 PVOID              p_ptr,
                                                 UINT               u_sec,
                                                 UINT               u_usec);

BOOL                axitimer_stop               (void);


HAXSEM              axsem_destroy               (HAXSEM             h_event);

HAXSEM              axsem_create                (int                id,
                                                 BOOL               initial);

BOOL                axsem_wait                  (HAXSEM             h_event,
                                                 UINT               TO);

BOOL                axsem_get                   (HAXSEM             h_event);

BOOL                axsem_set                   (HAXSEM             h_event);


// ------------------------------ message.c ----------------------------------


PAXMSG              axmsg_destroy               (PAXMSG                 msg);


HAXEVENT            axevent_destroy             (HAXEVENT              h_event);

HAXEVENT            axevent_create              (BOOL                   b_initial,
                                                 BOOL                   b_manual_reset);

BOOL                axevent_wait                (HAXEVENT              h_event,
                                                 UINT                  d_timeout);

UINT                axevent_wait_multi          (HAXEVENT *            ph_events,
                                                 UINT                   u_number,
                                                 UINT                   d_timeout_msecs);

BOOL                axevent_set                 (HAXEVENT              h_event,
                                                 BOOL                   b_signaled);

// ------------------------------ process.c ----------------------------------

HAXPROCESS          axprocess_destroy           (HAXPROCESS         h_process);

HAXPROCESS          axprocess_create            (PSTR               filename,
                                                 PSTR               cmdline,
                                                 UINT               flags);

UINT                axprocess_get_id            (HAXPROCESS         h_process);

HAXPIPE             axprocess_stdin             (HAXPROCESS         h_process);

HAXPIPE             axprocess_stdout            (HAXPROCESS         h_process);

BOOL                axprocess_get_exit_code     (HAXPROCESS         h_process,
                                                 PUINT              pd_result);

BOOL                axprocess_hup               (HAXPROCESS         h_process);

int                 axprocess_wait              (HAXPROCESS         h_process,
                                                 UINT               u_TO);

BOOL                axprocess_kill              (HAXPROCESS         h_process,
                                                 UINT               d_exit_code);

BOOL                axprocess_is_alive          (HAXPROCESS         h_process);

BOOL                axprocess_is_alive2         (UINT               pid);


// ------------------------------- pipe.c -----------------------------------

HAXPIPE             axpipe_destroy              (HAXPIPE            h_pipe);

HAXPIPE             axpipe_create               (UINT               d_buff_size);

BOOL                axpipe_set_buff_size        (HAXPIPE            h_pipe,
                                                 UINT               size);

INT                 axpipe_read                 (HAXPIPE            h_pipe,
                                                 PVOID              p_data,
                                                 INT                i_size,
                                                 UINT               u_timeout_msec);

INT                 axpipe_write                (HAXPIPE            h_pipe,
                                                 PVOID              p_data,
                                                 INT                i_size,
                                                 UINT               u_timeout_msec);


INT                 axmodule_get_filename       (PSTR               psz_buffer,
                                                 INT                size);

INT                 axmodule_get_path           (PSTR               psz_buffer,
                                                 INT                size);

#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus

#endif  // #ifndef __AXSYSTEMH__


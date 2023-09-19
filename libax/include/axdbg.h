// ***************************************************************************
// TITLE
//      Common debug Header
// PROJECT
//      Arsenal
// ***************************************************************************
//
// FILE
//      $Id:$
// HISTORY
//      $Log:$
// ***************************************************************************

#ifndef __AXDBGH___
#define __AXDBGH___

#define __FILELINE__2(a,b,c)  a "(" #b ") " c
#define __FILELINE__1(a,b,c)  __FILELINE__2(a,b,c)
#define __FILELINE__(a)        __FILELINE__1(__FILE__, __LINE__,a)

#if (BUILD_TOOL == __AX_msvc__)
#define __AXPLACE__    __FILELINE__(__FUNCTION__)
#elif (BUILD_TOOL == __AX_gcc__)
#define __AXPLACE__    __FILELINE__("")
#else
#define __AXPLACE__    __FILELINE__("")
#endif

#include <stdarg.h>
#include <axdbgmem.h>
#include <axdbgtrace.h>


#if (defined(DBGSYS))
#define DEBUGSTART(a)
#else
#define DEBUGSTART(a)
#endif

#if (defined(DBGSYS))
#define STARTDEBUGGER(a)    dbg_debugger_start(a)
#define SETDEBUGGER(a)      dbg_debugger_set(a)
#define GETDEBUGGER         dbg_debugger_get()
#else
#define STARTDEBUGGER(a)    true
#define SETDEBUGGER(a)
#define GETDEBUGGER         nil
#endif

#if (defined(DBGSYS))
#define STOPDEBUGGER()      dbg_debugger_stop()
#else
#define STOPDEBUGGER()
#endif


// URL: http://www.pixelbeat.org/programming/gcc/static_assert.html
#define ASSERT_CONCAT_(a, b) a##b
#define ASSERT_CONCAT(a, b) ASSERT_CONCAT_(a, b)
/* These can't be used after statements in c89. */
#ifdef __COUNTER__
  #define STATIC_ASSERT(e,m) \
    ;enum { ASSERT_CONCAT(static_assert_, __COUNTER__) = 1/(!!(e)) }
#else
  /* This can't be used twice on the same line so ensure if using in headers
   * that the headers are not included twice (by wrapping in #ifndef...#endif)
   * Note it doesn't cause an issue when used on same line of separate modules
   * compiled with gcc -combine -fwhole-program.  */
  #define STATIC_ASSERT(e,m) \
    ;enum { ASSERT_CONCAT(assert_line_, __LINE__) = 1/(!!(e)) }
#endif



// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

// ***************************************************************************
// ENUMERATION
//      DBGET
// PURPOSE
//      Debug event type
// ***************************************************************************
typedef enum
{
    DBGET_normal               = 0,
    DBGET_warning,
    DBGET_error,
    DBGET_fatal
} DBGET;



// ***************************************************************************
// ENUMERATION
//      DBGET
// PURPOSE
//      Debug event type
// ***************************************************************************


// ***************************************************************************
// STRUCTURE
//      DBGMCB
// PURPOSE
//      Debug memory control block
// ***************************************************************************
typedef struct _tag_DBGMCB
{
    PVOID         ptr;
    UINT          size;
    PCSTR         purpose;
    PVOID         next;
} DBGMCB, * PDBGMCB;


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif                                      //  #ifdef __cplusplus

void                dbg_debugger_stop           (void);

BOOL                dbg_debugger_start          (PCSTR              module);

HAXHANDLE           dbg_debugger_get            ();

void                dbg_debugger_set            (HAXHANDLE          handle);

int                 dbg_time                    (void);

void                dbg_event                   (DBGET              v_evt,
                                                 PCSTR              psz_place,
                                                 PCSTR              psz_message,
                                                 ...);

void                dbg_event_v                 (DBGET              v_evt,
                                                 PCSTR              psz_place,
                                                 PCSTR              psz_message,
                                                 va_list            list);

UINT                dbg_event_make_v            (PVOID              buff,
                                                 UINT               size,
                                                 PCSTR              psz_place,
                                                 PCSTR              psz_message,
                                                 va_list            list);



PSTR                dbg_strcpyn                 (PSTR               psz_t,
                                                 PSTR               psz_s,
                                                 UINT               u_size);

PSTR                dbg_dump                    (PSTR               psz_string,
                                                 UINT               d_string_width,
                                                 PVOID              p_data,
                                                 UINT               d_size);


U8                  dbgsys_crc8_calc            (PVOID              p_data,
                                                 UINT               u_size);

U16                 dbgsys_crc16_calc           (PVOID              p_data,
                                                 UINT               u_size);

HAXHANDLE           dbgsys_mutex_destroy        (HAXHANDLE          h_lock);

HAXHANDLE           dbgsys_mutex_create         (void);

BOOL                dbgsys_mutex_unlock         (HAXHANDLE          h_lock);

BOOL                dbgsys_mutex_lock           (HAXHANDLE          h_lock);

HAXHANDLE           dbgsys_msgqueue_open        (PCSTR              key);

HAXHANDLE           dbgsys_msgqueue_close       (HAXHANDLE          handle);

INT                 dbgsys_msgqueue_send        (HAXHANDLE          queue,
                                                 PVOID              data,
                                                 INT                size);

UINT                dbgsys_get_pid              ();

UINT                dbgsys_get_tid              ();

U32                 dbgsys_get_usecs            ();

HAXHANDLE           dbgsys_process_close        (HAXHANDLE          handle);

HAXHANDLE           dbgsys_process_run          (PCSTR              psz_filename,
                                                 PCSTR              psz_cmdline,
                                                 BOOL               hide);


HAXHANDLE           dbglink_satellite_open      (PCSTR              psz_opts);

HAXHANDLE           dbglink_satellite_close     (HAXHANDLE          handle);

INT                 dbglink_satellite_send      (HAXHANDLE          handle,
                                                 PVOID              msg,
                                                 INT                size);
// DEBUG
void                applog                      (PCSTR              message,
                                                 ...);

// INFO
void                applogi                     (PCSTR              message,
                                                 ...);


#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus

#endif                                      //  #ifndef __AXDBGH___



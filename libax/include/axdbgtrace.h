// ***************************************************************************
// TITLE
//      Tracing macros and functions
// PROJECT
//      Arsenal
// ***************************************************************************
//
// FILE
//      $Id:$
// HISTORY
//      $Log:$
// ***************************************************************************

#ifndef __AXDBGTRACEH___
#define __AXDBGTRACEH___


// ===========================================================================
// ================================= COMMON ==================================
// ===========================================================================

#define MAIN_ENTER(a,b)      if (STARTDEBUGGER(a)) ENTER(b)
#define MAIN_RETURN(a)       /*STOPDEBUGGER();*/ RETURN(a)

#define FUNCTION_NOT_IMPLEMENTED     FIXME("not implemented")


#if ( defined(DBGSOURCE) || !defined(DBGFULL) )

// ===========================================================================
// ============================= NON-DEBUG MODE ==============================
// ===========================================================================

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// ---------------------------------------------------------------------------

//#define ENTER(a)            if (a) {
//#define QUIT                }
//#define RETURN(a)           } return a

#define ENTER(condition)
#define QUIT                
#define RETURN(result)              return result

#define TX_ENTER(handle,condition)  if (condition) { axthreadX_sig_send(handle, AXSIG_GO, 0);
#define TX_EXIT(handle,code)        } axthreadX_sig_send(handle, AXSIG_QUIT, 0); axthreadX_exit(code);

#define ASSERT(condition,msg)
#define FIXME(msg)

#else                                       //  #ifdef DBGTRACE

// ===========================================================================
// =============================== DEBUG MODE ================================
// ===========================================================================

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// ---------------------------------------------------------------------------

#define ASSERT(condition,msg)   if (condition) axdbg_trace msg

#ifdef DBGTRACEFUNCTIONS
#define ENTER(condition)    { axdbg_enter(__AXPLACE__, #condition, (BOOL)(condition))
#define QUIT                axdbg_quit(__AXPLACE__); }
#define RETURN(result)      QUIT; return result
#else
#define ENTER(condition)    if (condition) {
#define QUIT                }
#define RETURN(result)      } return result
#endif

#define TX_ENTER(handle, condition) ENTER(condition); { axthreadX_sig_send(handle, AXSIG_GO, 0);
#define TX_EXIT(handle, code)       } axthreadX_sig_send(handle, AXSIG_QUIT, 0); axthreadX_exit(code); QUIT

#define FIXME(msg)            axdbg_trace("FIXME! %s: %s\n", __AXPLACE__, msg)

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif                                      //  #ifdef __cplusplus

void                axdbg_enter                 (PCSTR              psz_desc,
                                                 PCSTR              psz_cond,
                                                 BOOL               b_cond);

void                axdbg_quit                  (PCSTR              psz_desc);


#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus

#endif                                      //  #ifndef DBGMEM



#if (   defined(DBGSOURCE)                              ||              \
        !( (defined(DBGFULL)  || defined(DBGSYS) || defined(DEBUG)) &&  \
           (defined(DBGTRACE) || defined(APPTRACE)) )               )

#ifndef _MSC_VER
#ifndef AXTRACE
#define AXTRACE(...)        {}
#endif
#else 
#define AXTRACE
#endif

#else

#ifndef _MSC_VER
#ifndef AXTRACE
#ifdef APPTRACE
#define AXTRACE(...)        { app_trace(__AXPLACE__, ##__VA_ARGS__); }
#else
#define AXTRACE(...)        { axdbg_trace(__AXPLACE__, ##__VA_ARGS__); }
#endif
#endif
#endif

#define ASSERT(a,b)         if (a) { printf("ASSERT! %s: %s\n", __AXPLACE__, b); }
#define FIXME(a)            printf("FIXME! %s: %s\n", __AXPLACE__, a)

#ifdef __cplusplus
extern "C" {
#endif                                      //  #ifdef __cplusplus

void                axdbg_trace                 (PCSTR              psz_place,
                                                 PCSTR              psz_msg,
                                                 ...                );

void                app_trace                   (PCSTR              psz_place,
                                                 PCSTR              psz_msg,
                                                 ...                );


#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus

#endif

#endif                                      //  #ifndef __AXDBGTRACEH___



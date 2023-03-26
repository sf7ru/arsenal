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

#ifndef __AXSYSLOGH__
#define __AXSYSLOGH__

#include <axlog.h>

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

#ifdef __cplusplus

class AXSysLog
{
        PVOID           source;

public:

                        AXSysLog                (void);
                        ~AXSysLog               (void);


        void            close                   (void);

        BOOL            open                    (PCSTR          identifier);

        void            log_l                   (AXLOGDL        v_level,
                                                 const char *   msg,
                                                 va_list        st_va_list);

        void            log                     (AXLOGDL        v_level,
                                                 const char *   msg,
                                                 ...);
};

#endif

#endif //  #ifndef __AXSYSLOGH__


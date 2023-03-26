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

#ifndef __IAP_H__
#define __IAP_H__

#include <arsenal.h>
#include <app.h>
#include <singleton_template.hpp>
#include <Flash.h>

#include <customboard.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#if (!defined(CONF_IAP_FL_BOOT_SZ) && defined(CONF_FLASH_FL_BOOT_SZ))
#define CONF_IAP_FL_BOOT_SZ     CONF_FLASH_FL_BOOT_SZ
#endif

#if (!defined(CONF_IAP_FL_APP_SZ) && defined(CONF_IAP_FL_APP_SZ))
#define CONF_IAP_FL_APP_SZ      CONF_FLASH_FL_APP_SZ
#endif

#if (!defined(CONF_IAP_FL_UPDATE_SZ) && defined(CONF_IAP_FL_UPDATE_SZ))
#define CONF_IAP_FL_UPDATE_SZ   CONF_FLASH_FL_UPDATE_SZ
#endif

#if (!defined(CONF_IAP_FL_LOG_SZ) && defined(CONF_IAP_FL_LOG_SZ))
#define CONF_IAP_FL_LOG_SZ      CONF_FLASH_FL_LOG_SZ
#endif

#if (!defined(CONF_IAP_FL_VAR_SZ) && defined(CONF_IAP_FL_VAR_SZ))
#define CONF_IAP_FL_VAR_SZ      CONF_FLASH_FL_VAR_SZ
#endif

#if (!defined(CONF_IAP_FL_CONFIG_SZ) && defined(CONF_IAP_FL_CONFIG_SZ))
#define CONF_IAP_FL_CONFIG_SZ   CONF_FLASH_FL_CONFIG_SZ
#endif


// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class Iap: public SingletonTemplate<Iap>
{
public:

        U32             readId                  ();

        INT             readSerial              (PVOID          buff,
                                                 INT            size);


        BOOL            blankCheck              (int            start,
                                                 int            end);

        BOOL            erase                   (int            start,
                                                 int            end);

        BOOL            prepare                 (int            start,
                                                 int            end);

        BOOL            unprepare               (int            start,
                                                 int            end);

        int             write                   (void *         target_addr,
                                                 void *         source_addr,
                                                 int            size);

        BOOL            compare                 (void *         target_addr,
                                                 void *         source_addr,
                                                 int            size);

        int             readBootVer             ();

        BOOL            getFileProps            (PFLASHFILEPROPS props,
                                                 UINT           fileId);

        UINT            getSectorSizeByNum      (UINT           idx);

};

#endif    //  #ifndef  __IAP_H__

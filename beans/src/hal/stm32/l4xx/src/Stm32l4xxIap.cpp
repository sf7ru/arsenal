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

#include <arsenal.h>

#include <Iap.h>

#include <stm32l4xx_hal.h>
#include <stm32l4xx_hal_flash.h>

#include <stm32l4xx_aip_pvt.h>

#include <axdbg.h>
#include <stm32l4xx_hal_flash_ex.h>
#include <stm32l4xx_hal_conf.h>

//extern void myprintf(PCSTR, ...);
//
////#undef AXTRACE
//#define AXTRACE myprintf

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define TOTAL_PAGES         ((512 KB) / ADDR_FLASH_PAGE_SZ)
#define TOTAL_FILES         (sizeof(gFiles) / sizeof(FLASHFILEPROPS ))

#define WRITE_SIZE          sizeof(U64)

#ifdef CONFIG_BOOTLD
#define CONF_IAP_FL_BOOT_SZ     16
#endif // ifdef CONFIG_BOOTLD

#ifndef CONF_IAP_FL_BOOT_SZ
#define CONF_IAP_FL_BOOT_SZ     0
#endif

#ifndef CONF_IAP_FL_UPDATE_SZ
#define CONF_IAP_FL_UPDATE_SZ   0
#endif

#ifndef CONF_IAP_FL_LOG_SZ
#define CONF_IAP_FL_LOG_SZ      0
#endif

#ifndef CONF_IAP_FL_VAR_SZ
#define CONF_IAP_FL_VAR_SZ      0
#endif

#ifndef CONF_IAP_FL_CONFIG_SZ
#define CONF_IAP_FL_CONFIG_SZ   1
#endif

#ifndef CONF_IAP_FL_APP_SZ
#define CONF_IAP_FL_APP_SZ      (TOTAL_PAGES -             \
                                 CONF_IAP_FL_CONFIG_SZ -    \
                                 CONF_IAP_FL_VAR_SZ -       \
                                 CONF_IAP_FL_LOG_SZ -       \
                                 CONF_IAP_FL_UPDATE_SZ -    \
                                 CONF_IAP_FL_BOOT_SZ)
#endif

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct __tag_FLASHBLOCK
{
    UINT                    ptr;
    UINT                    size;
} FLASHBLOCK;

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

#define MAC_FILE(page, size) { (PVOID)ADDR_FLASH_PAGE_0 + ((page) * ADDR_FLASH_PAGE_SZ), (page), (size), 0 }

static FLASHFILEPROPS   gFiles      [] =
{
    MAC_FILE(0,                        CONF_IAP_FL_BOOT_SZ),

    MAC_FILE(CONF_IAP_FL_BOOT_SZ,      CONF_IAP_FL_APP_SZ),

    MAC_FILE(CONF_IAP_FL_BOOT_SZ +
             CONF_IAP_FL_APP_SZ,       CONF_IAP_FL_UPDATE_SZ),

    MAC_FILE(CONF_IAP_FL_BOOT_SZ +
             CONF_IAP_FL_APP_SZ    +
             CONF_IAP_FL_UPDATE_SZ,    CONF_IAP_FL_LOG_SZ),

    MAC_FILE(CONF_IAP_FL_BOOT_SZ +
             CONF_IAP_FL_APP_SZ    +
             CONF_IAP_FL_UPDATE_SZ +
             CONF_IAP_FL_LOG_SZ,       CONF_IAP_FL_VAR_SZ),

    MAC_FILE(CONF_IAP_FL_BOOT_SZ +
             CONF_IAP_FL_APP_SZ    +
             CONF_IAP_FL_UPDATE_SZ +
             CONF_IAP_FL_LOG_SZ    +
             CONF_IAP_FL_VAR_SZ,      CONF_IAP_FL_CONFIG_SZ)
};

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      Iap::readId
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      U32 --
// ***************************************************************************
U32 Iap::readId()
{
    //return Chip_IAP_ReadPID();
    return 0;
}
// ***************************************************************************
// FUNCTION
//      Iap::blankCheck
// PURPOSE
//
// PARAMETERS
//      int start --
//      int end   --
// RESULT
//      int --
// ***************************************************************************
BOOL Iap::blankCheck(int            start,
                     int            end)
{
    //return Chip_IAP_BlankCheckSector(start, end) == IAP_CMD_SUCCESS ? true : false;

    return 0;
}
// ***************************************************************************
// FUNCTION
//      Iap::erase
// PURPOSE
//
// PARAMETERS
//      int start --
//      int end   --
// RESULT
//      int --
// ***************************************************************************
BOOL Iap::erase(int            start,
                int            end)
{
    BOOL                    result          = false;
    FLASH_EraseInitTypeDef  es;
    uint32_t                PAGEError;

    AXTRACE("ERASE %u - %u\n", start, end);

    ENTER(true);

    es.TypeErase    = FLASH_TYPEERASE_PAGES;
    es.Banks        = FLASH_BANK_1 + (start / 256);
    es.Page         = start;
    es.NbPages      = ((end - start) + 1);

    AXTRACE("ERASE stage 1: Banks = %d page: %X  number: %u\n", es.Banks, es.Page, es.NbPages);

    result = (HAL_FLASHEx_Erase(&es, &PAGEError) == HAL_OK);

    AXTRACE("ERASE stage 2: result = %d\n", result);


    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      Iap::prepare
// PURPOSE
//
// PARAMETERS
//      int start --
//      int end   --
// RESULT
//      int --
// ***************************************************************************
BOOL Iap::prepare(int            start,
                  int            end)
{
    AXTRACE("IAP PREPARE!!!!\n");
    HAL_FLASH_Unlock();

    return true;
}
// ***************************************************************************
// FUNCTION
//      Iap::unprepare
// PURPOSE
//
// PARAMETERS
//      int start --
//      int end   --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL Iap::unprepare(int            start,
                    int            end)
{
    AXTRACE("IAP UNPREPARE!!!!\n");
    HAL_FLASH_Lock();

    return true;
}
// ***************************************************************************
// FUNCTION
//      Iap::write
// PURPOSE
//
// PARAMETERS
//      void * target_addr --
//      void * source_addr --
//      int    size        --
// RESULT
//      int --
// ***************************************************************************
int Iap::write(void *         target_addr,
               void *         source_addr,
               int            size)
{
    int             result          = -1;
    PU64            ons             = (PU64)source_addr;
    uint32_t        ont             = (uint32_t)target_addr;
    int             left;
    int             rd;

    ENTER(true);

    if (!(size & 7))
    {
        result = size;

        for (left = size; result && left; left -= WRITE_SIZE, ons++, ont += WRITE_SIZE)
        {
            if ((rd = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, ont, *ons)) != HAL_OK)
            {
                AXTRACE("HAL_FLASH_Program FAILED: %d\n", rd);

                result = 0;
            }
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      Iap::compare
// PURPOSE
//
// PARAMETERS
//      void * target_addr --
//      void * source_addr --
//      int    size        --
// RESULT
//      int --
// ***************************************************************************
BOOL Iap::compare(void *         target_addr,
                  void *         source_addr,
                  int            size)
{
//    return Chip_IAP_Compare((uint32_t)target_addr, (uint32_t)source_addr, size) == IAP_CMD_SUCCESS ? size : -1;

    return 0;
}
// ***************************************************************************
// FUNCTION
//      Iap::readBootVer
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      int --
// ***************************************************************************
int Iap::readBootVer()
{
//    return Chip_IAP_ReadBootCode();
    return 0;
}
// ***************************************************************************
// FUNCTION
//      Iap::getFileProps
// PURPOSE
//
// PARAMETERS
//      FLASHFILEPROPS props --
//      StorageFileId  id    --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL Iap::getFileProps(PFLASHFILEPROPS  props,
                       UINT             id)
{
    BOOL            result          = false;
    UINT            idx             = 0;

    ENTER(true);

    if (id < TOTAL_FILES)
    {
        props->begin        = gFiles[id].begin;
        props->ptr          = gFiles[id].ptr;
        props->size         = 0;
        props->sectors      = gFiles[id].sectors;

        for (idx = 0; idx < props->sectors; idx++)
        {
            props->size += ADDR_FLASH_PAGE_SZ;
        }

        result            = true;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      Iap::getSectorSize
// PURPOSE
//
// PARAMETERS
//      UINT idx --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
UINT Iap::getSectorSizeByNum(UINT           idx)
{
    UINT            result          = 0;

    ENTER(true);

    if (idx < TOTAL_PAGES)
    {
        result = ADDR_FLASH_PAGE_SZ;
    }

    RETURN(result);
}

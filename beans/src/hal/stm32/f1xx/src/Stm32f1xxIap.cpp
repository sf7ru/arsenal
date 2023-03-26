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

#include <stm32f1xx_hal.h>
#include <stm32f1xx_hal_flash.h>
//#include <stm32_hal_legacy.h>

#include <stm32f1xx_aip_pvt.h>

#include <axdbg.h>

#include <customboard.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#ifndef CONF_IAP_FLASH_SZ
#define CONF_IAP_FLASH_SZ   (64)
#endif

#define CONF_IAP_FL_BOOT_OFFSET 0
#ifndef CONF_IAP_FL_BOOT_SZ
#define CONF_IAP_FL_BOOT_SZ     14
#endif

#define CONF_IAP_FL_UPDATE_OFFSET   (CONF_IAP_FL_APP_OFFSET + CONF_IAP_FL_APP_SZ)
#ifndef CONF_IAP_FL_UPDATE_SZ
#define CONF_IAP_FL_UPDATE_SZ   0
#endif

#define CONF_IAP_FL_LOG_OFFSET  (CONF_IAP_FL_UPDATE_OFFSET + CONF_IAP_FL_UPDATE_SZ)
#ifndef CONF_IAP_FL_LOG_SZ
#define CONF_IAP_FL_LOG_SZ      0
#endif

#define CONF_IAP_FL_VAR_OFFSET  (CONF_IAP_FL_LOG_OFFSET + CONF_IAP_FL_LOG_SZ)
#ifndef CONF_IAP_FL_VAR_SZ
#define CONF_IAP_FL_VAR_SZ      0
#endif

#define CONF_IAP_FL_CONFIG_OFFSET   (CONF_IAP_FL_VAR_OFFSET + CONF_IAP_FL_VAR_SZ)
#ifndef CONF_IAP_FL_CONFIG_SZ
#define CONF_IAP_FL_CONFIG_SZ   1
#endif

#define CONF_IAP_FL_APP_OFFSET  (CONF_IAP_FL_BOOT_SZ)
#ifndef CONF_IAP_FL_APP_SZ
#define CONF_IAP_FL_APP_SZ      (CONF_IAP_FLASH_SZ -                       \
                                 CONF_IAP_FL_CONFIG_SZ -    \
                                 CONF_IAP_FL_VAR_SZ -       \
                                 CONF_IAP_FL_LOG_SZ -       \
                                 CONF_IAP_FL_UPDATE_SZ -    \
                                 CONF_IAP_FL_BOOT_SZ)
#endif


#define TOTAL_BLOCKS        ((CONF_IAP_FLASH_SZ * 1024) / FLASH_PAGE_SIZE)
#define TOTAL_FILES         (sizeof(gFileDescs) / sizeof(FILEDESC))

#define WRITE_SIZE          sizeof(U32)

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct __tag_FILEDESC
{
    UINT                    ptr;
    UINT                    begin;
    UINT                    size;
} FILEDESC;

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

// 192Kb for firmware and update,
// 2 blocks for log (ring buffer, erasing one block)
// 2 blocks for config, main and backup
static FILEDESC gFileDescs [] =
{
     // Bootloader
     { ADDR_FLASH_PAGE_0,    CONF_IAP_FL_BOOT_OFFSET, CONF_IAP_FL_BOOT_SZ },

     // StorageFileId_executable
     { ADDR_FLASH_PAGE_0 + (FLASH_PAGE_SIZE * CONF_IAP_FL_APP_OFFSET) ,  CONF_IAP_FL_APP_OFFSET, CONF_IAP_FL_APP_SZ },

     // StorageFileId_update
     { ADDR_FLASH_PAGE_0 + (FLASH_PAGE_SIZE * CONF_IAP_FL_UPDATE_OFFSET),   CONF_IAP_FL_UPDATE_OFFSET, CONF_IAP_FL_UPDATE_SZ  },

     // StorageFileId_log
     { ADDR_FLASH_PAGE_0 + (FLASH_PAGE_SIZE * CONF_IAP_FL_LOG_OFFSET),   CONF_IAP_FL_LOG_OFFSET, CONF_IAP_FL_LOG_SZ  },

     // StorageFileId_var
     { ADDR_FLASH_PAGE_0 + (FLASH_PAGE_SIZE * CONF_IAP_FL_VAR_OFFSET),   CONF_IAP_FL_VAR_OFFSET, CONF_IAP_FL_VAR_SZ  },

     // StorageFileId_config
     { ADDR_FLASH_PAGE_0 + (FLASH_PAGE_SIZE * CONF_IAP_FL_CONFIG_OFFSET),  CONF_IAP_FL_CONFIG_OFFSET, CONF_IAP_FL_CONFIG_SZ  },
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

    AXTRACE("ERASE %u - %u", start, end);

    ENTER(true);

    es.TypeErase   = FLASH_TYPEERASE_PAGES;
    es.PageAddress = ADDR_FLASH_PAGE_0 + (FLASH_PAGE_SIZE * start);
    es.NbPages     = ((end - start) + 1);

    AXTRACE("ERASE stage 1: addr: %X  pages: %u", es.PageAddress, es.NbPages);

    result = (HAL_FLASHEx_Erase(&es, &PAGEError) == HAL_OK);

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
    AXTRACE("IAP PREPARE!!!!");
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
    AXTRACE("IAP UNPREPARE!!!!");
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
    PU32            ons             = (PU32)source_addr;
    uint32_t        ont             = (uint32_t)target_addr;
    int             left;
    int             rd;

    ENTER(true);

    if (!(size & 3))
    {
        result = size;

        for (left = size; result && left; left -= WRITE_SIZE, ons++, ont += WRITE_SIZE)
        {
            if ((rd = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, ont, *ons)) != HAL_OK)
            {
                AXTRACE("HAL_FLASH_Program FAILED: %d", rd);

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

    ENTER(true);

    if (id < TOTAL_FILES)
    {
        props->sectors     = gFileDescs[id].size;

        if (props->sectors)
        {
            props->begin    = gFileDescs[id].begin;
            props->size     = props->sectors * FLASH_PAGE_SIZE;
            props->ptr      = (PVOID)gFileDescs[id].ptr;
            result          = true;
        }
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

    if (idx < TOTAL_BLOCKS)
    {
        result = FLASH_PAGE_SIZE;
    }

    RETURN(result);
}

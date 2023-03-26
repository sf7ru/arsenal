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

#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_flash.h>

#include <stm32f4xx_aip_pvt.h>

#include <axdbg.h>
#include <stm32f4xx_hal_flash_ex.h>

//#undef AXTRACE
//#define AXTRACE     printf

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define TOTAL_SECTORS       (sizeof(gSectors) / sizeof(FLASHBLOCK))
#define TOTAL_FILES         (sizeof(gFiles) / sizeof(FLASHFILEPROPS ))
#define SIZE_0_TO_3         ( 16 * 1024)
#define SIZE_4_TO_4         ( 64 * 1024)
#define SIZE_5_TO_11        (128 * 1024)

#define WRITE_SIZE          sizeof(U32)

#ifdef CONFIG_BOOTLD
#define CONF_IAP_FL_BOOT_SZ     1
//#define CONF_IAP_FL_CONFIG_SZ   1
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
#define CONF_IAP_FL_APP_SZ      (TOTAL_SECTORS -             \
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

// 512 KB
static  FLASHBLOCK          gSectors [] =
{
    { 0x08000000, SIZE_0_TO_3  }, // 0
    { 0x08004000, SIZE_0_TO_3  }, // 1
    { 0x08008000, SIZE_0_TO_3  }, // 2
    { 0x0800C000, SIZE_0_TO_3  }, // 3

    { 0x08010000, SIZE_4_TO_4  }, // 4

    { 0x08020000, SIZE_5_TO_11 }, // 5
    { 0x08040000, SIZE_5_TO_11 }, // 6
    { 0x08060000, SIZE_5_TO_11 }, // 7

#if (BOOTLD_FLASH_SZ_KB == 1024)
    { 0x08080000, SIZE_5_TO_11 }, // 8
    { 0x080A0000, SIZE_5_TO_11 }, // 9
    { 0x080C0000, SIZE_5_TO_11 }, // 10
    { 0x080E0000, SIZE_5_TO_11 }  // 11
#endif
};

#define MAC_FILE(sector, size) { (PVOID)gSectors[sector].ptr, sector, size, 0 }

#ifdef CONFIG_BOOTLD

//
// NOTE
//      We are using first sector after bootloader as config sector, otherwise - last one.
//

static FLASHFILEPROPS   gFiles      [] =
{
    MAC_FILE(0,                        CONF_IAP_FL_BOOT_SZ),

    MAC_FILE(CONF_IAP_FL_BOOT_SZ   +
             CONF_IAP_FL_CONFIG_SZ,    CONF_IAP_FL_APP_SZ),

    MAC_FILE(CONF_IAP_FL_BOOT_SZ   +
             CONF_IAP_FL_CONFIG_SZ +
             CONF_IAP_FL_APP_SZ,       CONF_IAP_FL_UPDATE_SZ),

    MAC_FILE(CONF_IAP_FL_BOOT_SZ   +
             CONF_IAP_FL_CONFIG_SZ +
             CONF_IAP_FL_APP_SZ    +
             CONF_IAP_FL_UPDATE_SZ,    CONF_IAP_FL_LOG_SZ),

    MAC_FILE(CONF_IAP_FL_BOOT_SZ   +
             CONF_IAP_FL_CONFIG_SZ +
             CONF_IAP_FL_APP_SZ    +
             CONF_IAP_FL_UPDATE_SZ +
             CONF_IAP_FL_LOG_SZ,       CONF_IAP_FL_VAR_SZ),

    MAC_FILE(CONF_IAP_FL_BOOT_SZ,      CONF_IAP_FL_CONFIG_SZ)
};

#else // ifdef CONFIG_BOOTLD

static FLASHFILEPROPS   gFiles      [] =
{
    MAC_FILE(0,                        0),

    MAC_FILE(0,                        CONF_IAP_FL_APP_SZ),

    MAC_FILE(CONF_IAP_FL_APP_SZ,       CONF_IAP_FL_UPDATE_SZ),

    MAC_FILE(CONF_IAP_FL_APP_SZ    +
             CONF_IAP_FL_UPDATE_SZ,    CONF_IAP_FL_LOG_SZ),

    MAC_FILE(CONF_IAP_FL_APP_SZ    +
             CONF_IAP_FL_UPDATE_SZ +
             CONF_IAP_FL_LOG_SZ,       CONF_IAP_FL_VAR_SZ),

    MAC_FILE(CONF_IAP_FL_APP_SZ    +
             CONF_IAP_FL_UPDATE_SZ +
             CONF_IAP_FL_LOG_SZ    +
             CONF_IAP_FL_VAR_SZ,      CONF_IAP_FL_CONFIG_SZ)
};

#endif // ifdef CONFIG_BOOTLD


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
//    FLASH_EraseInitTypeDef  es;
//    uint32_t                PAGEError;
//    UINT                    sectors;

    AXTRACE("ERASE %u - %u\n", start, end);

    ENTER(true);


    //AXTRACE("ERASE stage 1: sector: %d  number: %u\n", es.Sector, es.NbSectors);

    for (int i  = start; (i <= end); i++)
    {
        FLASH_Erase_Sector(i, VOLTAGE_RANGE_3);
    }

    result = true;

    //AXTRACE("ERASE stage 2: result = %d \n", result);

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
    __disable_irq();
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
    __enable_irq();

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
            props->size += gSectors[props->begin + idx].size;
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

    if (idx < TOTAL_SECTORS)
    {
        result = gSectors[idx].size;
    }

    RETURN(result);
}

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

#ifndef __BOOTLD_H__
#define __BOOTLD_H__

#include <stdint.h>
#include <custombootld.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define BOOTLD_F1_INFO_OFFSET   0x10c
#define BOOTLD_F4_INFO_OFFSET   0x188
#define BOOTLD_F7_INFO_OFFSET   0x1c8

#define BOOTLD_UPDATE_SIGN      "uPdAtE"

#if defined(STM32F746xx) || defined(STM32F767xx)

#define BOOTLD_LOADER_SIZE      ((uint32_t)(BOOTLD_LOADER_SIZE_KB * 1024))
#define BOOTLD_INFO_OFFSET      ((uint32_t)BOOTLD_LOADER_SIZE + BOOTLD_F7_INFO_OFFSET)
#define BOOTLD_UPDATE_INFO_ADDR ((char*)0x2001E000)

#elif defined(STM32F401xE) || defined(STM32L475xx) || defined(STM32F407xx) || defined(STM32F429xx) || defined(STM32F446xx)

#define BOOTLD_LOADER_SIZE      ((uint32_t)(BOOTLD_LOADER_SIZE_KB * 1024))
#define BOOTLD_INFO_OFFSET      ((uint32_t)BOOTLD_LOADER_SIZE + BOOTLD_F4_INFO_OFFSET)
#define BOOTLD_UPDATE_INFO_ADDR ((char*)0x2001E000)


#elif defined(STM32F103xB)

#define BOOTLD_LOADER_SIZE      ((uint32_t)0x00003800)
#define BOOTLD_INFO_OFFSET      ((uint32_t)BOOTLD_LOADER_SIZE + BOOTLD_F1_INFO_OFFSET)
#define BOOTLD_UPDATE_INFO_ADDR ((char*)0x20001C00)

#elif defined(BOOTLDHOST)

#else

#error ("TARGET CHIP NOT DEFINED")

#endif

#define BOOTLD_FLASH_BEGIN      ((uint32_t)0x08000000)
#define BOOTLD_APP_START        (BOOTLD_FLASH_BEGIN + BOOTLD_LOADER_SIZE)

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct __tag_BOOTLDINFO
{
    uint32_t        size;
    uint32_t        checksum;
    uint32_t        reserved1;
    uint32_t        reserved2;
} BOOTLDINFO, *PBOOTLDINFO;


#endif // __BOOTLD_H__

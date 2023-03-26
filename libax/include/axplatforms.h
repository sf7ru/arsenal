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

#ifndef __AXPLATFORMSH__
#define __AXPLATFORMSH__

// ------------------------------ Platforms ----------------------------------

#define __AX_none__               0

// TARGET_ARCH

#define __AX_i86__              100
#define __AX_i386__             103

#define __AX_arm__              110
#define __AX_lpc__              111
#define __AX_stm32__            112
#define __AX_mdr32__            113
#define __AX_avr__              120
#define __AX_xtensa__           130



// BUILD_TOOL
#define __AX_c__                10
#define __AX_gcc__              11
#define __AX_msvc__             12
#define __AX_keil__             19


// TARGET_FAMILY, TARGET_SYSTEM

#define __AX_standalone__       200  // TARGET_FAMILY, TARGET_SYSTEM ..

#define __AX_win__              210 // TARGET_FAMILY, TARGET_SYSTEM ..
#define __AX_winnt__            211
#define __AX_wince__            212

#define __AX_unix__             220 // TARGET_FAMILY, TARGET_SYSTEM ..
#define __AX_linux__            221
#define __AX_bsd__              222
#define __AX_darwin__           223

#define __AX_rtos__             250 // TARGET_FAMILY, TARGET_SYSTEM ..
#define __AX_freertos__         251



#endif                                      //  #ifndef __AXPLATFORMSH__


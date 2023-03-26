// ***************************************************************************
// TITLE
//      Arsenal 10 Library Main Header
// PROJECT
//      Arsenal/Library
// ***************************************************************************
//
// FILE
//      $Id:$
// HISTORY
//      $Log:$
// ***************************************************************************

#ifndef __ARSENAL10H__
#define __ARSENAL10H__

// ---------------------------------------------------------------------------
// ----------------------------- DEFINITIONS ---------------------------------
// ---------------------------------------------------------------------------

#include <axplatforms.h>
#include <axendian.h>
#include <axmacros.h>

// ----------------------------- Portability ---------------------------------

#include <axportability.h>

#define AXLONG                  long

// ----------------------------- Base types ----------------------------------

typedef char                    I8;
typedef unsigned short          U16;
typedef long long               I64;
typedef unsigned long long      U64;
typedef int                     I32;
typedef unsigned int            U32;
typedef unsigned char           U8;
typedef short                   I16;

typedef unsigned long           ULONG;

#if !defined(true)
#define true                    (BOOL)1
#define false                   (BOOL)0
#endif

#define YES                     true
#define NO                      false

#define ON                      true
#define OFF                     false

#ifndef nil
#define nil                     0
#endif

#define KB                      * 1024UL
#define MB                      KB * 1024UL
#define GB                      MB * 1024ULL
#define TB                      GB * 1024ULL
#define KHZ                     * 1000
#define MHZ                     KHZ * 1000

typedef U8         *            PU8;
typedef const U8   *            PCU8;
typedef U16        *            PU16;
typedef I16        *            PI16;
typedef const U16  *            PCU16;
typedef U32        *            PU32;
typedef I32        *            PI32;
typedef const U32  *            PCU32;
typedef I64        *            PI64;
typedef U64        *            PU64;
typedef const U64  *            PCU64;

#ifndef _WINDOWS_H
typedef char                    CHAR;
typedef int                     INT;
typedef int        *            PINT;
typedef unsigned int            UINT;
typedef unsigned int*           PUINT;
typedef long                    LONG;
typedef long long               LLONG;
typedef long       *            PLONG;
typedef long long  *            PLLONG;
typedef char       *            PSTR;
typedef const char *            PCSTR;
typedef void       *            PVOID;
typedef const void *            PCVOID;
#endif                                      //  #ifndef _WINDOWS_H

#define AXII                    ((UINT)(-1))
#define AXIP                    ((PCVOID)(-1))

typedef PVOID                   HAXHANDLE;
typedef BOOL                    (*PAXDATAUSER)(PVOID,PVOID,UINT);

#define AXTEXT(...)               #__VA_ARGS__


// -------------------------- Log details level ------------------------------

#include <axdbg.h>

#endif                                      //  #ifndef __ARSENAL10H__

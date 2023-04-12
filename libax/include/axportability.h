// ***************************************************************************
// TITLE
//      Arsenal Library Set Portability Header
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: portability.h,v 1.4 2004/03/24 08:28:26 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************

#ifndef __AXPORTABILITYH__
#define __AXPORTABILITYH__


// ---------------------------------------------------------------------------
// ----------------------------- DEFINITIONS ---------------------------------
// ---------------------------------------------------------------------------

#if (TARGET_FAMILY == __AX_win__)
typedef long long               FILESIZEINT;
#define S_ISDIR(a)              (((a) & S_IFDIR) == S_IFDIR)
#if !defined(_WINDEF_H) && !defined(_WINDEF_)
typedef int                      BOOL;
#endif
#   define AXLPATH          260
#   define LA6EXPORTNAME        __declspec(dllexport)
#   define LA6IMPORTNAME        __declspec(dllimport)
#   if (BUILD_TOOL != __AX_gcc__)
#       define LL                   "I64"
#       define snprintf             _snprintf
#       define atoll                _atoi64
#       define strtoull             _strtoi64
#       define vsnprintf            _vsnprintf
#   else
#       define LL                   "ll"
#   endif
#   define LA6POINTER           *
#   define LA6FARPOINTER        LA6POINTER
#if (TARGET_SYSTEM == __AX_wince__)
#       define strcmpi          _stricmp
#endif                                      //  #if (TARGET_SYSTEM...
#elif (TARGET_FAMILY == __AX_unix__)
typedef int                      BOOLEAN;
#include <unistd.h>
#include <limits.h>
typedef int                     BOOL;
typedef long long               FILESIZEINT;
#       define LL                   "ll"
#   ifndef strcmpi
#       define strcmpi          strcasecmp
#   endif
#   include <sys/param.h>
#   ifdef PATH_MAX
#	    define AXLPATH		PATH_MAX
#	else
# 	    ifdef MAXPATHLEN
#  	        define AXLPATH	MAXPATHLEN
#	    else
#           define AXLPATH  1024
#       endif
#   endif
#   define LA6POINTER           *
#   define LA6FARPOINTER        LA6POINTER
#   define LA6EXPORTNAME
#   define LA6IMPORTNAME
#elif (TARGET_FAMILY == __AX_none__)//  #if (JKL_TARGET_FAMILY...
typedef long                    FILESIZEINT;
typedef char                    BOOL;
//#   ifndef snprintf
//#     define snprintf       _snprintf
//#   endif
//#   ifndef vsnprintf
//#     define vsnprintf      _vsnprintf
//#   endif
#   define AXLPATH          80
#   define LA6EXPORTNAME
#   define LA6IMPORTNAME
#   define LA6POINTER           *
#   if (TARGET_SYSTEM == __AX_micro__)
#       define LA6FARPOINTER    LA6POINTER
#   else
#       define LA6FARPOINTER    far LA6POINTER
#   endif                                   //  ifdef (TARGET_SYSTEM ==..
#else
typedef long                    FILESIZEINT;
typedef char                    BOOL;
#   define snprintf             _snprintf
#   define vsnprintf            _vsnprintf
#   define AXLPATH              80
#   define LA6EXPORTNAME
#   define LA6IMPORTNAME
#   define LA6POINTER           *
#   define LA6FARPOINTER        *
#endif                                      //  #if (TARGET_FAMILY...

#if (BUILD_TOOL == __AX_gcc__)
#define AXPACKED(a) a __attribute__((aligned(1),packed))
#elif (BUILD_TOOL == __AX_msvc__)
#define AXPACKED(a) __declspec(align(1)) a
#else
#define AXPACKED(a) a
#endif

#ifndef va_copy
#define axva_copy(a,b)      a = b
#define axva_copy_end
#else
#define axva_copy           va_copy
#define axva_copy_end       va_end
#endif

#ifdef __NO_PRINTF__
#undef  snprintf
#define snprintf __DO_NOT_USE_SNPRINTF__
#undef  vsnprintf
#define vsnprintf __DO_NOT_USE_VSNPRINTF__
#undef  printf
#define printf __DO_NOT_USE_PRINTF__

//extern int __DO_NOT_USE_SNPRINTF__(char *, int, char*,...);
//extern int __DO_NOT_USE_VSNPRINTF__(char *, int, char*, int);
#endif

typedef float               F32;
typedef double              D64;

#if (TARGET_ARCH == __AX_xtensa__)
    #include <sdk.h>
#else // #if (TARGET_ARCH == __AX_xtensa__)
    #if (TARGET_SYSTEM == __AX_freertos__)
        #include <stdint.h>
        #include <stddef.h>

        #ifdef __cplusplus
        extern "C" {
        #endif // __cplusplus
        void *pvPortRealloc(void *mem, size_t newsize);
        void *pvPortCalloc(int count, size_t newsize);
        void *pvPortMalloc(size_t newsize);
        void vPortFree(void *mem);
        #ifdef __cplusplus
        }
        #endif // __cplusplus

        #define LIBMALLOC           pvPortMalloc
        #define LIBFREE             vPortFree
        #define LIBCALLOC           pvPortCalloc
        #define LIBREALLOC          pvPortRealloc
    #else
        #define LIBMALLOC           malloc
        #define LIBFREE             free
        #define LIBCALLOC           calloc
        #define LIBREALLOC          realloc
    #endif
#endif // #if (TARGET_ARCH == __AX_xtensa__)


#endif                                      //  #ifndef __AXPORTABILITYH__

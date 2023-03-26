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

#ifndef __AXENDIANH__
#define __AXENDIANH__

// ---------------------------------------------------------------------------
// ----------------------------- DEFINITIONS ---------------------------------
// ---------------------------------------------------------------------------

#define __AX_BE__               10
#define __AX_LE__               11

#define __AX_INTEL_ENDIAN__     __AX_LE__
#define __AX_MOTO_ENDIAN__      __AX_BE__

#if !defined (TARGET_ENDIAN)
#define TARGET_ENDIAN           __AX_INTEL_ENDIAN__
#endif

#define make_word(a, b)         ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define make_dword(a, b)        ((long)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define low_word(l)             ((WORD)(l))
#define high_word(l)            ((WORD)(((DWORD)(l) >> 16) & 0xffff))
#define MAC_LOWBYTE(w)          ((BYTE)(w))
#define MAC_HIGHBYTE(w)         ((BYTE)(((WORD)(w) >> 8) & 0xff))
#define MAC_LOWCASE(a)          ((a) | 0x20)

#define MAC_SWAP_INT(a,b)       (a ^= (b ^= (a ^= b)))
#define MAC_SWAP(a,b,tmp)       { tmp = a; a = b; b = tmp; }

#define MAC_FLIP_BYTE(x)        (x)
#define MAC_FLIP_U16(x)         (U16) ( (((x) & 0xff00) >> 8) | (((x) & 0xff) << 8) )
#define MAC_FLIP_U32(x)         (U32)( (MAC_FLIP_U16((((x) & 0xffff0000) >> 16))) | \
                                         (MAC_FLIP_U16((x) & 0xffff) << 16) )

#define MAC_FLIP_U64(x)        (U64)( (MAC_FLIP_U32(((x) >> 32) & 0xffffffff)) | \
                                         (((UINT64)MAC_FLIP_U32((x) & 0xffffffff)) << 32) )

#define NATIVE_LE               (TARGET_ENDIAN == __AX_LE__)
#define NATIVE_BE               (TARGET_ENDIAN == __AX_BE__)

#if (NATIVE_LE)
#define MAC_BE_WORD(x)          MAC_FLIP_U16(x)
#define MAC_BE_DWORD(x)         MAC_FLIP_U32(x)
#define MAC_BE_U64(x)           MAC_FLIP_U64(x)
#define MAC_LE_WORD(x)          (x)
#define MAC_LE_DWORD(x)         (x)
#define MAC_LE_U64(x)           (x)

#else
#define MAC_LE_WORD(x)          MAC_FLIP_U16(x)
#define MAC_LE_DWORD(x)         MAC_FLIP_U32(x)
#define MAC_LE_UINT64(x)        MAC_FLIP_U64(x)
#define MAC_BE_WORD(x)          (x)
#define MAC_BE_DWORD(x)         (x)
#define MAC_BE_U64(x)           (x)
#endif                                      //  #if (TARGET_BIGENDIAN == __AX_LE__)

#define MAC_BE_U16              MAC_BE_WORD
#define MAC_BE_U32              MAC_BE_DWORD

#define MAC_LE_U16              MAC_LE_WORD
#define MAC_LE_U32              MAC_LE_DWORD

#define MAC_ALIGN_U32(a)        (((a) & 3) ? ((a) + (4 - ((a) & 3))) : (a))
#define MAC_ALIGN(a,b)          (((a) & ((b) - 1)) ? ((a) + ((b) - ((a) & ((b) - 1)))) : (a))

#endif                                      //  #ifndef __AXENDIANH__

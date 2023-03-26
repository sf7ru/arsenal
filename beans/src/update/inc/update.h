// ***************************************************************************
// TITLE
//      Main header
// PROJECT
//      UrbanLocator
// ***************************************************************************
// FILE
//      $Id$
// ***************************************************************************

#ifndef __ULUPDATEH__
#define __ULUPDATEH__

#include <arsenal.h>
#include <axdata.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define UUPAGE_SZ           (1 KB)
#define UUPAGE_DATA_SZ      (UUPAGE_SZ - sizeof(UUPAGE))

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef enum
{
    ULUPRODUCT_program      = 1,
    ULUPRODUCT_config       = 2,
    ULUPRODUCT_raw_image    = 3,
    ULUPRODUCT_archive      = 4
} ULUPRODUCT;

typedef enum
{
     ULUPLATFORM_host       = 0,
     ULUPLATFORM_ritmt      = 1,
     ULUPLATFORM_owa        = 2,
     ULUPLATFORM_lpc        = 3,
     ULUPLATFORM_stm        = 5,
     ULUPLATFORM_snark      = 201,
     ULUPLATFORM_m11        = 205,

    ULUPLATFORM_mx401      = 241,
    ULUPLATFORM_mx446      = 242,

} ULUPLATFORM;


typedef enum
{
     ULUSTAGE_handshake     = 0,
     ULUSTAGE_transfer      = 1,
     ULUSTAGE_check         = 2,
     ULUSTAGE_applying      = 3
} ULUSTAGE;

typedef enum
{
     ULURESULT_done         = 1,
     ULURESULT_process      = 2,
     ULURESULT_failed       = 3,
     ULURESULT_not_ready    = 4,
     ULURESULT_ready        = 5,
     ULURESULT_mismatch     = 6,
     ULURESULT_uptodate     = 7
} ULURESULT;


// ***************************************************************************
// STRUCTURE
//      ULUFILE
// PURPOSE
//
// ***************************************************************************
#pragma pack(1)
typedef AXPACKED(struct) __tag_ULUFILE
{
    U8              product;
    U8              platform;
    U8              model;
    U8              compression;
    U32             version;
    U32             size;
    U32             checksum;
} ULUFILE, * PULUFILE;
#pragma pack()


// ***************************************************************************
// STRUCTURE
//      UUBLOCK
// PURPOSE
//
// ***************************************************************************
#pragma pack(1)
typedef AXPACKED(struct) __tag_ULUBLOCK
{
    U16             checksum;
    U32             offset;
} ULUBLOCK, * PULUBLOCK;
#pragma pack()

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif                                      //  #ifdef __cplusplus

PAXDATA             ulu_hex_to_update           (PCSTR              fname);

PAXDATA             ulu_bin_to_update           (PCSTR              fname);

BOOL                ulu_fillname_to_header      (PCSTR              fname,
                                                 PULUFILE           head,
                                                 PSTR               nameBuff,
                                                 UINT               size);

BOOL                ulu_update_check            (PULUFILE           head);

#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus

#endif                                      //  #ifndef __ULUPDATEH__

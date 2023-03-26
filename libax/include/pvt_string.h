// ***************************************************************************
// TITLE
//      UNIX Private Header
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: pvt_unix.h,v 1.2 2003/05/09 09:33:01 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************

#ifndef __AXSTRING_PRIVATEH__
#define __AXSTRING_PRIVATEH__

#include <arsenal.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|(9)----------------|(30)---------------------------------------------

#define FOOTER_SZ           2
#define MINSTRING_SZ        (1 + FOOTER_SZ)


// ---------------------------------------------------------------------------
// -------------------------------- TYPES ------------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// STRUCTURE
//      LA6SLUTENTRY
// PURPOSE
//
// ***************************************************************************
#pragma pack(1)
typedef AXPACKED(struct) __tag_AXSLUTENTRY
{
    U32             d_digest;
    U32             d_offset;
} AXSLUTENTRY, * PAXSLUTENTRY;
#pragma pack()

// ***************************************************************************
// STRUCTURE
//      AXSLUT
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_AXSLUT
{
    PSTR            psz_strings;
    UINT            d_digest_entries;
    BOOL            b_case_sensitive;
    PAXSLUTENTRY    pst_digest_tab;
} AXSLUT, * PAXSLUT;

// ***************************************************************************
// STRUCTURE
//      AXSTRING
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_AXSTRING
{
    UINT            u_size;
    UINT            u_used;
    PU8             p_data;
    UINT            u_max;
} AXSTRING, * PAXSTRING;

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif                                      //  #ifdef __cplusplus


int                 _ax_nonblock_dev_read       (int                i_fd,
                                                 PVOID              p_data,
                                                 int                i_size,
                                                 UINT               u_timeout_msec);

int                 _ax_nonblock_dev_write      (int                i_fd,
                                                 PVOID              p_data,
                                                 int                i_size,
                                                 UINT               u_timeout_msec);



#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus


#endif                                      //  #ifndef __AXSTRING_PRIVATEH__

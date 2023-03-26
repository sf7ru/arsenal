// ***************************************************************************
// TITLE
//      Commpn Private Header
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: pvt_unix.h,v 1.2 2003/05/09 09:33:01 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************

#ifndef __AX_PRIVATEH__
#define __AX_PRIVATEH__

#include <stdarg.h>

#include <arsenal.h>



// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

// --------------------------------
#define A7DBGHDR_HDR_BITS    4
#define A7DBGHDR_PLACE_BITS  6
#define A7DBGHDR_MSG_BITS   12
#define A7DBGHDR_TYPE_BITS   2
#define A7DBGHDR_EVT_BITS    8
// --------------------------------
//                          32 bits

#define A7DBGHDR_PLACE_MAX  ((1 << A7DBGHDR_PLACE_BITS) - 1)
#define A7DBGHDR_MSG_MAX    ((1 << A7DBGHDR_MSG_BITS) - 1)

#define A7DBGHDR_SIZE       sizeof(AXDBGMSGHDR)

// ---------------------------------------------------------------------------
// -------------------------------- TYPES ------------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// STRUCTURE
//      AXDBGMSGHDR
// PURPOSE
//
// ***************************************************************************
#pragma pack(1)
typedef AXPACKED(struct) __tag_AXDBGMSGHDR
{
    // Magic

    // Size of header is uint32 words
    U32                     hdrSize     : A7DBGHDR_HDR_BITS;
    // Size of place string in bytes
    U32                     placeSize   : A7DBGHDR_PLACE_BITS;
    // Size of message string in bytes
    U32                     msgSize     : A7DBGHDR_MSG_BITS;
    // DBGET
    U32                     type        : A7DBGHDR_TYPE_BITS;
    // DBGEV
    U32                     evt         : A7DBGHDR_EVT_BITS;

    // Mandatory values

    U32                     usecs;              // Timestamp

    // Optional values

    U32                     secs;               // Time in seconds
    U32                     pid;                // Processor Id
    U32                     tid;                // Thread Id
} AXDBGMSGHDR, * PAXDBGMSGHDR;
#pragma pack()

// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif                                      //  #ifdef __cplusplus


// ---------------------------- nonblocking.c --------------------------------

int                 _ax_nonblock_dev_read       (int                    i_fd,
                                                 PVOID                  p_data,
                                                 int                    i_size,
                                                 UINT                   u_timeout_msec);

int                 _ax_nonblock_dev_write      (int                    i_fd,
                                                 PVOID                  p_data,
                                                 int                    i_size,
                                                 UINT                   u_timeout_msec);


//void            la6dbg_debugger                 (A7LOGDL           v_dl,
//                                                 PSTR               psz_message,
//                                                ...);


#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus


#endif                                      //  #ifndef __AX_PRIVATEH__

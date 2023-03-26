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

#ifndef __AXUNIX_PRIVATEH__
#define __AXUNIX_PRIVATEH__

#include <arsenal.h>



// ---------------------------------------------------------------------------
// -------------------------------- TYPES ------------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// STRUCTURE
//      AXPIPE
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_AXPIPE
{
    int                 i_in;
    int                 i_out;
} AXPIPE, * PAXPIPE;


// ***************************************************************************
// STRUCTURE
//      AXFILE
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_AXFILE
{
    int         i_fd;
} AXFILE, * PAXFILE;

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


#endif                                      //  #ifndef __AXUNIX_PRIVATEH__

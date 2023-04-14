// ***************************************************************************
// TITLE
//      Win32 Private Header
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: pvt_win.h,v 1.2 2003/05/09 09:33:01 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************

#ifndef __AXWIN_PRIVATEH__
#define __AXWIN_PRIVATEH__

#include <arsenal.h>


// ---------------------------------------------------------------------------
// ----------------------------- DEFINITIONS ---------------------------------
// ---------------------------------------------------------------------------

#define MAC_TIME_WIN2AX(a,b)   { a->year          = b->wYear;            \
                                  a->month         = b->wMonth;           \
                                  a->day_of_week   = b->wDayOfWeek;       \
                                  a->day           = b->wDay;             \
                                  a->hour          = b->wHour;            \
                                  a->minute        = b->wMinute;          \
                                  a->second        = b->wSecond;          \
                                  a->milliseconds  = b->wMilliseconds;    }


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
    HANDLE              h_in;
    HANDLE              h_out;
} AXPIPE, * PAXPIPE;

// ***************************************************************************
// STRUCTURE
//      WIN32WTIMER
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_WIN32WTIMER
{
    HANDLE          h_timer;
} WIN32WTIMER, * PWIN32WTIMER;


// ***************************************************************************
// STRUCTURE
//      AXFILE
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_AXFILE
{
    HANDLE          h_file;
} AXFILE, * PAXFILE;

// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif                                      //  #ifdef __cplusplus

PVOID           la6la6_NTDLL_get_proc           (PSTR       psz_procname);

int             _la6_nonblk_file_read           (HANDLE     h_file,
                                                 PVOID      p_data,
                                                 int        i_size,
                                                 UINT       u_timeout_msec);

int             _la6_nonblk_file_write          (HANDLE     h_file,
                                                 PVOID      p_data,
                                                 int        i_size,
                                                 UINT       u_timeout_msec);


#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus


#endif                                      //  #ifndef __AXWIN32_PRIVATEH__

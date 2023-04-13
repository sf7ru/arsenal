// ***************************************************************************
// TITLE
//      File searching for Win32
// PROJECT
//
// ***************************************************************************
//
// FILE
//      $Id$
// HISTORY
//      $Log$
// ***************************************************************************


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#include <windows.h>

#define FINDHANDLE      ((HANDLE)pst_find->p_handle)

#include <arsenal.h>

#include <axfile.h>


#include <pvt_win.h>


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------
/*
// ***************************************************************************
// FUNCTION
//      UINT3FILETIMEToUNIXTime
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      LPFILETIME   ft --
// RESULT
//       --
// ***************************************************************************
static UINT32 FILETIMEToUNIXTime(LPFILETIME ft)
{
    ULONGLONG qw = (((ULONGLONG)ft->dwHighDateTime)<<32) + ft->dwLowDateTime;

    return (UINT32)(qw - 116444736000000000ui64) / 10000000ui64;
}
*/
// ***************************************************************************
// FUNCTION
//      UINT3FILETIMEToUNIXTime
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      LPFILETIME   ft --
// RESULT
//       --
// ***************************************************************************
static UINT32 FILETIMEToUNIXTime(LPFILETIME ft)
{
    UINT32          u_result        = 0;
    struct tm       atm;
    SYSTEMTIME      st;

    ENTER(ft);

    if (FileTimeToSystemTime ( ft, &st ))
    {
        /* Important: mktime looks at the tm_isdst field to determine
         * whether to apply the DST correction. If this field is zero,
         * then no DST is applied. If the field is one, then DST is
         * applied. If the field is minus one, then DST is applied
         * if the United States rule calls for it (DST starts at
         * 02:00 on the first Sunday in April and ends at 02:00 on
         * the last Sunday in October.
         *
         * If you are concerned about time zones that follow different
         * rules, then you must either use GetTimeZoneInformation() to
         * get your system's TIME_ZONE_INFO and use the information
         * therein to figure out whether the time in question was in
         * DST or not, or else use SystemTimeToTzSpecifiedLocalTime()
         * to do the same.
         *
         * I haven't tried playing with SystemTimeToTzSpecifiedLocalTime()
         * so I am nor sure how well it handles funky stuff.
         */
        atm.tm_sec = st.wSecond;
        atm.tm_min = st.wMinute;
        atm.tm_hour = st.wHour;
        atm.tm_mday = st.wDay;
        /* tm_mon is 0 based */
        atm.tm_mon = st.wMonth - 1;
        /* tm_year is 1900 based */
        atm.tm_year = st.wYear>1900?st.wYear - 1900:st.wYear;
        atm.tm_isdst = -1;      /* see notes above */
        u_result = (UINT32)mktime ( &atm );
    }

    RETURN(u_result);
}
// ***************************************************************************
// FUNCTION
//      _copy_data
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PAXFILEFIND        pst_find      --
//      WIN3_FIND_DATA *   pst_find_data --
// RESULT
//      void --
// ***************************************************************************
static void _copy_data(PAXFILEFIND          pst_find,
                       WIN32_FIND_DATA *    pst_find_data)
{
//    SYSTEMTIME  st_systime;

    ENTER(TRUE);

    pst_find->u_size   = (UINT64)pst_find_data->nFileSizeLow |
                        ((UINT64)pst_find_data->nFileSizeHigh << 32);

    strcpy(pst_find->sz_name, pst_find_data->cFileName);

// ----------------------------- Attributes ----------------------------------

    pst_find->u_mode = 0;

    if ((pst_find_data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        pst_find->u_mode |= S_IFDIR;

    if ((pst_find_data->dwFileAttributes & FILE_ATTRIBUTE_READONLY))
        pst_find->u_mode |= S_IREAD;
    else
        pst_find->u_mode |= S_IREAD | S_IWRITE;

// -------------------------------- Times ------------------------------------

    pst_find->v_ctime = FILETIMEToUNIXTime(&pst_find_data->ftCreationTime);
    pst_find->v_atime = FILETIMEToUNIXTime(&pst_find_data->ftLastAccessTime);
    pst_find->v_mtime = FILETIMEToUNIXTime(&pst_find_data->ftLastWriteTime);

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      axfile_find_destroy
// PURPOSE
//
// PARAMETERS
//      PAXFILEFIND   pst_find --
// RESULT
//      PAXFILEFIND  --
// ***************************************************************************
PAXFILEFIND axfile_find_destroy(PAXFILEFIND pst_find)
{
    ENTER(pst_find);

    if ((FINDHANDLE != INVALID_HANDLE_VALUE))
        FindClose(FINDHANDLE);

    FREE(pst_find);

    RETURN(NULL);
}
// ***************************************************************************
// FUNCTION
//      axfile_find_first
// PURPOSE
//
// PARAMETERS
//      PSTR    psz_path  --
//      DWORD   u_modes --
// RESULT
//      PAXFILEFIND  --
// ***************************************************************************
PAXFILEFIND axfile_find_first(PSTR psz_path, UINT u_modes)
{
    PAXFILEFIND         pst_find        = NULL;
    WIN32_FIND_DATA     st_find_data;

    ENTER(psz_path);

    if ((pst_find = CREATE(AXFILEFIND)) != NULL)
    {
        memset(pst_find, 0, sizeof(AXFILEFIND));

        if ((pst_find->p_handle = (PVOID)FindFirstFile(psz_path,
                           &st_find_data)) != INVALID_HANDLE_VALUE)
        {
            _copy_data(pst_find, &st_find_data);
        }
        else
            pst_find = axfile_find_destroy(pst_find);
    }

    RETURN(pst_find);
}
// ***************************************************************************
// FUNCTION
//      axfile_find_next
// PURPOSE
//      Find next files
// PARAMETERS
//      PAXFILEFIND pst_find -- Pointer to Find control structure
// RESULT
//      BOOL  -- TRUE if file was found FALSE if not
// ***************************************************************************
BOOL axfile_find_next(PAXFILEFIND pst_find)
{
    BOOL                b_result        = FALSE;
    WIN32_FIND_DATA     st_find_data;

    ENTER(pst_find);

    if (FindNextFile(FINDHANDLE, &st_find_data))
    {
        _copy_data(pst_find, &st_find_data);

        b_result = TRUE;
    }

    RETURN(b_result);
}

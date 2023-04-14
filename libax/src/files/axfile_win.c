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

#include <stdlib.h>
#include <fcntl.h>

#include <windows.h>

#include <arsenal.h>
#include <axfile.h>
#include <pvt_win.h>


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      axfile_close
// PURPOSE
//
// PARAMETERS
//      HAXFILE   h_file --
// RESULT
//      HAXFILE  --
// ***************************************************************************
HAXFILE axfile_close(HAXFILE h_file)
{
    PAXFILE         pst_file    = (PAXFILE)h_file;

    ENTER(pst_file);

    if (pst_file->h_file != INVALID_HANDLE_VALUE)
        CloseHandle(pst_file->h_file);

    FREE(pst_file);

    RETURN(NULL);
}
// ***************************************************************************
// FUNCTION
//      axfile_open
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_fname --
//      PSTR   psz_mode  --
// RESULT
//      HAXFILE  --
// ***************************************************************************
HAXFILE axfile_open(PCSTR   psz_fname,
                    UINT    u_mode)
{
    PAXFILE         pst_file    = NULL;
    DWORD           d_access    = GENERIC_READ;
    DWORD           d_mode      = OPEN_EXISTING;

    ENTER(psz_fname);

    if ((pst_file = CREATE(AXFILE)) != NULL)
    {
        pst_file->h_file = INVALID_HANDLE_VALUE;

        if (u_mode & O_RDWR)
        {
            d_access    |= (GENERIC_READ | GENERIC_WRITE);
            d_mode       = OPEN_ALWAYS;
        }

        if (u_mode & O_CREAT)
        {
            d_mode       = CREATE_ALWAYS;
        }

        if ((pst_file->h_file = CreateFile(psz_fname, d_access, 0, NULL, d_mode,
                FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
        {
            // AXTRACE("  NOT OPEN: %s (mode %d)\n", psz_fname, u_mode);
            pst_file = (PAXFILE)axfile_close((HAXFILE)pst_file);
        }
    }

    RETURN((HAXFILE)pst_file);
}
// ***************************************************************************
// FUNCTION
//      axfile_write
// PURPOSE
//
// PARAMETERS
//      HAXFILE   h_file   --
//      PVOID     p_buffer --
//      INT       i_size   --
// RESULT
//      INT  --
// ***************************************************************************
INT axfile_write(HAXFILE        h_file,
                 PVOID          p_buffer,
                 INT            i_size)
{
    INT             i_result        = -1;
    PAXFILE         pst_file        = (PAXFILE)h_file;
    DWORD           d_written;

    ENTER(pst_file && p_buffer && i_size);

    if (WriteFile(pst_file->h_file, p_buffer, i_size, &d_written, NULL))
    {
        i_result = (int)d_written;
    }

    RETURN(i_result);
}
// ***************************************************************************
// FUNCTION
//      axfile_read
// PURPOSE
//
// PARAMETERS
//      HAXFILE   h_file   --
//      PVOID     p_buffer --
//      INT       i_size   --
// RESULT
//      INT  --
// ***************************************************************************
INT axfile_read(HAXFILE         h_file,
                PVOID           p_buffer,
                INT             i_size)
{
    INT             i_result        = -1;
    PAXFILE         pst_file        = (PAXFILE)h_file;
    DWORD           d_read;

    ENTER(pst_file && p_buffer && i_size);


    if (ReadFile(pst_file->h_file, p_buffer, i_size, &d_read, NULL))
    {
        i_result = (int)d_read;
    }

    RETURN(i_result);
}
// ***************************************************************************
// FUNCTION
//      axfile_move
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_old     --
//      PSTR   psz_new     --
//      BOOL   b_overwrite --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL axfile_move(PCSTR      psz_new,
                 PCSTR      psz_old,
                 BOOL       b_overwrite)
{
    BOOL b_result   = FALSE;

    ENTER(psz_old && psz_new);

    b_result = MoveFileEx(psz_old, psz_new,
               MOVEFILE_COPY_ALLOWED | (b_overwrite ? MOVEFILE_REPLACE_EXISTING : 0));

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axfile_remove
// PURPOSE
//
// PARAMETERS
//      PCSTR psz_filename --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL axfile_remove(PCSTR       psz_filename)
{
    BOOL            b_result        = FALSE;

    ENTER(TRUE);

    b_result = DeleteFile(psz_filename);

    RETURN(b_result);
}

// ***************************************************************************
// FUNCTION
//      axfile_copy
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_old     --
//      PSTR   psz_new     --
//      BOOL   b_overwrite --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL axfile_copy(PCSTR      psz_new,
                 PCSTR      psz_old,
                 BOOL       b_overwrite)
{
    BOOL b_result   = FALSE;

    ENTER(psz_old && psz_new);

    b_result = CopyFile(psz_old, psz_new, !b_overwrite);

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axfile_seek
// PURPOSE
//
// PARAMETERS
//      HAXFILE       h_file   --
//      FILESIZEINT   i_offs   --
//      UINT          u_whence --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL axfile_seek(HAXFILE            h_file,
                 FILESIZEINT        i_offs,
                 UINT               u_whence)
{
    PAXFILE         pst_file        = (PAXFILE)h_file;
    BOOL            b_result        = FALSE;
    DWORD           d_direct        = FILE_BEGIN;
    LARGE_INTEGER   st_li;

    ENTER(pst_file);

    switch (u_whence)
    {
        case SEEK_END:
            d_direct = FILE_END;
            break;

        case SEEK_CUR:
            d_direct = FILE_CURRENT;
            break;
    }

    st_li.QuadPart  = i_offs;
    b_result        = SetFilePointerEx(pst_file->h_file, st_li, NULL, d_direct);

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axfile_expand
// PURPOSE
//
// PARAMETERS
//      HAXFILE       h_file --
//      FILESIZEINT   i_size --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL axfile_size_set(HAXFILE          h_file,
                     FILESIZEINT      i_size)
{
    BOOL            b_result        = FALSE;
    PAXFILE         pst_file        = (PAXFILE)h_file;

    ENTER(pst_file);

    if (axfile_seek(h_file, i_size, SEEK_SET))
    {
        b_result = SetEndOfFile(pst_file->h_file);
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axfile_size
// PURPOSE
//
// PARAMETERS
//      HAXFILE   h_file --
// RESULT
//      FILESIZEINT  --
// ***************************************************************************
FILESIZEINT axfile_size_get(HAXFILE     h_file)
{
    FILESIZEINT     i_size          = 0;
    PAXFILE         pst_file        = (PAXFILE)h_file;
    LARGE_INTEGER   st_li;

    ENTER(pst_file);


#ifdef __MINGW32__
    DWORD hi = 0;
    i_size = GetFileSize(pst_file->h_file, &hi);

    i_size = (hi << 32) | i_size;

#else
    if (GetFileSizeEx(pst_file->h_file, &st_li))
    {
        i_size = (FILESIZEINT)st_li.QuadPart;
    }
#endif

    RETURN(i_size);
}
// ***************************************************************************
// FUNCTION
//      axfile_statfs
// PURPOSE
//
// PARAMETERS
//      PAXSTATFS   pst_stat --
//      PSTR        psz_path --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL axfile_statfs(PAXSTATFS    pst_stat,
                   PSTR         psz_path)
{
    BOOL            b_result                    = FALSE;
    ULARGE_INTEGER  FreeBytesAvailable;
    ULARGE_INTEGER  TotalNumberOfBytes;
    ULARGE_INTEGER  TotalNumberOfFreeBytes;

    ENTER(pst_stat && psz_path);

    if (GetDiskFreeSpaceEx(psz_path,
                           &FreeBytesAvailable,
                           &TotalNumberOfBytes,
                           &TotalNumberOfFreeBytes))
    {
        pst_stat->u_type    = 0;
        pst_stat->u_total   = (FILESIZEINT)TotalNumberOfBytes.QuadPart;
        pst_stat->u_free    = (FILESIZEINT)TotalNumberOfFreeBytes.QuadPart;
        pst_stat->u_avail   = (FILESIZEINT)FreeBytesAvailable.QuadPart;
        b_result            = TRUE;
    }

    RETURN(b_result);
}


// ***************************************************************************
// FUNCTION
//      la6_file_times_get
// PURPOSE
//      Retrieve file creation, access and modification time
// PARAMETERS
//      PAXDATE pst_c_time   -- Pointer to structure for creation time
//      PAXDATE pst_a_time   -- Pointer to structure for access time
//      PAXDATE pst_m_time   -- Pointer to structure for modification time
//      PSTR     psz_filename -- Pointer to path and file name
// RESULT
//      BOOL  -- TRUE if all is ok or FALSE if error has occured
// ***************************************************************************
BOOL axfile_times_get(PAXDATE pst_c_time, PAXDATE pst_a_time, PAXDATE pst_m_time, PSTR psz_filename)
{
    BOOL        b_result        = false;
    HANDLE      h_file;
    FILETIME    st_c_time;
    FILETIME    st_a_time;
    FILETIME    st_m_time;
    SYSTEMTIME  st_sys_c_time;
    SYSTEMTIME  st_sys_a_time;
    SYSTEMTIME  st_sys_m_time;
    SYSTEMTIME  st_tcsys_c_time;
    SYSTEMTIME  st_tcsys_a_time;
    SYSTEMTIME  st_tcsys_m_time;

    if ((pst_c_time || pst_a_time || pst_m_time) && psz_filename)
    {
        if ((h_file = CreateFile(psz_filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) != INVALID_HANDLE_VALUE)
        {
            if (GetFileTime(h_file, &st_c_time, &st_a_time, &st_m_time))
            {
                FileTimeToSystemTime(&st_c_time, &st_sys_c_time);
                FileTimeToSystemTime(&st_a_time, &st_sys_a_time);
                FileTimeToSystemTime(&st_m_time, &st_sys_m_time);

                SystemTimeToTzSpecificLocalTime(NULL, &st_sys_c_time, &st_tcsys_c_time);
                SystemTimeToTzSpecificLocalTime(NULL, &st_sys_a_time, &st_tcsys_a_time);
                SystemTimeToTzSpecificLocalTime(NULL, &st_sys_m_time, &st_tcsys_m_time);

                if (pst_c_time)
                    MAC_TIME_WIN2AX(pst_c_time, (&st_tcsys_c_time));

                if (pst_a_time)
                    MAC_TIME_WIN2AX(pst_a_time, (&st_tcsys_a_time));

                if (pst_m_time)
                    MAC_TIME_WIN2AX(pst_m_time, (&st_tcsys_m_time));

                b_result = true;
            }

            CloseHandle(h_file);
        }
    }

    return b_result;
}

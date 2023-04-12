// ***************************************************************************
// TITLE
//      Win32 specific routines
// PROJECT
//      Arsenal Library
// ***************************************************************************
//
// FILE
//      $Id: specials.c,v 1.2 2003/08/06 12:25:58 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// HISTORY
//      $Log: specials.c,v $
//      Revision 1.2  2003/08/06 12:25:58  A.Kozhukhar
//      *** empty log message ***
//
//      Revision 1.1.1.1  2003/02/14 13:17:32  A.Kozhukhar
//      Initial release
//
// ***************************************************************************

#include <windows.h>

#include <arsenal.h>
#include <axsystem.h>



// ---------------------------------------------------------------------------
// -------------------------- LOCAL DEFINITIONS ------------------------------
// ---------------------------------------------------------------------------

static HINSTANCE g_h_NTDLL          = NULL;


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      la6la6_NTDLL_get_proc
// PURPOSE
//      Retrieve entry point of function from NTDLL.DLL
// PARAMETERS
//      PSTR  psz_procname -- Pointer to name of function
// RESULT
//      PVOID  -- Entry point of function if exist or NULL if not
// ***************************************************************************
PVOID la6la6_NTDLL_get_proc(PSTR psz_procname)
{
    PVOID   pfn_ret     = NULL;

    if (  g_h_NTDLL                                               ||
          ((g_h_NTDLL = LoadLibrary(TEXT("NTDLL.DLL"))) != NULL)  )
        pfn_ret = GetProcAddress(g_h_NTDLL, psz_procname);

    return pfn_ret;
}
// ***************************************************************************
// FUNCTION
//      _la6_nonblk_file_read
// PURPOSE
//
// PARAMETERS
//      HANDLE h_file         --
//      PVOID  p_data         --
//      int    i_size         --
//      UINT   u_timeout_msec --
// RESULT
//      int --
// ***************************************************************************
int _la6_nonblk_file_read(HANDLE    h_file,
                          PVOID     p_data,
                          int       i_size,
                          UINT      u_timeout_msec)
{
    INT                 i_read          = -1;
    UINT                u_left;
    PBYTE               p_on;
    DWORD               d_read;
//    DWORD               d_timeout;
    OVERLAPPED          st_o;

    ENTER((h_file != INVALID_HANDLE_VALUE) && p_data && i_size);

    memset(&st_o, 0, sizeof(OVERLAPPED));

    if ((st_o.hEvent = CreateEvent(
                NULL, TRUE, FALSE, NULL)) != INVALID_HANDLE_VALUE)
    {
        i_read  = 0;
        u_left  = i_size;
        p_on    = p_data;

        while (u_left)
        {
            if (!ReadFile(h_file, p_on, 1, &d_read, &st_o))
//            if (!ReadFile(h_file, p_on, u_left, &d_read, &st_o))
            {
                if (GetLastError() == ERROR_IO_PENDING)
                {
                    if (WaitForSingleObject(st_o.hEvent,
                                    u_timeout_msec) == WAIT_OBJECT_0)
                    {
                        if (!GetOverlappedResult(h_file,
                                        &st_o, &d_read, FALSE))
                        {
                            i_read = -1;
                            break;
                        }
                    }
                    else
                    {
                        CancelIo(h_file);
                        break;
                    }
                }
                else
                {
                    i_read = -1;
                    break;
                }
            }

            i_read  += d_read;
            u_left  -= d_read;
            p_on    += d_read;
        }

        CloseHandle(st_o.hEvent);
    }

    RETURN(i_read);
}
// ***************************************************************************
// FUNCTION
//      _la6_nonblk_file_write
// PURPOSE
//
// PARAMETERS
//      HANDLE h_file         --
//      PVOID  p_data         --
//      int    i_size         --
//      UINT   u_timeout_msec --
// RESULT
//      int --
// ***************************************************************************
int _la6_nonblk_file_write(HANDLE   h_file,
                           PVOID    p_data,
                           int      i_size,
                           UINT     u_timeout_msec)
{
    INT                 i_written       = -1;
    PBYTE               p_on;
    OVERLAPPED          st_o;
    UINT                u_left;
    DWORD               d_written;
    DWORD               d_Last_Error                = 0;

    ENTER((h_file != INVALID_HANDLE_VALUE) && p_data && i_size);

    memset(&st_o, 0, sizeof(OVERLAPPED));

    if ((st_o.hEvent = CreateEvent(NULL,TRUE,
                            FALSE, NULL)) != INVALID_HANDLE_VALUE)
    {
        i_written   = 0;
        u_left      = i_size;
        p_on        = p_data;

        while (u_left)
        {
            if (!WriteFile(h_file, p_on, u_left, &d_written, &st_o))
            {
                if (GetLastError() == ERROR_IO_PENDING)
                {
                    if (WaitForSingleObject(st_o.hEvent,
                                    u_timeout_msec) == WAIT_OBJECT_0)
                    {
                        if (!GetOverlappedResult(h_file,
                                        &st_o, &d_written, FALSE))
                        {
                            i_written = -1;
                            break;
                        }
                    }
                    else
                    {
                        CancelIo(h_file);
                        break;
                    }
                }
                else
                {
                    i_written = -1;
                    break;
                }
            }

            i_written   += d_written;
            u_left      -= d_written;
            p_on        += d_written;
        }

        CloseHandle(st_o.hEvent);
    }

    RETURN(i_written);
}

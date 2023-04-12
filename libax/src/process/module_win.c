// ***************************************************************************
// TITLE
//      Executable Module Win32  Platform
// PROJECT
//      Arsenal Library
// ***************************************************************************
//
// FILE
//      $Id: module.c,v 1.1.1.1 2003/02/14 13:17:33 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// HISTORY
//      $Log: module.c,v $
//      Revision 1.1.1.1  2003/02/14 13:17:33  A.Kozhukhar
//      Initial release
//
// ***************************************************************************


#include <windows.h>

#include <arsenal.h>

#include <axsystem.h>


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------


// ***************************************************************************
// FUNCTION
//      la6_module_get_filename
// PURPOSE
//      Retrieve file name of current module
// PARAMETERS
//      PSTR  psz_buffer -- Pointer to target string
//      DWORD d_size     -- Size of target string
// RESULT
//      DWORD   -- Number of retrieved symbols
// ***************************************************************************
INT axmodule_get_filename(PSTR psz_buffer, INT d_size)
{
    INT   d_ret_size = 0;

    if (psz_buffer && d_size)
        d_ret_size = GetModuleFileName(NULL, psz_buffer, d_size);

    return d_ret_size;
}

// ***************************************************************************
// FUNCTION
//      la6_module_get_path
// PURPOSE
//      Retrieve file path of current module
// PARAMETERS
//      PSTR  psz_buffer -- Pointer to target string
//      DWORD d_size     -- Size of target string
// RESULT
//      DWORD   -- Number of retrieved symbols
// ***************************************************************************
INT axmodule_get_path(PSTR psz_buffer, INT d_size)
{
    DWORD   d_ret_size = 0;
    PSTR    psz_tmp;

    if ((d_ret_size = axmodule_get_filename(psz_buffer, d_size)) != 0)
    {
        if ( ((psz_tmp = strrchr(psz_buffer, '\\')) !=  NULL)   ||
             ((psz_tmp = strrchr(psz_buffer, '/'))  !=  NULL)   )
        {
            *psz_tmp = 0;
            d_ret_size = strlen(psz_buffer);
        }
        else
            d_ret_size = 0;
    }

    return d_ret_size;
}

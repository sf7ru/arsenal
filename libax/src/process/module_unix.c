// ***************************************************************************
// TITLE
//      Executable Module UNIX Platform
// PROJECT
//      Arsenal Library
// ***************************************************************************
//
// FILE
//      $Id: module.c,v 1.3 2003/09/17 11:53:11 A.Urakhchinskiy Exp $
// AUTHOR
//      $Author: A.Urakhchinskiy $
// HISTORY
//      $Log: module.c,v $
//      Revision 1.3  2003/09/17 11:53:11  A.Urakhchinskiy
//      no message
//
//      Revision 1.2  2003/09/08 15:58:21  A.Kozhukhar
//      *** empty log message ***
//
//      Revision 1.1  2003/02/28 10:35:20  A.Kozhukhar
//      Initial release
//
// ***************************************************************************


#include <unistd.h>

#include <string.h>

#include <arsenal.h>
#include <axsystem.h>
#include <axfile.h>


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      axmodule_get_filename
// PURPOSE
//      Retrieve file name of current module
// PARAMETERS
//      PSTR  psz_buffer -- Pointer to target string
//      INT d_size     -- Size of target string
// RESULT
//      INT   -- Number of retrieved symbols
// ***************************************************************************
INT axmodule_get_filename(PSTR      psz_buffer,
                          INT       size)
{
    INT       d_ret_size  = 0;
    ssize_t     i_read;

    if ((i_read = readlink("/proc/self/exe", psz_buffer, (int)size)) != -1)
    {
        *(psz_buffer + i_read)  = 0;
        d_ret_size              = i_read;
    }

    return d_ret_size;
}
// ***************************************************************************
// FUNCTION
//      axmodule_get_path
// PURPOSE
//      Retrieve file path of current module
// PARAMETERS
//      PSTR  psz_buffer -- Pointer to target string
//      INT d_size     -- Size of target string
// RESULT
//      INT   -- Number of retrieved symbols
// ***************************************************************************
INT axmodule_get_path(PSTR  psz_buffer,
                      INT   size)
{
    INT   d_ret_size = 0;

    if (axmodule_get_filename(psz_buffer, size))
    {
        fname_extract_path(psz_buffer);

        size = strlen(psz_buffer);
    }

    return d_ret_size;
}

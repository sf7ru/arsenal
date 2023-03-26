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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arsenal.h>
#include <axstring.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      fname_change_ext
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_filename --
//      INT    i_len        --
//      PSTR   psz_ext      --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL fname_change_ext(PSTR      psz_filename,
                      INT       i_len,
                      PSTR      psz_ext)
{
    BOOL        b_result    = false;
    INT         i_size;
    PSTR        psz_end;

    ENTER(psz_filename && i_len && psz_ext);

    if ((psz_end = strrchr(psz_filename, '.')) == NULL)
    {
        psz_end = psz_filename + strlen(psz_filename);
    }

    i_size = i_len - (psz_end - psz_filename);

    if (i_size > 0)
    {
        strz_cpy(psz_end, psz_ext, i_size);
        b_result = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      fname_slashes
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_path  --
//      BOOL   b_forward --
// RESULT
//      void  --
// ***************************************************************************
void fname_slashes(PSTR     psz_path,
                   BOOL     b_forward)
{
    PSTR        psz_on;

    ENTER(psz_path);

    if (b_forward)
    {
        for (psz_on = psz_path; *psz_on; psz_on++)
        {
            if (*psz_on == '\\')
            {
                *psz_on = '/';
            }
        }
    }
    else
    {
        for (psz_on = psz_path; *psz_on; psz_on++)
        {
            if (*psz_on == '/')
            {
                *psz_on = '\\';
            }
        }
    }

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      fname_change_name
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_filename --
//      INT    i_len        --
//      PSTR   psz_fname    --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL fname_change_name(PSTR    psz_filename,
                        INT     i_len,
                        PSTR    psz_fname)
{
    BOOL        b_result    = false;
    INT         i_size;
    PSTR        psz_end;

    ENTER(psz_filename && i_len && psz_fname);

    if (  ((psz_end = strrchr(psz_filename, '/'))  == NULL)     &&
          ((psz_end = strrchr(psz_filename, '\\')) == NULL)     )
    {
        psz_end = psz_filename + strlen(psz_filename);
    }
    else
        psz_end++;

    i_size = i_len - (psz_end - psz_filename);

    if (i_size > 0)
    {
        strz_cpy(psz_end, psz_fname, i_size);
        b_result = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      fname_get_path
// PURPOSE
//
// PARAMETERS
//      PSTR    psz_path  --
//      UINT    u_len     --
//      PCSTR   psz_fname --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL fname_get_path(PSTR    psz_path,
                    UINT    u_len,
                    PCSTR   psz_fname)
{
    BOOL        b_result    = false;
    UINT        u_size;
    PSTR        psz_on;

    ENTER(psz_path);

    if (  ((psz_on = strrchr(psz_fname, '/')) != NULL)   ||
          ((psz_on = strrchr(psz_fname, '\\')) != NULL)  )
    {
        u_size = psz_on - psz_fname;

        if ((u_size + sizeof(CHAR)) < u_len)
        {
            memcpy(psz_path, psz_fname, u_size);

            *(psz_path + u_size)    = 0;
            b_result                = true;
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      fname_get_name
// PURPOSE
//
// PARAMETERS
//      PSTR  psz_name  --
//      UINT  u_len     --
//      PCSTR psz_fname --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL fname_get_name(PSTR    psz_name,
                    UINT    u_len,
                    PCSTR   psz_fname)
{
    BOOL        b_result    = false;
    PSTR        psz_on;

    ENTER(psz_name);

    if (  ((psz_on = strrchr(psz_fname, '/')) != NULL)   ||
          ((psz_on = strrchr(psz_fname, '\\')) != NULL)  )
    {
        psz_on += 1;
    }
    else
        psz_on = (PSTR)psz_fname;

    strz_cpy(psz_name, psz_on, u_len);

    b_result = true;

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      fname_get_ext
// PURPOSE
//
// PARAMETERS
//      PSTR  psz_ext   --
//      UINT  u_len     --
//      PCSTR psz_fname --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL fname_get_ext(PSTR    psz_ext,
                   UINT    u_len,
                   PCSTR   psz_fname)
{
    BOOL        b_result    = false;
    PSTR        psz_on;

    ENTER(psz_ext);

    if ((psz_on = strrchr(psz_fname, '.')) != NULL)
    {
        strz_cpy(psz_ext, psz_on + 1, u_len);

        b_result = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      fname_extract_path
// PURPOSE
//
// PARAMETERS
//      PSTR psz_path --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL fname_extract_path(PSTR psz_fname)
{
    BOOL        b_result    = false;
    PSTR        psz_on;

    ENTER(psz_fname);

    if (  ((psz_on = strrchr(psz_fname, '/')) != NULL)   ||
          ((psz_on = strrchr(psz_fname, '\\')) != NULL)  )
    {
        *psz_on     = 0;
        b_result    = true;
    }

    RETURN(b_result);
}

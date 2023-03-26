// ***************************************************************************
// TITLE
//      File Access Utilities
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: files_common.c,v 1.2 2003/09/10 06:55:35 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <arsenal.h>

#include <sys/stat.h>

#if (TARGET_FAMILY == __AX_unix__)
#define MAC_MKDIR(a,b)  mkdir(a, 0755)
#  include <unistd.h>
#else
#define MAC_MKDIR(a,b)  mkdir(a)
#  include <direct.h>
#endif

#include <axfile.h>
#include <axstring.h>

// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      axpath_create
// PURPOSE
//
// PARAMETERS
//      PSTR psz_path --
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
BOOL axpath_create(PSTR psz_path)
{
    BOOL            b_result        = false;
    PSTR            psz_copy;
    PSTR            psz_on;
    PSTR            psz_slash;

    ENTER(psz_path);

    if ((psz_copy = strz_dup(psz_path)) != NULL)
    {
        psz_on = psz_copy;

#if (TARGET_FAMILY == __AX_win__)

        for (   psz_slash = psz_copy;
                ((psz_slash = strchr(psz_slash, '\\')) != NULL);
                psz_slash++)
        {
            *psz_slash = '/';
        }

        if ((psz_slash = strchr(psz_copy, ':')) != NULL)
        {
            psz_on = ++psz_slash;
        }
#endif

        if (*psz_on == '/')
            psz_on++;

        do
        {
            if ((psz_slash = strchr(psz_on, '/'))  != NULL)
                *psz_slash = 0;

            b_result = !MAC_MKDIR(psz_copy, -1) ||
                       (errno == EEXIST);

            if (psz_slash)
            {
                *psz_slash = '/';
                psz_on  = ++psz_slash;
            }

        } while (b_result && psz_slash);

        FREE(psz_copy);
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axpath_create_to_file
// PURPOSE
//      Create path to file
// PARAMETERS
//      PSTR psz_path_and_filename -- Pointer to path and file name
// RESULT
//      BOOL  -- true if all is ok or false if error has occured
// ***************************************************************************
BOOL axpath_create_to_file(PSTR psz_path_and_filename)
{
    BOOL    b_result    = false;
    UINT    d_size;
    PSTR    psz_slash;
    PSTR    psz_path;

    if (psz_path_and_filename && *psz_path_and_filename)
    {
        if ((psz_path = MALLOC(d_size =
                strlen(psz_path_and_filename) + sizeof(CHAR))) != NULL)
        {
            memcpy(psz_path, psz_path_and_filename, d_size);

            if( ((psz_slash = strrchr(psz_path, '/'))  != NULL) ||
                ((psz_slash = strrchr(psz_path, '\\')) != NULL) )
            {
                *psz_slash = 0;
                b_result   = axpath_create(psz_path);
            }
            else
                b_result = true;

            FREE(psz_path);
        }
    }

    return b_result;
}
// ***************************************************************************
// FUNCTION
//      a7file_exist
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_filename --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL axfile_exist(PCSTR psz_filename)
{
    BOOL            b_result    = false;
    struct stat     st_st;

    ENTER(psz_filename);

    if (!stat(psz_filename, &st_st))
    {
        b_result = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      a7file_is_dir
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_path --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL axfile_is_dir(PSTR psz_path)
{
    BOOL            b_result    = false;
    struct stat     st_st;

    ENTER(psz_path);

    if (!stat(psz_path, &st_st))
    {
        b_result = S_ISDIR(st_st.st_mode);
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      a7file_size_get2
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_fname --
// RESULT
//      FILESIZEINT  --
// ***************************************************************************
FILESIZEINT axfile_get_size(PCSTR       psz_fname)
{
    FILESIZEINT     u_result        = 0;
    struct stat     st_st;

    ENTER(psz_fname);

    if (!stat(psz_fname, &st_st))
    {
        u_result = st_st.st_size;
    }

    RETURN(u_result);
}
// ***************************************************************************
// FUNCTION
//      axfile_load
// PURPOSE
//
// PARAMETERS
//      PSTR  p_filename        --
//      PUINT pd_size           --
//      UINT  d_additional_size --
// RESULT
//      PVOID --
// ***************************************************************************
PVOID axfile_load(PCSTR        p_filename,
                  PUINT        pd_size,
                  UINT d_additional_size)
{
    FILE    *p_in_file;
    UINT    d_size;
    PU8     p_buff = NULL;

    if (p_filename && pd_size)
    {
        if ((p_in_file = fopen(p_filename, "rb")) != NULL)
        {
            fseek(p_in_file, 0, SEEK_END);
            d_size = ftell(p_in_file);
            fseek(p_in_file, 0, SEEK_SET);

            if ( d_size && ((p_buff = MALLOC_D(d_size + d_additional_size, "Loaded File")) != NULL) )
            {
                if (fread(p_buff, d_size, 1, p_in_file))
                    *pd_size = d_size;
                else
                {
                    FREE(p_buff);
                    p_buff = NULL;
                }
            }

            fclose(p_in_file);
        }
    }

    return p_buff;
}
BOOL axfile_save_data(PCSTR              p_filename,
                      PAXDATA            data)
{
    BOOL        result          = false;
    FILE *      fout;

    ENTER(true);

    if ((fout = fopen(p_filename, "w+b")) != nil)
    {
        result = (fwrite(data->ab_data, data->u_size, 1, fout) == 1);

        fclose(fout);
    }

    RETURN(result);
}
BOOL axfile_save(PCSTR              p_filename,
                 PCVOID             data,
                 UINT               size)
{
    BOOL        result          = false;
    FILE *      fout;

    ENTER(true);

    if ((fout = fopen(p_filename, "w+b")) != nil)
    {
        result = (fwrite(data, size, 1, fout) == 1);

        fclose(fout);
    }

    RETURN(result);
}

// ***************************************************************************
// TITLE
//      Files Find for UNIX
// PROJECT
//      Arsenal Library
// ***************************************************************************
//
// FILE
//      $Id: find.c,v 1.1 2003/02/28 10:33:12 A.Kozhukhar Exp $
// HISTORY
//      $Log$
// ***************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

#include <arsenal.h>
#include <axfile.h>
#include <axstring.h>


#include <dirent.h>
#include <sys/stat.h>


// ---------------------------------------------------------------------------
// -------------------------------- TYPES ------------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// STRUCTURE
//      TRIPLE
// PURPOSE
//      Triple-byte word
// ***************************************************************************
typedef struct __tag_FINDRES
{
    DIR *       pst_dir;
    CHAR        sz_dir      [ AXLPATH ];
    CHAR        sz_mask     [ AXLPATH ];
} FINDRES, * PFINDRES;


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      _cvt_dir
// PURPOSE
//
// PARAMETERS
//      PAXFILEFIND          pst_find --
//      struct dirent   * pst_dir  --
// RESULT
//      none
// ***************************************************************************
static void _cvt_dir(PAXFILEFIND        pst_find,
                     struct dirent *    pst_dir)
{
    PFINDRES            pst_res         = (PFINDRES)pst_find->p_handle;
    struct stat         st_stat;
    CHAR                sz_filename     [ AXLPATH ];

    ENTER(true);

    strz_cpy(pst_find->sz_name, pst_dir->d_name, AXLPATH);

    snprintf(sz_filename, AXLPATH, "%s/%s", pst_res->sz_dir, pst_find->sz_name);

    if (!stat(sz_filename, &st_stat))
    {
        pst_find->v_ctime   = st_stat.st_ctime;
        pst_find->v_atime   = st_stat.st_atime;
        pst_find->v_mtime   = st_stat.st_mtime;
        pst_find->u_size    = st_stat.st_size;
        pst_find->u_mode    = st_stat.st_mode;
    }
    else
    {
        pst_find->v_ctime   = 0;
        pst_find->v_atime   = 0;
        pst_find->v_mtime   = 0;
        pst_find->u_size    = 0;
    }

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      _find
// PURPOSE
//
// PARAMETERS
//      PAXFILEFIND        pst_find --
// RESULT
//      struct dirent * --
// ***************************************************************************
static struct dirent * _find(PFINDRES pst_res)
{
    struct dirent *     pst_found       = NULL;
    struct dirent *     pst_dir;

    ENTER(true);

    while (!pst_found && ((pst_dir = readdir(pst_res->pst_dir)) != NULL))
    {
        if (strz_wildcard(pst_dir->d_name, pst_res->sz_mask))
        {
            pst_found = pst_dir;
        }
    }

    RETURN(pst_found);
}
// ***************************************************************************
// FUNCTION
//      axfile_find_destroy
// PURPOSE
//      Destroy Find control structure
// PARAMETERS
//      PAXFILEFIND pst_find -- Pointer to Find control structure
// RESULT
//      PAXFILEFIND  -- Always NULL
// ***************************************************************************
PAXFILEFIND axfile_find_destroy(PAXFILEFIND pst_find)
{
    PFINDRES        pst_res     = (PFINDRES)pst_find->p_handle;

    ENTER(pst_find);

    if (pst_res->pst_dir)
        closedir(pst_res->pst_dir);

    FREE(pst_res);
    FREE(pst_find);

    RETURN(NULL);
}
// ***************************************************************************
// FUNCTION
//      axfile_find_first
// PURPOSE
//      Find first file
// PARAMETERS
//      PSTR     psz_path  -- Start path
//      UINT     u_mode -- Attributes of file (see dreamix.h)
// RESULT
//      PAXFILEFIND -- Pointer to the just created Find control structure
//                  if file was found or NULL if not
// ***************************************************************************
PAXFILEFIND axfile_find_first(PSTR  psz_mask,
                              UINT  u_mode)
{
    PAXFILEFIND         pst_find        = NULL;
    struct dirent *     pst_dir;
    PSTR                psz_on;
    PFINDRES            pst_res;

    ENTER(psz_mask);

    if ((pst_find = CREATE(AXFILEFIND)) != NULL)
    {
        if ((pst_res = CREATE(FINDRES)) != NULL)
        {
            pst_find->p_handle = pst_res;

            strz_cpy(pst_res->sz_dir, psz_mask, AXLPATH);

            if ((psz_on = strrchr(pst_res->sz_dir, '/')) != NULL)
            {
                *(psz_on++) = 0;

                strz_cpy(pst_res->sz_mask, psz_on, AXLPATH);
            }
            else
            {
                strz_cpy(pst_res->sz_mask, psz_mask, AXLPATH);
                strcpy(pst_res->sz_dir, ".");
            }

            if ((pst_res->pst_dir = opendir(pst_res->sz_dir)) != NULL)
            {
                if ((pst_dir = _find(pst_res)) != NULL)
                {
                    _cvt_dir(pst_find, pst_dir);
                }
                else
                    pst_find = axfile_find_destroy(pst_find);
            }
            else
                pst_find = axfile_find_destroy(pst_find);
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
//      BOOL  -- true if file was found false if not
// ***************************************************************************
BOOL axfile_find_next(PAXFILEFIND pst_find)
{
    BOOL                b_result        = false;
    PFINDRES            pst_res         = (PFINDRES)pst_find->p_handle;
    struct dirent *     pst_found       = NULL;

    ENTER(pst_find && pst_res->pst_dir);

    if ((pst_found = _find(pst_res)) != NULL)
    {
        _cvt_dir(pst_find, pst_found);

        b_result = true;
    }

    RETURN(b_result);
}

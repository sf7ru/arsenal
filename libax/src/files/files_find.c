// ***************************************************************************
// TITLE
//      File Find Utilities
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: ffind.c,v 1.3 2004/03/24 08:28:26 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <arsenal.h>
#include <axfile.h>
#include <axstring.h>

// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

INT axfile_find_X(PSTR              psz_path,
                  PSTR              psz_subpath,
                  PSTR              psz_mask,
                  UINT              u_flags,
                  PFNAXFILEFINDCB   pfn_callback,
                  PVOID             p_param)
{
#define CALLBACK    if (!(*pfn_callback)(pst_find, psz_path, psz_subpath, p_param)) \
                    { i_found = -i_found; }

    INT             i_found         = 0;
    INT             i_subdir;
    PAXFILEFIND     pst_find;
    PSTR            psz_curr_mask;
    BOOL            b_proceed;
    CHAR            sz_full_path    [ AXLPATH + 1 ];
    CHAR            sz_tmp_mask     [ AXLPATH + 1 ];


    ENTER(psz_path && psz_mask && pfn_callback);

    strz_sformat(sz_full_path, AXLPATH,
                 "%s/%s/*", psz_path, psz_subpath);

    if ((pst_find = axfile_find_first(sz_full_path, 0)) != NULL)
    {
        do
        {
            i_found++;

            if ( strcmp(pst_find->sz_name, DEF_UPDIRNAME)      &&
                 strcmp(pst_find->sz_name, DEF_CURRDIRNAME)    )
            {
                if (S_ISDIR(pst_find->u_mode))
                {
                    if (u_flags & AXFFF_recurse)
                    {
// ------------------------ Recurse subdirectory -----------------------------

                        if (u_flags & AXFFF_dir)
                        {
                            CALLBACK;
                        }

                        sprintf(sz_full_path, "%s/%s", psz_subpath, pst_find->sz_name);

                        if ((i_subdir = axfile_find_X(psz_path, sz_full_path, psz_mask,
                                                      u_flags, pfn_callback, p_param)) >= 0)
                        {
                            i_found += i_subdir;
                        }
                        else
                        {
                            i_found = (-i_found) + i_subdir;
                        }

                        if (u_flags & AXFFF_dir_after)
                        {
                            CALLBACK;
                        }
                    }
                    else if (u_flags & (AXFFF_dir | AXFFF_dir_after))
                    {
                        CALLBACK;
                    }
                }
                else                    //  if (u_flags & ...
                {

// ----------------- Matching filename with set of masks ---------------------

                    b_proceed     = false;
                    psz_curr_mask = psz_mask;

                    while ( !b_proceed &&
                            ((psz_curr_mask = strz_substring_get_w_quotes(sz_tmp_mask, psz_curr_mask, AXLPATH, AXFFF_LIST_DELIMITER, AXFFF_LIST_QUOTE)) != NULL) )
                    {
                        if ((b_proceed = strz_wildcard(pst_find->sz_name, sz_tmp_mask)) != false)
                        {
                            CALLBACK;
                        }
                    }
                }
            }                           //  if ( strcmp(pst_find->sz_name...

        } while ((i_found >= 0) && axfile_find_next(pst_find));

        axfile_find_destroy(pst_find);
    }                                   // if ((pst_find = la6_file_find_first...

    RETURN(i_found);

#undef CALLBACK
}

// ***************************************************************************
// FUNCTION
//      axfile_find
// PURPOSE
//      Recursive file search
// PARAMETERS
//      PSTR               psz_path     -- Base path to search
//      PSTR               psz_subpath  -- Subpath to search
//      PSTR               psz_mask     -- List of mask strings, separated
//                                         by zero. Last element just zero
//      UINT               u_flags      -- Flags
//      PFNAXFILEFINDCB    pfn_callback -- Callback function which calls every
//                                         time when matched file has found
//      PVOID              p_param      -- Pointer to User parameter for
//                                         Callback function
// RESULT
//      INT  -- code
//                   0     : nothing found
//                  -1     : error while searching
//                   other : number of found entries
// ***************************************************************************
INT axfile_find(PSTR               psz_path,
                PSTR               psz_subpath,
                PSTR               psz_mask,
                UINT               u_flags,
                PFNAXFILEFINDCB    pfn_callback,
                PVOID              p_param)
{
#define CALLBACK    if (!(*pfn_callback)(pst_find, psz_path, psz_subpath, p_param)) \
                    { i_found = -i_found; }

    INT             i_found         = 0;
    INT             i_subdir;
    PAXFILEFIND     pst_find;
    PSTR            psz_curr_mask;
    BOOL            b_proceed;
    CHAR            sz_full_path    [ AXLPATH + 1 ];
    CHAR            sz_tmp_mask     [ AXLPATH + 1 ];


    ENTER(psz_path && psz_mask && pfn_callback);

    strz_sformat(sz_full_path, AXLPATH,
                 "%s/%s/*", psz_path, psz_subpath);

    if ((pst_find = axfile_find_first(sz_full_path, 0)) != NULL)
    {
        do
        {
            i_found++;

            if ( strcmp(pst_find->sz_name, DEF_UPDIRNAME)      &&
                 strcmp(pst_find->sz_name, DEF_CURRDIRNAME)    )
            {
                if (S_ISDIR(pst_find->u_mode))
                {
                    if (u_flags & AXFFF_recurse)
                    {
// ------------------------ Recurse subdirectory -----------------------------

                        if (u_flags & AXFFF_dir)
                        {
                            CALLBACK;
                        }

                        sprintf(sz_full_path, "%s/%s", psz_subpath, pst_find->sz_name);

                        if ((i_subdir = axfile_find_X(psz_path, sz_full_path, psz_mask,
                                                      u_flags, pfn_callback, p_param)) >= 0)
                        {
                            i_found += i_subdir;
                        }
                        else
                        {
                            i_found = (-i_found) + i_subdir;
                        }

                        if (u_flags & AXFFF_dir_after)
                        {
                            CALLBACK;
                        }
                    }
                    else if (u_flags & (AXFFF_dir | AXFFF_dir_after))
                    {
                        CALLBACK;
                    }
                }
                else                    //  if (u_flags & ...
                {

// ----------------- Matching filename with set of masks ---------------------

                    b_proceed     = false;
                    psz_curr_mask = psz_mask;

                    while ( !b_proceed &&
                            ((psz_curr_mask = strz_substring_get_w_quotes(sz_tmp_mask,
                                                                          psz_curr_mask,
                                                                          AXLPATH,
                                                                          AXFFF_LIST_DELIMITER,
                                                                          AXFFF_LIST_QUOTE)) != NULL) )
                    {
                        if ((b_proceed = strz_wildcard(pst_find->sz_name, sz_tmp_mask)) != false)
                        {
                            CALLBACK;
                        }
                    }
                }
            }                           //  if ( strcmp(pst_find->sz_name...

        } while ((i_found >= 0) && axfile_find_next(pst_find));

        axfile_find_destroy(pst_find);
    }                                   // if ((pst_find = la6_file_find_first...

    RETURN(i_found);

#undef CALLBACK
}

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
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <arsenal.h>
#include <axlog.h>
#include <axstring.h>
#include <axfile.h>
#include <axtime.h>
#include <axlog.h>
#include <pvt_log.h>



// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define NAME_SZ             AXLPATH
#define STAMP_SZ            128
#define DEF_DAY             (24 * 60 * 60)


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// STRUCTURE
//      LA6FILEWAY
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_FILEWAY
{
    AXLOGWAY        st_std;
    U32          u_flags;
    FILE *          pst_file;
    AXDATE          st_last;
    UINT            u_lifetime;

    HAXMUTEX        LCK;

    CHAR            sz_path         [ AXLPATH ];
    CHAR            sz_name         [ NAME_SZ ];
    CHAR            sz_filename     [ AXLPATH ];
} FILEWAY, * PFILEWAY;


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      _file_rotate
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PFILEWAY   pst_way --
// RESULT
//      void --
// ***************************************************************************
static BOOL _file_rotate(PFILEWAY       pst_way,
                         PAXDATE        pst_date)
{
    BOOL            b_result        = true;
    UINT            modifier        = 1;
    CHAR            sz_fn_new       [ AXLPATH + 1 ];
    CHAR            sz_fn_old       [ AXLPATH + 1 ];
    CHAR            sz_fn_ori       [ AXLPATH + 1 ];

    ENTER(pst_way);

    if (  ( (pst_date->year  != pst_way->st_last.year)    ||
            (pst_date->month != pst_way->st_last.month)   ||
            (pst_date->day   != pst_way->st_last.day)     ))
    {
        strz_sformat(sz_fn_old, AXLPATH, "%s/%s.log",
                 pst_way->sz_path, pst_way->sz_name);

        if (pst_way->pst_file)
            fclose(pst_way->pst_file);

        if (pst_way->u_flags & AXLOGFL_NAMEDIR)
        {
            strz_sformat(sz_fn_new, AXLPATH, "%s/%s/%.4d-%.2d-%.2d.log",
                     pst_way->sz_path,
                     pst_way->sz_name,
                     pst_way->st_last.year,
                     pst_way->st_last.month,
                     pst_way->st_last.day);
        }
        else
        {
            if (pst_way->u_flags & AXLOGFL_DATEDIR)
            {
                strz_sformat(sz_fn_new, AXLPATH, "%s/%.4d-%.2d-%.2d/%s.log",
                         pst_way->sz_path,
                         pst_way->st_last.year,
                         pst_way->st_last.month,
                         pst_way->st_last.day,
                         pst_way->sz_name);
            }
            else
            {
                strz_sformat(sz_fn_new, AXLPATH, "%s/%s.%.4d-%.2d-%.2d.log",
                         pst_way->sz_path,
                         pst_way->sz_name,
                         pst_way->st_last.year,
                         pst_way->st_last.month,
                         pst_way->st_last.day);
            }
        }

        axpath_create_to_file(sz_fn_new);

        strz_cpy(sz_fn_ori, sz_fn_new, AXLPATH);
        fname_change_ext(sz_fn_ori, AXLPATH, "");

        while (axfile_exist(sz_fn_new))
        {
            snprintf(sz_fn_new, AXLPATH, "%s-%u.log", sz_fn_ori, modifier++);
        }

        b_result = axfile_move(sz_fn_new, sz_fn_old, true);

        memcpy(&pst_way->st_last, pst_date, sizeof(pst_way->st_last));

        if ((pst_way->pst_file = fopen(sz_fn_old, "at")) != NULL)
        {
            fprintf(pst_way->pst_file, "S %.4d-%.2d-%.2d %.2d:%.2d:%.2d  %s '%s'\n",
                    pst_date->year, pst_date->month, pst_date->day,
                    pst_date->hour, pst_date->minute, pst_date->second,
                    (b_result ? "previous log in" : "cannot move log to"),
                    sz_fn_new);
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      _found
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PAXFILEFIND   pst_find --
//      PSTR          psz_name --
//      PFILEWAY      pst_way  --
// RESULT
//      BOOL --
// ***************************************************************************
static BOOL _found(PAXFILEFIND      pst_find,
                   PSTR             psz_path,
                   PSTR             psz_subdir,
                   PFILEWAY         pst_way)
{
    BOOL            b_result        = true;
    CHAR            sz_filename     [ AXLPATH ];

    ENTER(true);

    if ((pst_find->v_mtime + (pst_way->u_lifetime * DEF_DAY)) < axutime_get())
    {
        strz_sformat(sz_filename, AXLPATH, "%s/%s/%s", psz_path, psz_subdir, pst_find->sz_name);

        remove(sz_filename);
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      _file_clean
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PFILEWAY   pst_way --
// RESULT
//      void --
// ***************************************************************************
static void _file_clean(PFILEWAY        pst_way)
{
    CHAR            sz_path         [ AXLPATH ];
    CHAR            sz_mask         [ AXLPATH ];

    ENTER(pst_way);


    if (pst_way->u_flags & AXLOGFL_NAMEDIR)
    {
        strz_sformat(sz_path, AXLPATH, "%s/%s",
                 pst_way->sz_path, pst_way->sz_name);

        strz_cpy(sz_mask, "*.log", AXLPATH);
    }
    else
    {
        if (pst_way->u_flags & AXLOGFL_DATEDIR)
        {
            strz_cpy(sz_path, pst_way->sz_path, AXLPATH);

            strz_sformat(sz_mask, AXLPATH, "%s.log", pst_way->sz_name);
        }
        else
        {
            strz_cpy(sz_path, pst_way->sz_path, AXLPATH);

            strz_sformat(sz_mask, AXLPATH, "%s.*.log", pst_way->sz_name);
        }
    }

    axfile_find(sz_path, "", sz_mask, AXFFF_recurse, (PFNAXFILEFINDCB)_found, pst_way);

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      _fileway_msg
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PFILEWAY     pst_way  --
//      AXLOGDL      v_level  --
//      PAXDATE      pst_date --
//      PSTR         psz_msg  --
// RESULT
//      void --
// ***************************************************************************
static void _fileway_msg(PFILEWAY       pst_way,
                         AXLOGDL        v_level,
                         PAXDATE        pst_date,
                         PSTR           psz_msg)
{
    PSTR                psz_start       = psz_msg;
    PSTR                psz_end;
    CHAR                sz_stamp        [ STAMP_SZ + 1 ]    = "";

    ENTER(true);

    if (pst_way->LCK)
        axmutex_lock(pst_way->LCK, true);

    if (  pst_way->pst_file                                                 ||
          ((pst_way->pst_file = fopen(pst_way->sz_filename, "at")) != NULL) )
    {
        if (pst_way->u_flags & AXLOGFL_STAMP)
        {
            strz_sformat(sz_stamp, STAMP_SZ, "%s %.4d-%.2d-%.2d %.2d:%.2d:%.2d  ",
                     strz_sign_from_AXLOGDL(v_level),
                     pst_date->year, pst_date->month,   pst_date->day,
                     pst_date->hour, pst_date->minute,  pst_date->second);
        }

        if (pst_way->u_flags & AXLOGFL_ROTATE)
        {
/*
            printf("LOG ROTATE: (%.4d-%.2d-%.2d %.2d:%.2d:%.2d) (%.4d-%.2d-%.2d %.2d:%.2d:%.2d)\n",
                   pst_way->st_last.year, pst_way->st_last.month,   pst_way->st_last.day,
                   pst_way->st_last.hour, pst_way->st_last.minute,  pst_way->st_last.second,
                     pst_date->year, pst_date->month,   pst_date->day,
                     pst_date->hour, pst_date->minute,  pst_date->second);
*/
            if (_file_rotate(pst_way, pst_date))
            {
                if (pst_way->u_lifetime)
                {
                    _file_clean(pst_way);
                }
            }
        }

        do
        {
            if ((psz_end = strchr(psz_start, '\n')) != NULL)
            {
                *(psz_end++) = 0;

                while (*psz_end && (*((PU8)psz_end) < ' '))
                {
                    psz_end++;
                }
            }

            fprintf(pst_way->pst_file, "%s%s\n", sz_stamp, psz_start);

            psz_start = psz_end;

        } while (psz_start && *psz_start);

        if (pst_way->u_flags & AXLOGFL_SHARED)
        {
            fclose(pst_way->pst_file);
            pst_way->pst_file = NULL;
        }
        else
            fflush(pst_way->pst_file);
    }

    if (pst_way->LCK)
        axmutex_unlock(pst_way->LCK);

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      axlog_file_close
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PFILEWAY   pst_way --
// RESULT
//      PFILEWAY --
// ***************************************************************************
static PFILEWAY _fileway_close(PFILEWAY pst_way)
{
    ENTER(pst_way);

    if (pst_way->pst_file)
        fclose(pst_way->pst_file);

    SAFEDESTROY(axmutex, pst_way->LCK);

    FREE(pst_way);

    RETURN(NULL);
}
// ***************************************************************************
// FUNCTION
//      axlog_file_open
// PURPOSE
//      Open Log file
// PARAMETERS
//      PSTR           psz_name -- Pointer to unique Log name
//      PSTR           psz_path -- Pointer to path for log file
//      DWORD          u_flags  -- Flags for creation
//                  AXLOGFL_no_stat: Do not collect statistic about tail
//                  AXLOGFL_rotate:  Do everyday log rotation
//                  AXLOGFL_no_dirs: Do not make directories while rotation
// RESULT
//      PFILEWAY -- Pointer to the just created Log control structure
//                        if file was created or NULL is not
// ***************************************************************************
HAXLOGWAY axlog_file_way(PCSTR      psz_path,
                         PCSTR      psz_name,
                         UINT       u_lifetime,
                         UINT       u_flags)
{
    PFILEWAY        pst_way             = NULL;

    ENTER(psz_name && psz_path);

    if ((pst_way = CREATE(FILEWAY)) != NULL)
    {
        pst_way->st_std.pfn_close   = (PFNLOGCLOSE)_fileway_close;
        pst_way->st_std.pfn_msg     = (PFNLOGMSG)_fileway_msg;
        pst_way->st_std.v_type      = AXLOGWT_file;
        pst_way->u_lifetime         = u_lifetime;
        pst_way->u_flags            = u_flags;

        strz_cpy(pst_way->sz_name, psz_name, NAME_SZ);
        strz_cpy(pst_way->sz_path, psz_path, AXLPATH);

        strz_trim_s(pst_way->sz_path, AXLPATH);

        strz_sformat(pst_way->sz_filename, AXLPATH, "%s/%s.log",
                     pst_way->sz_path, pst_way->sz_name);

        if (axpath_create_to_file(pst_way->sz_filename))
        {
            if (u_flags & AXLOGFL_ROTATE)
            {
                if (!axfile_times_get(NULL, NULL, &pst_way->st_last, pst_way->sz_filename))
                {
                    axdate_get(&pst_way->st_last);
                }
            }

            if (pst_way->u_flags & AXLOGFL_SHARED)
            {
                if ((pst_way->LCK = axmutex_create()) == NULL)
                {
                    pst_way = _fileway_close(pst_way);
                }
            }
            else
            {
                if ((pst_way->pst_file = fopen(pst_way->sz_filename, "at")) == NULL)
                {
                    pst_way = _fileway_close(pst_way);
                }
            }
        }
        else
        {
            pst_way = _fileway_close(pst_way);
        }
    }

    RETURN((HAXLOGWAY)pst_way);
}

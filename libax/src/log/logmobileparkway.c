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

#include <axstring.h>
#include <axfile.h>

#include <axlog.h>
#include <pvt_log.h>



// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define NAME_SZ             32
#define STAMP_SZ            128
#define DEF_DAY             (24 * 60 * 60)


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// STRUCTURE
//      LA6MPWAY
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_ONEFILE
{
    U32          filter;
    FILE *          pst_file;
    AXDATE          st_last;

    CHAR            sz_template     [ AXLPATH ];
} ONEFILE, * PONEFILE;


// ***************************************************************************
// STRUCTURE
//      LA6MPWAY
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_MPWAY
{
    AXLOGWAY        st_std;
    U32          u_flags;
    UINT            files;
    ONEFILE         file            [ AXLOGMPWAY_MAXFILES ];
} MPWAY, * PMPWAY;


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      constructLogNameFromTempl
// PURPOSE
//
// PARAMETERS
//      PSTR    filename --
//      UINT    len      --
//      PSTR    templ    --
//      PAXDATE pst_date --
// RESULT
//      void --
// ***************************************************************************
void constructLogNameFromTempl(PSTR      filename,
                               UINT      len,
                               PSTR      templ,
                               PAXDATE   pst_date)
{
    PSTR        onS;
    PSTR        onT;
    UINT        tmp;
    UINT        left;

    ENTER(true);

    onS     = templ;
    onT     = filename;
    left    = len - 1;

    while (*onS && left)
    {
        if (*onS == '%')
        {
            tmp = 0;

            switch (*(++onS))
            {
                case 'Y':
                    tmp = strz_sformat(onT, left, "%.4d", pst_date->year);
                    break;

                case 'M':
                    tmp = strz_sformat(onT, left, "%.2d", pst_date->month);
                    break;

                case 'd':
                    tmp = strz_sformat(onT, left, "%.2d", pst_date->day);
                    break;

                case 'h':
                    tmp = strz_sformat(onT, left, "%.2d", pst_date->hour);
                    break;

                case 'm':
                    tmp = strz_sformat(onT, left, "%.2d", pst_date->minute);
                    break;

                case 's':
                    tmp = strz_sformat(onT, left, "%.2d", pst_date->second);
                    break;

                case '%':
                    *onT = '%';
                    tmp  = 1;
                    break;

                default:

                    break;
            }

            left -= tmp;
            onT  += tmp;
            onS++;
        }
        else
        {
            *(onT++) = *(onS++);
            left--;
        }
    }

    *onT = 0;

    QUIT;
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      fileCheck
// PURPOSE
//
// PARAMETERS
//      PONEFILE F        --
//      PAXDATE  pst_date --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
static BOOL fileCheck(PONEFILE     F,
                      PAXDATE      pst_date)
{
    BOOL            b_result        = false;
    CHAR            filename        [ AXLPATH + 1 ];

    ENTER(F);

    if (    (F->pst_file        == NULL)                ||
            ( (pst_date->year != F->st_last.year)   ||
            (pst_date->month  != F->st_last.month)  ||
            (pst_date->day    != F->st_last.day)    ||
            (pst_date->hour   != F->st_last.hour)   ))
    {
        constructLogNameFromTempl(filename, AXLPATH, F->sz_template, pst_date);

        if (F->pst_file)
            fclose(F->pst_file);

        if (axpath_create_to_file(filename))
        {
            if ((F->pst_file = fopen(filename, "at")) != NULL)
            {
                memcpy(&F->st_last, pst_date, sizeof(F->st_last));

                b_result = true;
            }
        }
    }
    else
        b_result = true;

    RETURN(b_result);
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      spreadWord
// PURPOSE
//
// PARAMETERS
//      PMPWAY  pst_way   --
//      U32  mask      --
//      PAXDATE pst_date  --
//      PSTR    psz_stamp --
//      PSTR    psz_word  --
// RESULT
//      void --
// ***************************************************************************
static void spreadWord(PMPWAY       pst_way,
                       U32       mask,
                       PAXDATE      pst_date,
                       PSTR         psz_stamp,
                       PSTR         psz_word)
{
    UINT                idx;

    ENTER(true);

    for (idx = 0; idx < pst_way->files; idx++)
    {
        if (  (pst_way->file[idx].filter & mask)        &&
              fileCheck(&pst_way->file[idx], pst_date)  )
        {
            if (pst_way->file[idx].pst_file != NULL)
            {
                fprintf(pst_way->file[idx].pst_file,
                        "%s %s\n", psz_stamp, psz_word);
            }
        }
    }

    QUIT;
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      flushFiles
// PURPOSE
//
// PARAMETERS
//      PMPWAY pst_way --
// RESULT
//      void --
// ***************************************************************************
static void flushFiles(PMPWAY       pst_way)
{
    UINT                idx;

    ENTER(true);

    for (idx = 0; idx < pst_way->files; idx++)
    {
        if (pst_way->file[idx].pst_file)
        {
            fflush(pst_way->file[idx].pst_file);
        }
    }

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      _mpway_msg
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PMPWAY     pst_way  --
//      AXLOGDL      v_level  --
//      PAXDATE      pst_date --
//      PSTR         psz_msg  --
// RESULT
//      void --
// ***************************************************************************
static void _mpway_msg(PMPWAY       pst_way,
                       AXLOGDL      v_level,
                       PAXDATE      pst_date,
                       PSTR         psz_msg)
{
    PSTR                psz_start       = psz_msg;
    UINT                mask;
    PSTR                psz_end;
    CHAR                sz_stamp        [ STAMP_SZ + 1 ];

    ENTER(true);

    strz_sformat(sz_stamp, STAMP_SZ, "%.2d.%.2d.%.4d %.2d:%.2d:%.2d;%s;",
             pst_date->day, pst_date->month, pst_date->year,
             pst_date->hour, pst_date->minute,  pst_date->second,
             strz_from_AXLOGDL(v_level));

/*
        printf("LOG ROTATE: (%.4d-%.2d-%.2d %.2d:%.2d:%.2d) (%.4d-%.2d-%.2d %.2d:%.2d:%.2d)\n",
               pst_way->st_last.year, pst_way->st_last.month,   pst_way->st_last.day,
               pst_way->st_last.hour, pst_way->st_last.minute,  pst_way->st_last.second,
                 pst_date->year, pst_date->month,   pst_date->day,
                 pst_date->hour, pst_date->minute,  pst_date->second);
*/

    mask = (v_level < AXLOGDL_bcast) ? (1 << v_level) : -1;

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

        if (pst_way->u_flags & AXLOGFL_CONSOLE)
        {
            printf("%s %s\n", sz_stamp, psz_start);
        }

        spreadWord(pst_way, mask, pst_date, sz_stamp, psz_start);

        psz_start = psz_end;

    } while (psz_start && *psz_start);

    flushFiles(pst_way);

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      axlog_mobilepark_close
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PMPWAY   pst_way --
// RESULT
//      PMPWAY --
// ***************************************************************************
static PMPWAY _mpway_close(PMPWAY pst_way)
{
    UINT            idx;

    ENTER(pst_way);

    for (idx = 0; idx < AXLOGMPWAY_MAXFILES; idx++)
    {
        if (pst_way->file[idx].pst_file != NULL)
        {
            fclose(pst_way->file[idx].pst_file);
        }
    }

    FREE(pst_way);

    RETURN(NULL);
}
// ***************************************************************************
// FUNCTION
//      axlog_mobilepark_open
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
//      PMPWAY -- Pointer to the just created Log control structure
//                        if file was created or NULL is not
// ***************************************************************************
HAXLOGWAY axlog_mobilepark_way(UINT     u_flags)
{
    PMPWAY          pst_way             = NULL;

    ENTER(true);

    if ((pst_way = CREATE(MPWAY)) != NULL)
    {
        pst_way->st_std.pfn_close   = (PFNLOGCLOSE)_mpway_close;
        pst_way->st_std.pfn_msg     = (PFNLOGMSG)_mpway_msg;
        pst_way->st_std.v_type      = AXLOGWT_mp;
        pst_way->u_flags            = u_flags;
    }

    RETURN((HAXLOGWAY)pst_way);
}
// ***************************************************************************
// FUNCTION
//      axlog_mobilepark_add_file
// PURPOSE
//
// PARAMETERS
//      HAXLOGWAY h_way     --
//      PCSTR     psz_fname --
//      U32    u_filter  --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL axlog_mobilepark_add_file(HAXLOGWAY          h_way,
                               PCSTR              psz_fname,
                               U32             u_filter)
{
    BOOL            b_result        = false;
    PMPWAY          pst_way         = (PMPWAY)h_way;
    PONEFILE        F;
    AXDATE          date;


    ENTER(psz_fname);

    if (pst_way->files < AXLOGMPWAY_MAXFILES)
    {
        F = &pst_way->file[pst_way->files];

        strz_cpy(F->sz_template, psz_fname, AXLPATH);

        strz_trim_s(F->sz_template, AXLPATH);

        F->filter = u_filter;

        axdate_get(&date);

        if (fileCheck(F, &date))
        {
            pst_way->files++;
            b_result = true;
        }
    }

    RETURN(b_result);
}

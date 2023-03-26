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

#include <axfile.h>

#include <arsenal.h>
#include <records.h>
#include <axhashing.h>

#include <pvt_records.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// ---------------------------------------------------------------------------

#define COUNT(a)        (a->p_bottom - a->p_top) / a->u_recsize

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

// ***************************************************************************
// STRUCTURE
//      FILEHEADER
// ***************************************************************************
typedef struct __tag_FILEHEADER
{
    U32             u_checksum;
    UINT            u_version;
    UINT            u_total;
    UINT            u_recsize;
    UINT            u_maxrecs;
    UINT            u_init;
} FILEHEADER, * PFILEHEADER;

// ***************************************************************************
// STRUCTURE
//      FILEENTRY
// ***************************************************************************
typedef struct __tag_FILEENTRY
{
    UINT            u_size;
} FILEENTRY, * PFILEENTRY;

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      recqueue_destroy
// PURPOSE
//
// PARAMETERS
//      HRECQUEUE   h_queue --
// RESULT
//      HRECQUEUE  --
// ***************************************************************************
HRECQUEUE recqueue_destroy(HRECQUEUE h_queue)
{
    PRECQUEUE       pst_Q   = (PRECQUEUE)h_queue;

    ENTER(pst_Q);

    if (recqueue_clean(h_queue))
    {
        if (pst_Q->p_data)
            FREE(pst_Q->p_data);

        FREE(pst_Q);

        pst_Q = NULL;
    }

    RETURN((HRECQUEUE)pst_Q);
}
// ***************************************************************************
// FUNCTION
//      recqueue_create
// PURPOSE
//
// PARAMETERS
//      UINT          u_recsize -- Record size
//      UINT          u_init     -- Number of records for initial
//                                  memory allocation for queue data
//      UINT          u_max      -- Maximum number of items in queue
//      PFNRECCLEAN   pfn_clean  -- Cleaner function
// RESULT
//      HRECQUEUE  --
// ***************************************************************************
HRECQUEUE recqueue_create(UINT          u_recsize,
                          UINT          u_init,
                          UINT          u_max,
                          PFNRECCLEAN   pfn_clean)
{
    PRECQUEUE       pst_Q       = NULL;
    UINT            u_size;

    ENTER(u_recsize && u_init);

    if ((pst_Q = CREATE(RECQUEUE)) != NULL)
    {
        pst_Q->pfn_clean    = pfn_clean;
        pst_Q->u_init       = u_init;
        pst_Q->u_recsize    = u_recsize;
        pst_Q->u_maxrecs    = u_max;
        u_size              = u_recsize * u_init;

        if ((pst_Q->p_data = MALLOC(u_size)) != NULL)
        {
            pst_Q->p_top = pst_Q->p_bottom = pst_Q->p_data;
            pst_Q->p_end = pst_Q->p_data + u_size;
        }
        else
            pst_Q = (PRECQUEUE)recqueue_destroy((HRECQUEUE)pst_Q);
     }

    RETURN((HRECQUEUE)pst_Q);
}
// ***************************************************************************
// FUNCTION
//      recqueue_peek
// PURPOSE
//
// PARAMETERS
//      HRECQUEUE   h_queue --
// RESULT
//      PVOID  --
// ***************************************************************************
PVOID recqueue_peek(HRECQUEUE h_queue)
{
    PVOID           p_result    = NULL;
    PRECQUEUE       pst_Q       = (PRECQUEUE)h_queue;

    ENTER(pst_Q);

    if (pst_Q->p_top != pst_Q->p_bottom)
        p_result = pst_Q->p_top;

    RETURN(p_result);
}
// ***************************************************************************
// FUNCTION
//      recqueue_peek_x
// PURPOSE
//
// PARAMETERS
//      HRECQUEUE h_queue --
//      UINT      index   --
// RESULT
//      PVOID --
// ***************************************************************************
PVOID recqueue_peek_x(HRECQUEUE h_queue,
                      UINT      index)
{
    PVOID           p_result    = NULL;
    PRECQUEUE       pst_Q       = (PRECQUEUE)h_queue;
    PU8           on;

    ENTER(pst_Q);

    on = pst_Q->p_top + (index * pst_Q->u_recsize);

    if (on < pst_Q->p_bottom)
    {
        p_result = on;
    }

    RETURN(p_result);
}
// ***************************************************************************
// FUNCTION
//      recqueue_peek
// PURPOSE
//
// PARAMETERS
//      HRECQUEUE   h_queue --
// RESULT
//      PVOID  --
// ***************************************************************************
BOOL recqueue_replace(HRECQUEUE    h_queue,
                      PVOID        original,
                      PVOID        replacement)
{
    BOOL            b_result    = false;
    PU8           on;
    PRECQUEUE       pst_Q       = (PRECQUEUE)h_queue;

    ENTER(pst_Q);

    on = (PU8)original;

    if ( (on >= pst_Q->p_top)                           &&
         ((on + pst_Q->u_recsize) <= pst_Q->p_bottom)   )
    {
        memcpy(on, replacement, pst_Q->u_recsize);

        b_result = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      recqueue_count
// PURPOSE
//
// PARAMETERS
//      HRECQUEUE   h_queue --
// RESULT
//      UINT  --
// ***************************************************************************
UINT recqueue_count(HRECQUEUE h_queue)
{
    UINT            u_count     = 0;
    PRECQUEUE       pst_Q       = (PRECQUEUE)h_queue;

    ENTER(pst_Q);

    if (pst_Q->p_bottom && pst_Q->p_top && pst_Q->u_recsize)
    {
        u_count = COUNT(pst_Q);
    }

    RETURN(u_count);
}
// ***************************************************************************
// FUNCTION
//      la6_recqueue_clean
// PURPOSE
//      Clean Record Queue
// PARAMETERS
//      HRECQUEUE pst_Q -- Pointer to Record Queue control structure
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
BOOL recqueue_clean(HRECQUEUE h_queue)
{
    BOOL            b_result    = false;
    PRECQUEUE       pst_Q       = (PRECQUEUE)h_queue;
    PU8           p_on;

    ENTER(pst_Q);

    b_result = true;

    if (pst_Q->pfn_clean)
    {
/*
        for (  p_on = pst_Q->p_data;
               b_result && (p_on < pst_Q->p_end);
               p_on += pst_Q->u_recsize)
        {
            if (!(*pst_Q->pfn_clean)(p_on))
            {
                b_result = false;
            }
        }
*/

        for (  p_on = pst_Q->p_top;
               b_result && (p_on < pst_Q->p_bottom);
               p_on += pst_Q->u_recsize)
        {
            if (!(*pst_Q->pfn_clean)(p_on))
            {
                b_result = false;
            }
        }
    }

    if (b_result)
    {
        pst_Q->p_top = pst_Q->p_bottom = pst_Q->p_data;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      recqueue_to_recarray
// PURPOSE
//
// PARAMETERS
//      HRECQUEUE h_queue --
//      BOOL      clean   --
// RESULT
//      HRECARRAY --
// ***************************************************************************
HRECARRAY recqueue_to_recarray(HRECQUEUE          h_queue,
                               BOOL               clean)
{
    PRECARRAY       pst_records = NULL;
    PRECQUEUE       pst_Q       = (PRECQUEUE)h_queue;
    UINT            u_count;

    ENTER(pst_Q);

    if ((u_count = recqueue_count(h_queue)) != 0)
    {
        if ((pst_records = (PRECARRAY)recarray_create(
                pst_Q->u_recsize, u_count, 1, pst_Q->pfn_clean)) != NULL)
        {
            memcpy(pst_records->p_data, pst_Q->p_top, pst_Q->u_recsize * u_count);

            pst_records->u_total    = u_count;
            pst_records->u_used     = u_count;

            if (clean)
            {
                pst_Q->p_top = pst_Q->p_bottom = pst_Q->p_data;
            }
        }
    }

    RETURN((HRECARRAY)pst_records);
}
// ***************************************************************************
// FUNCTION
//      recqueue_get
// PURPOSE
//
// PARAMETERS
//      HRECQUEUE   h_queue --
//      PVOID       p_data  -- Pointer to target data buffer. In case it equals
//                             zero, no data will be copied, but record
//                             will be pushed out
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL recqueue_get(HRECQUEUE     h_queue,
                  PVOID         p_data)
{
    BOOL            b_result    = false;
    PRECQUEUE       pst_Q       = (PRECQUEUE)h_queue;
    PVOID           p_tmp;

    ENTER(pst_Q);

    if ((p_tmp = recqueue_peek(h_queue)) != NULL)
    {
        if (p_data)
            memcpy(p_data, p_tmp, pst_Q->u_recsize);

        pst_Q->p_top += pst_Q->u_recsize;

        if (pst_Q->p_top == pst_Q->p_bottom)
            pst_Q->p_top = pst_Q->p_bottom = pst_Q->p_data;

        b_result = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      recqueue_del
// PURPOSE
//
// PARAMETERS
//      HRECQUEUE h_queue --
//      UINT      index   --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL recqueue_del(HRECQUEUE     h_queue,
                  UINT          index)
{
    BOOL            b_result    = false;
    PRECQUEUE       pst_Q       = (PRECQUEUE)h_queue;
    PU8           to;
    PU8           from;

    ENTER(pst_Q);

    if ((to = (PU8)recqueue_peek_x(h_queue, index)) != nil)
    {
        from = to + pst_Q->u_recsize;

        if (from < pst_Q->p_bottom)
        {
            memcpy(to, from, pst_Q->p_bottom - from);
        }
        else
            pst_Q->p_top = pst_Q->p_bottom = pst_Q->p_data;

        b_result = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      _soft_realloc
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PRECQUEUE   pst_Q --
// RESULT
//      BOOL --
// ***************************************************************************
static BOOL _soft_realloc(PRECQUEUE       pst_Q)
{
    BOOL            b_result        = false;
    UINT            u_diff;
    UINT            u_size;

    ENTER(true);

    if ((u_size = pst_Q->p_bottom - pst_Q->p_top) > 0)
    {
        if ((u_diff = pst_Q->p_top - pst_Q->p_data) > 0)
        {
            memmove(pst_Q->p_data, pst_Q->p_top, u_size);

            pst_Q->p_top    = pst_Q->p_data;
            pst_Q->p_bottom = pst_Q->p_data + u_size;
            b_result        = true;
        }
    }
    else
    {
        pst_Q->p_top = pst_Q->p_bottom = pst_Q->p_data;
        b_result     = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      _realloc
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PRECQUEUE   pst_Q --
// RESULT
//      BOOL --
// ***************************************************************************
static BOOL _realloc(PRECQUEUE pst_Q)
{
    BOOL            b_result    = false;
    UINT            u_count;
    PU8           p_new;
    UINT            u_size;

    ENTER(true);

    u_count = COUNT(pst_Q);

    if (u_count < pst_Q->u_maxrecs)
    {
        u_size = u_count;

        if ((u_count = ((pst_Q->u_maxrecs - u_count) >> 1)) != 0)
        {
            u_size += u_count;
        }
        else
            u_size++;

        u_size *= pst_Q->u_recsize;

        if ((p_new = REALLOC(pst_Q->p_data, u_size)) != NULL)
        {
            pst_Q->p_top    = p_new + (pst_Q->p_top     - pst_Q->p_data);
            pst_Q->p_bottom = p_new + (pst_Q->p_bottom  - pst_Q->p_data);
            pst_Q->p_end    = p_new + u_size;
            pst_Q->p_data   = p_new;

            b_result        = true;
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      la6_recqueue_rec_add
// PURPOSE
//      Add Record to Queue
// PARAMETERS
//      HRECQUEUE pst_Q -- Pointer to Record Queue control structure
//      PVOID        p_data    -- Pointer to Recodr data to add
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
BOOL recqueue_add(HRECQUEUE     h_queue,
                  PVOID         p_data)
{
    BOOL            b_result    = false;
    PRECQUEUE       pst_Q       = (PRECQUEUE)h_queue;

    ENTER(pst_Q);

    if (  (pst_Q->p_bottom < pst_Q->p_end)      ||
          _soft_realloc(pst_Q)                  ||
          (pst_Q->u_maxrecs && _realloc(pst_Q)) )
    {
        memcpy(pst_Q->p_bottom, p_data, pst_Q->u_recsize);
        pst_Q->p_bottom += pst_Q->u_recsize;
        b_result         = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      recqueue_get_rollback_point
// PURPOSE
//
// PARAMETERS
//      HRECQUEUE h_queue --
// RESULT
//      UINT --
// ***************************************************************************
UINT recqueue_get_rollback_point(HRECQUEUE     h_queue)
{
    UINT            point       = 0;
    PRECQUEUE       pst_Q       = (PRECQUEUE)h_queue;

    ENTER(pst_Q);

    point = pst_Q->p_bottom - pst_Q->p_top;

    RETURN(point);
}
// ***************************************************************************
// FUNCTION
//      recqueue_rollback
// PURPOSE
//
// PARAMETERS
//      HRECQUEUE h_queue --
//      UINT      point   --
// RESULT
//      void --
// ***************************************************************************
void recqueue_rollback(HRECQUEUE    h_queue,
                       UINT         point)
{
    PRECQUEUE       pst_Q       = (PRECQUEUE)h_queue;

    ENTER(pst_Q);

    pst_Q->p_bottom = pst_Q->p_top + point;

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      recqueue_raw_get
// PURPOSE
//
// PARAMETERS
//      HRECQUEUE     h_queue  --
//      PA7DATAX   pst_data --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL recqueue_raw_get(HRECQUEUE     h_queue,
                      PAXDATAX      pst_data)
{
    BOOL            b_result    = false;
    PRECQUEUE       pst_Q       = (PRECQUEUE)h_queue;
    UINT            u_count;

    ENTER(pst_Q && pst_data);

    if ((u_count = COUNT(pst_Q)) > 0)
    {
        pst_data->p_data    = pst_Q->p_top;
        pst_data->u_size    = pst_Q->u_recsize * u_count;
        b_result            = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      recqueue_raw_set
// PURPOSE
//
// PARAMETERS
//      HRECQUEUE     h_queue  --
//      PA7DATAX   pst_data --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL recqueue_raw_set(HRECQUEUE     h_queue,
                      PAXDATAX      pst_data)
{
    BOOL            b_result    = false;
    PRECQUEUE       pst_Q       = (PRECQUEUE)h_queue;

    ENTER(pst_Q && pst_data);

    if ( !( (pst_data->u_size % pst_Q->u_recsize)                  ||
            (pst_data->u_size > ((unsigned)(pst_Q->p_end - pst_Q->p_data))) ))
    {
        pst_Q->p_top    = pst_Q->p_data;
        pst_Q->p_bottom = pst_Q->p_top + pst_data->u_size;

        memcpy(pst_Q->p_top, pst_data->p_data, pst_data->u_size);

        b_result            = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//
// PURPOSE
//
// PARAMETERS
//      UINT      version      --
//      HRECQUEUE h_queue      --
//      PFNRECSER pfn_ser      --
//      PCSTR     psz_filename --
// RESULT
//      BOOL recqueue_save --
// ***************************************************************************
BOOL recqueue_save (UINT               version,
                    HRECQUEUE          h_queue,
                    PFNRECSER          pfn_ser,
                    PCSTR              psz_filename)
{
    BOOL            b_result        = false;
    PRECQUEUE       pst_Q           = (PRECQUEUE)h_queue;
    HAXFILE         h_file;
    PU8           p_on;
    FILEHEADER      hdr;
    FILEENTRY       ent;
    PVOID           saveData;

    ENTER(pst_Q);


    hdr.u_version   = version;
    hdr.u_recsize   = pst_Q->u_recsize;
    hdr.u_maxrecs   = pst_Q->u_maxrecs;
    hdr.u_total     = pst_Q->p_bottom - pst_Q->p_top;
    hdr.u_init      = pst_Q->u_init;

    if ((h_file = axfile_open(psz_filename, O_RDWR | O_CREAT)) != NULL)
    {
        hdr.u_checksum  = 0;
        hdr.u_checksum  = crc32_calc(&hdr, sizeof(hdr));

        if (axfile_write(h_file, &hdr, sizeof(hdr)) == sizeof(hdr))
        {
            for (  p_on = pst_Q->p_top, b_result = true;
                   b_result && (p_on < pst_Q->p_bottom);
                   p_on += pst_Q->u_recsize)
            {
                ent.u_size = pst_Q->u_recsize;

                if ((saveData = pfn_ser ? (*pfn_ser)(p_on, &ent.u_size) : p_on) != NULL)
                {
                    b_result = (axfile_write(h_file, &ent, sizeof(ent)) == sizeof(ent))     &&
                               (axfile_write(h_file, saveData, ent.u_size) == ent.u_size)   ;

                    if (saveData != p_on)
                    {
                        FREE(saveData);
                    }
                }
                else
                    b_result = false;
            }
        }

        axfile_close(h_file);

        if (!b_result)
            axfile_remove(psz_filename);
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      recqueue_load
// PURPOSE
//
// PARAMETERS
//      UINT        version      --
//      PCSTR       psz_filename --
//      PFNRECUNSER pfn_unser    --
//      PFNRECCLEAN pfn_clean    --
// RESULT
//      HRECQUEUE --
// ***************************************************************************
HRECQUEUE recqueue_load(UINT               version,
                        PCSTR              psz_filename,
                        PFNRECUNSER        pfn_unser,
                        PFNRECCLEAN        pfn_clean)
{
    HRECQUEUE       h_queue     = NULL;
    PRECQUEUE       pst_Q;
    HAXFILE         h_file;
    FILEHEADER      hdr;
    FILEENTRY       ent;
    PVOID           p_buff;
    UINT            u_tmp;
    PVOID           loadData;
    U32             checksum;

    ENTER(true);

    if ((h_file = axfile_open(psz_filename, O_RDONLY)) != NULL)
    {
        if (axfile_read(h_file, &hdr, sizeof(hdr)) == sizeof(hdr))
        {
            checksum        = hdr.u_checksum;
            hdr.u_checksum  = 0;

            if (  (checksum == crc32_calc(&hdr, sizeof(hdr)))   &&
                  (hdr.u_version == version)                        )
            {
                if ((h_queue = recqueue_create(hdr.u_recsize,
                                               hdr.u_total ? hdr.u_total : hdr.u_init,
                                               hdr.u_maxrecs,
                                               pfn_clean)) != NULL)
                {
                    pst_Q = (PRECQUEUE)h_queue;

                    while (h_queue && (axfile_read(h_file, &ent, sizeof(ent)) == sizeof(ent)))
                    {
                        if ((p_buff = MALLOC(ent.u_size)) != NULL)
                        {
                            u_tmp = ent.u_size;

                            if (  (axfile_read(h_file, p_buff, ent.u_size) == ent.u_size)   &&
                                  ((loadData = pfn_unser ?
                                          (*pfn_unser)(p_buff, &u_tmp) : p_buff) != NULL)   )
                            {
                                memcpy(pst_Q->p_bottom, loadData, pst_Q->u_recsize);
                                pst_Q->p_bottom += pst_Q->u_recsize;

                                if (loadData != p_buff)
                                {
                                    FREE(loadData);
                                }
                            }
                            else
                            {
                                h_queue = recqueue_destroy(h_queue);
                                printf("recqueue load error 1\n");
                            }

                            FREE(p_buff);
                        }
                        else
                        {
                            h_queue = recqueue_destroy(h_queue);
                            printf("recqueue load error 2\n");
                        }
                    }
                }
            }
        }

        axfile_close(h_file);
    }

    RETURN(h_queue);
}
// ***************************************************************************
// FUNCTION
//      recqueue_enum
// PURPOSE
//
// PARAMETERS
//      HRECQUEUE  h_queue  --
//      PFNRECENUM pfn_enum --
//      PVOID      p_ptr    --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL recqueue_enum(HRECQUEUE          h_queue,
                   PFNRECENUM         pfn_enum,
                   PVOID              p_ptr)
{
    BOOL            b_result        = false;
    PRECQUEUE       pst_Q           = (PRECQUEUE)h_queue;
    PU8             p_on;
    UINT            u_idx;

    ENTER(pst_Q && pfn_enum);

    for (  u_idx = 0, p_on = pst_Q->p_top, b_result = true;
           b_result && (p_on < pst_Q->p_bottom);
           p_on += pst_Q->u_recsize, u_idx++)
    {
        b_result = (*pfn_enum)(p_ptr, p_on, pst_Q->u_recsize, u_idx);
    }

    RETURN(b_result);
}

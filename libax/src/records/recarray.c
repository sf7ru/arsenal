// ***************************************************************************
// TITLE
//      Record Array
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: recarray.c,v 1.7 2004/03/24 08:28:26 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************

#include <stdio.h>

#include <stdlib.h>
#include <string.h>

#include <arsenal.h>

#include <axhashing.h>

#include <records.h>

#include <records.h>
#include <pvt_records.h>

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
    UINT            u_total;
    UINT            u_used;
    UINT            u_recsize;
    UINT            u_amount;
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
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      _clean
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PRECARRAY   pst_array --
//      PU8       p_entry   --
// RESULT
//      BOOL --
// ***************************************************************************
static BOOL _clean(PRECARRAY    pst_array,
                   PU8        p_on)
{
    BOOL            b_result        = true;

    ENTER(true);

    if (pst_array->pfn_clean)
    {
        b_result = (*(PFNRECCLEAN2)pst_array->pfn_clean)(p_on, pst_array->p_ptr);
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      _recarray_realloc
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PRECARRAY   pst_array --
//      UINT        u_amount  --
// RESULT
//      BOOL --
// ***************************************************************************
static BOOL _recarray_realloc(PRECARRAY     pst_array,
                              UINT          u_amount)
{
    BOOL    b_result    = false;
    UINT    u_new_total;
    PVOID   p_new_data;

    ENTER(pst_array);

    if (u_amount)
    {
        u_new_total = pst_array->u_total + u_amount;

        if ((p_new_data = REALLOC(pst_array->p_data,
                                  pst_array->u_recsize *
                                  u_new_total)) != NULL)
        {
            pst_array->p_data   = p_new_data;
            pst_array->u_total  = u_new_total;
            b_result            = true;
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      recarray_destroy
// PURPOSE
//
// PARAMETERS
//      HRECARRAY   h_array --
// RESULT
//      HRECARRAY  --
// ***************************************************************************
HRECARRAY recarray_destroy(HRECARRAY h_array)
{
    PRECARRAY       pst_array       = (PRECARRAY)h_array;

    ENTER(pst_array);

    if (recarray_clean(h_array))
    {
        pst_array = (PRECARRAY)recarray_destroy_noclean(h_array);
    }

    RETURN((HRECARRAY)pst_array);
}
// ***************************************************************************
// FUNCTION
//      recarray_destroy_noclean
// PURPOSE
//
// PARAMETERS
//      HRECARRAY h_array --
// RESULT
//      HRECARRAY --
// ***************************************************************************
HRECARRAY recarray_destroy_noclean(HRECARRAY h_array)
{
    PRECARRAY       pst_array       = (PRECARRAY)h_array;

    ENTER(pst_array);

    if (pst_array->p_data)
        FREE(pst_array->p_data);

    FREE(pst_array);

    RETURN(NULL);
}
// ***************************************************************************
// FUNCTION
//      recarray_create
// PURPOSE
//
// PARAMETERS
//      UINT         u_rec_size     --
//      UINT         u_rec_num      --
//      UINT         u_reloc_amount --
//      PFNRECFREE   pfn_free       --
// RESULT
//      HRECARRAY  --
// ***************************************************************************
HRECARRAY recarray_create_x(UINT            u_rec_size,
                            UINT            u_rec_num,
                            UINT            u_reloc_amount,
                            PFNRECCLEAN     pfn_clean,
                            PVOID           p_ptr)
{
    PRECARRAY       pst_array       = NULL;
//    UINT            u_tmp;

    ENTER(u_rec_size && u_rec_num);

/*
#if (defined(DBGSYS) && (TARGET_ARCH == __AX_arm__))

        if (u_rec_size & 3)
        {
            A7TRACE((A7P, "  RECARRAY WARNING!!! structure size if not aligned!  (size = %d)\n", u_rec_size));

            u_tmp = 0;
            u_tmp = 1 / u_tmp;
        }

#endif
*/

    if ((pst_array = CREATE(RECARRAY)) != NULL)
    {
        if ((pst_array->p_data = MALLOC(u_rec_size * u_rec_num)) != NULL)
        {
            pst_array->u_total      = u_rec_num;
            pst_array->u_amount     = u_reloc_amount;
            pst_array->u_recsize    = u_rec_size;
            pst_array->pfn_clean    = pfn_clean;
            pst_array->p_ptr        = p_ptr;
        }
        else
            pst_array = (PRECARRAY)recarray_destroy((HRECARRAY)pst_array);
    }

    RETURN((HRECARRAY)pst_array);
}
// ***************************************************************************
// FUNCTION
//      recarray_add
// PURPOSE
//
// PARAMETERS
//      HRECARRAY   h_array --
//      PVOID       p_data  --
// RESULT
//      UINT  --
// ***************************************************************************
UINT recarray_add(HRECARRAY     h_array,
                  PVOID         p_data)
{
    UINT            u_index         = AXII;
    PRECARRAY       pst_array       = (PRECARRAY)h_array;
    PVOID           p_on;

    ENTER(pst_array);

    if (  (pst_array->u_used < pst_array->u_total)          ||
          _recarray_realloc(pst_array, pst_array->u_amount) )
    {
        u_index = pst_array->u_used++;
        p_on    = GET_RECORD(pst_array, u_index);

        memcpy(p_on, p_data, pst_array->u_recsize);
    }

    RETURN(u_index);
}
// ***************************************************************************
// FUNCTION
//      recarray_add_array
// PURPOSE
//
// PARAMETERS
//      HRECARRAY h_array --
//      PVOID     p_data  --
//      UINT      u_recs  --
// RESULT
//      UINT --
// ***************************************************************************
UINT recarray_add_array(HRECARRAY          h_array,
                        PVOID              p_data,
                        UINT               u_recs)
{
    UINT            u_index         = AXII;
    PRECARRAY       pst_array       = (PRECARRAY)h_array;
    PVOID           p_on;

    ENTER(pst_array);

    if (  ((pst_array->u_used + u_recs) < pst_array->u_total)               ||
          _recarray_realloc(pst_array, MAC_MAX(pst_array->u_amount, u_recs)))
    {
        u_index             = pst_array->u_used;
        pst_array->u_used  += u_recs;
        p_on                = GET_RECORD(pst_array, u_index);

        memcpy(p_on, p_data, pst_array->u_recsize * u_recs);
    }

    RETURN(u_index);
}
// ***************************************************************************
// FUNCTION
//      recarray_count
// PURPOSE
//
// PARAMETERS
//      HRECARRAY   h_array --
// RESULT
//      UINT  --
// ***************************************************************************
UINT recarray_count(HRECARRAY h_array)
{
    UINT            u_cnt           = 0;
    PRECARRAY       pst_array       = (PRECARRAY)h_array;

    ENTER(pst_array);

    u_cnt = pst_array->u_used;

    RETURN(u_cnt);
}
// ***************************************************************************
// FUNCTION
//      recarray_count
// PURPOSE
//
// PARAMETERS
//      HRECARRAY   h_array --
// RESULT
//      UINT  --
// ***************************************************************************
UINT recarray_total(HRECARRAY h_array)
{
    UINT            u_cnt           = 0;
    PRECARRAY       pst_array       = (PRECARRAY)h_array;

    ENTER(pst_array);

    u_cnt = pst_array->u_total;

    RETURN(u_cnt);
}
// ***************************************************************************
// FUNCTION
//      recarray_get
// PURPOSE
//
// PARAMETERS
//      HRECARRAY   h_array --
//      UINT        u_index --
// RESULT
//      PVOID  --
// ***************************************************************************
PVOID recarray_get(HRECARRAY    h_array,
                   UINT         u_index)
{
    PVOID           p_data          = NULL;
    PRECARRAY       pst_array       = (PRECARRAY)h_array;

    ENTER(pst_array);

    p_data = GET_RECORD(pst_array, u_index);

    RETURN(p_data);
}
// ***************************************************************************
// FUNCTION
//      recarray_ovr
// PURPOSE
//
// PARAMETERS
//      HRECARRAY   h_array --
//      UINT        u_index --
//      PVOID       p_data  --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL recarray_ovr(HRECARRAY     h_array,
                  UINT          u_index,
                  PVOID         p_data)
{
    BOOL            b_result        = false;
    PRECARRAY       pst_array       = (PRECARRAY)h_array;
    PU8           p_on;

    ENTER(pst_array);

    if ((p_on = GET_RECORD(pst_array, u_index)) != NULL)
    {
        memcpy(p_on, p_data, pst_array->u_recsize);

        b_result = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      recarray_del
// PURPOSE
//
// PARAMETERS
//      HRECARRAY   h_array --
//      UINT        u_index --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL recarray_del(HRECARRAY     h_array,
                  UINT          u_index)
{
    BOOL            b_result        = false;
    PRECARRAY       pst_array       = (PRECARRAY)h_array;
    PU8           p_on;

    ENTER(pst_array);

    if ((p_on = GET_RECORD(pst_array, u_index)) != NULL)
    {
        if (_clean(pst_array, p_on))
        {
            if (u_index != (--pst_array->u_used))
            {
                memmove(p_on,
                        p_on + pst_array->u_recsize,
                        (pst_array->u_used - u_index) * pst_array->u_recsize);
            }

            b_result = true;
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      recarray_get_last
// PURPOSE
//
// PARAMETERS
//      HRECARRAY   h_array --
// RESULT
//      PVOID  --
// ***************************************************************************
PVOID recarray_get_last(HRECARRAY   h_array)
{
    PRECARRAY       pst_array       = (PRECARRAY)h_array;
    PVOID           p_data          = NULL;

    ENTER(pst_array);

    if (pst_array->u_used)
    {
        p_data = (pst_array->p_data + (pst_array->u_recsize * (pst_array->u_used - 1)));
    }

    RETURN(p_data);
}

/*
void                recarray_clean              (HRECARRAY          h_array);


BOOL                recarray_ins                (HRECARRAY          h_array,
                                                 UINT               u_index,
                                                 PVOID              p_data);

BOOL                recarray_ovr                (HRECARRAY          h_array,
                                                 UINT               u_index,
                                                 PVOID              p_data);


PVOID               recarray_get_last           (HRECARRAY          h_array);

UINT                recarray_get_idx            (HRECARRAY          h_array,
                                                 PVOID              p_data);

UINT                recarray_get_size           (HRECARRAY          h_array);

BOOL                recarray_cat                (HRECARRAY          pst_target,
                                                 HRECARRAY          pst_source);

HRECARRAY           recarray_dup                (HRECARRAY          h_array);

HRECARRAY           recarray_conv               (PVOID              p_data,
                                                 UINT               u_size);

PVOID               recarray_freeze             (HRECARRAY          h_array,
                                                 PUINT              pu_used);

BOOL                recarray_minimize           (HRECARRAY          h_array);

HRECARRAY           recarray_load               (PSTR               psz_filename);

BOOL                recarray_save               (HRECARRAY          h_array,
                                                 PSTR               psz_filename);

// ***************************************************************************
// STATIC FUNCTION
//      _la6_realloc_recarray
// PURPOSE
//      Reallocate memory for Array
// PARAMETERS
//      HRECARRAY pst_array -- Pointer to Record Array control structure
//      DWORD        d_new_amount -- Amount of new records to add
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
static BOOL _la6_realloc_recarray(HRECARRAY pst_array, DWORD d_new_amount)
{
    BOOL    b_result    = false;
    PVOID   p_new_data;

    ENTER(pst_array && d_new_amount);

    if ((p_new_data = REALLOC_D(pst_array->p_data,
                                 pst_array->u_recsize *
                                 (pst_array->u_total  +
                                  d_new_amount), NULL)) != NULL)
    {
        pst_array->u_total += d_new_amount;
        pst_array->p_data       = p_new_data;
        b_result                   = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      la6_recarray_destroy
// PURPOSE
//      Destroy the Record Array and its control structure
// PARAMETERS
//      HRECARRAY pst_array -- Pointer to Record Array control structure
// RESULT
//      HRECARRAY    -- Always NULL
// ***************************************************************************
HRECARRAY la6_recarray_destroy(HRECARRAY pst_array)
{
    if (pst_array)
    {
        if (pst_array->p_data)
            FREE(pst_array->p_data);

        FREE(pst_array);
    }

    return NULL;
}

// ***************************************************************************
// FUNCTION
//      la6_recarray_create
// PURPOSE
//      Create a Record Array
// PARAMETERS
//      DWORD        d_size    -- Size of Record
//      DWORD        d_initial -- Initial count of records to allocate
// RESULT
//      HRECARRAY    -- Pointer to the just created Record Array control
//                         structure if all is ok or NULL if not
// ***************************************************************************
HRECARRAY la6_recarray_create(DWORD d_size, DWORD d_initial)
{
    HRECARRAY pst_array = NULL;

    if (d_size && d_initial)
    {
        if ((pst_array = MALLOC_D(sizeof(LA6RECARRAY), "LA6RECARRAY")) != NULL)
        {
            memset(pst_array, 0 , sizeof(LA6RECARRAY));

            if ((pst_array->p_data = MALLOC_D(d_size * d_initial, "RECARRAY data")) != NULL)
            {
                pst_array->u_total      = d_initial;
                pst_array->u_amount   = d_initial;
                pst_array->u_recsize    = d_size;
            }
            else
                pst_array = la6_recarray_destroy(pst_array);
        }
    }

    return pst_array;
}
// ***************************************************************************
// FUNCTION
//      la6_recarray_minimize
// PURPOSE
//      Free data unwanted by Array
// PARAMETERS
//      HRECARRAY pst_array -- Pointer to Record Array control structure
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
BOOL la6_recarray_minimize(HRECARRAY pst_array)
{
    BOOL    b_result            = false;
    PVOID   p_new;
    DWORD   d_tmp_allocated;

    if (pst_array && pst_array->p_data)
    {
        d_tmp_allocated =
            ( pst_array->u_used  &&
              (pst_array->u_used > pst_array->u_amount)  ?
              pst_array->u_used                                   :
              pst_array->u_amount                              );

        if (  (d_tmp_allocated < pst_array->u_total)                         &&
              ((p_new = REALLOC_D(pst_array->p_data,
                    d_tmp_allocated * pst_array->u_recsize, NULL)) != NULL)  )
        {
            pst_array->p_data      = p_new;
            pst_array->u_total = d_tmp_allocated;
            b_result                  = true;
        }
    }

    return  b_result;
}

// ***************************************************************************
// FUNCTION
//      la6_recarray_rec_add
// PURPOSE
//      Add Record to Array
// PARAMETERS
//      HRECARRAY pst_array -- Pointer to Record Array control structure
//      PVOID        p_data       -- Pointer to record data
// RESULT
//      DWORD
//          AXII:     Error while adding
//          Other:              Index of added record
// ***************************************************************************
DWORD la6_recarray_rec_add(HRECARRAY pst_array, PVOID p_data)
{
    DWORD   d_index = AXII;

    if (pst_array && pst_array->p_data && p_data)
    {
        // If we have enough space for this
        if ( (pst_array->u_used < pst_array->u_total)              ||
             _la6_realloc_recarray(pst_array, pst_array->u_amount)  )
        {
            d_index = pst_array->u_used++;
            memcpy(pst_array->p_data + (d_index * pst_array->u_recsize),
                   p_data,
                   pst_array->u_recsize);
        }
    }

    return d_index;
}

// ***************************************************************************
// FUNCTION
//      la6_recarray_rec_del
// PURPOSE
//      Delete Record in Array
// PARAMETERS
//      HRECARRAY pst_array -- Pointer to Record Array control structure
//      DWORD        d_index      -- Index of record
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
BOOL la6_recarray_rec_del(HRECARRAY pst_array, DWORD d_index)
{
    BOOL    b_result        = false;

    if (pst_array && pst_array->p_data && (d_index < pst_array->u_used))
    {
        // If the record is not last
        if (d_index != (--pst_array->u_used))
            memcpy(pst_array->p_data + (d_index       * pst_array->u_recsize),
                   pst_array->p_data + ((d_index + 1) * pst_array->u_recsize),
                   (pst_array->u_used - d_index)   * pst_array->u_recsize);

        b_result = true;
    }

    return b_result;
}

// ***************************************************************************
// FUNCTION
//      la6_recarray_rec_ins
// PURPOSE
//      Insert Record into the Record Array
// PARAMETERS
//      HRECARRAY pst_array -- Pointer to Record Array control structure
//      DWORD        d_index      -- Index of record
//      PVOID        p_data       -- Pointer to Record data to insert
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// NOTE
//      You can insert only between existing records or at start
//      or end of recordset
// ***************************************************************************
BOOL la6_recarray_rec_ins(HRECARRAY pst_array, DWORD d_index, PVOID p_data)
{
    BOOL    b_result        = false;
    PU8   p_curr_data;

    if (pst_array && pst_array->p_data && (d_index <= pst_array->u_used))
    {
        // If we have enough space for this
        if ( ((pst_array->u_used + 1) <= pst_array->u_total)       ||
             _la6_realloc_recarray(pst_array, pst_array->u_amount)  )
        {
            // If the record become the last
            if (d_index == pst_array->u_used)
            {
                if (la6_recarray_rec_add(pst_array, p_data) != AXII)
                    b_result = true;
            }
            else
            {
                p_curr_data = pst_array->p_data + (d_index * pst_array->u_recsize);

                memmove(p_curr_data + pst_array->u_recsize,
                        p_curr_data,
                        pst_array->u_recsize * (pst_array->u_used - d_index));
                memcpy( p_curr_data,
                        p_data,
                        pst_array->u_recsize);

                pst_array->u_used++;
                b_result = true;
            }
        }
    }

    return b_result;
}

// ***************************************************************************
// FUNCTION
//      la6_recarray_rec_get_ptr
// PURPOSE
//      Retrieve pointer to Record data
// PARAMETERS
//      HRECARRAY pst_array -- Pointer to Record Array control structure
//      DWORD        d_index      -- Index of Record
// RESULT
//      PVOID   -- Pointer to Record data if exist or NULL if not
// ***************************************************************************
PVOID la6_recarray_rec_get_ptr(HRECARRAY pst_array, DWORD d_index)
{
    PVOID   p_data = NULL;

    if (pst_array && pst_array->p_data && (d_index < pst_array->u_used))
        p_data = pst_array->p_data + (pst_array->u_recsize * d_index);

    return p_data;
}
// ***************************************************************************
// FUNCTION
//      la6_recarray_rec_get_idx
// PURPOSE
//      Retrieve Record index by pointer to Record
// PARAMETERS
//      HRECARRAY pst_array -- Pointer to Record Array control structure
//      PVOID        p_data       -- Pointer to data inside the Records Array
// RESULT
//      DWORD
//          AXII:     Record not exist or Pointer is wrong
//          Other:              Index of record
// ***************************************************************************
DWORD la6_recarray_rec_get_idx(HRECARRAY pst_array, PVOID p_data)
{
    DWORD   d_index = AXII;
    div_t   st_div;

    if (pst_array && p_data && pst_array->p_data)
    {
        st_div = div(((PU8)p_data) - pst_array->p_data, pst_array->u_recsize);
        if (!st_div.rem)
            d_index = st_div.quot;
    }

    return d_index;
}

// ***************************************************************************
// FUNCTION
//      la6_recarray_count
// PURPOSE
//      Retrieve number of records in Array
// PARAMETERS
//      HRECARRAY pst_array -- Pointer to Record Array control structure
// RESULT
//      DWORD   -- Number of Records in Array
// ***************************************************************************
DWORD la6_recarray_count(HRECARRAY pst_array)
{
    DWORD   d_num = 0;

    if (pst_array)
        d_num = pst_array->u_used;

    return d_num;
}
*/
// ***************************************************************************
// FUNCTION
//      recarray_recsize
// PURPOSE
//
// PARAMETERS
//      HRECARRAY h_array --
// RESULT
//      UINT --
// ***************************************************************************
UINT recarray_recsize(HRECARRAY h_array)
{
    UINT            result          = 0;
    PRECARRAY       pst_array       = (PRECARRAY)h_array;

    ENTER(true);

    if (pst_array)
        result = pst_array->u_recsize;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      la6_recarray_dup
// PURPOSE
//      Dublicate RecArray
// PARAMETERS
//      HRECARRAY pst_array -- Exist RecArray
// RESULT
//      HRECARRAY    -- Pointer to the new created Record Array control
//                         structure if all is ok or NULL if not
// ***************************************************************************
HRECARRAY recarray_dup(HRECARRAY h_array)
{
    PRECARRAY           pst_new     = NULL;
    PRECARRAY           pst_array   = (PRECARRAY)h_array;

    ENTER(pst_array);

    if ((pst_new = (PRECARRAY)recarray_create_x(pst_array->u_recsize,
                                        pst_array->u_total,
                                        pst_array->u_amount,
                                        pst_array->pfn_clean,
                                        pst_array->p_ptr)) != NULL)
    {
        if (pst_array->u_used)
        {
            pst_new->u_used = pst_array->u_used;

            memcpy(pst_new->p_data, pst_array->p_data,
                        pst_array->u_used * pst_array->u_recsize);
        }
    }

    RETURN((HRECARRAY)pst_new);
}
// ***************************************************************************
// FUNCTION
//      la6_recarray_conv
// PURPOSE
//      Convert raw data to Record Array
// PARAMETERS
//      DWORD        d_size -- Size of Record
//      PVOID        p_data -- Pointer to data
//      UINT         u_size -- Size of data
// RESULT
//      HRECARRAY    -- Pointer to the just created Record Array control
//                         structure if all is ok or NULL if not
// ***************************************************************************
HRECARRAY recarray_conv(UINT    u_recsize,
                        PVOID   p_data,
                        UINT    u_size)
{
    PRECARRAY       pst_array = NULL;

    ENTER(u_recsize && p_data && u_size);

    if ((pst_array = CREATE(RECARRAY)) != NULL)
    {
        if ((u_size >= u_recsize) && (!(u_size % u_recsize)))
        {
            pst_array->p_data           = p_data;
            pst_array->u_total          = u_size / u_recsize;
            pst_array->u_used           = pst_array->u_total;
            pst_array->u_amount         = 0;
            pst_array->u_recsize        = u_recsize;
        }
        else
            pst_array = (PRECARRAY)recarray_destroy((HRECARRAY)pst_array);
    }

    RETURN((HRECARRAY)pst_array);
}
// ***************************************************************************
// FUNCTION
//      recarray_clean
// PURPOSE
//
// PARAMETERS
//      HRECARRAY   h_array --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL recarray_clean(HRECARRAY h_array)
{
    BOOL            b_result        = false;
    PRECARRAY       pst_array       = (PRECARRAY)h_array;
    UINT            u_idx;
    PU8           p_on;

    ENTER(pst_array);

    b_result = true;

    if (  pst_array->p_data     &&
          pst_array->pfn_clean  &&
          pst_array->u_used     )
    {
        u_idx = pst_array->u_used - 1;
        p_on  = pst_array->p_data + (pst_array->u_recsize * u_idx);

        do
        {
            if (_clean(pst_array, p_on))
            {
                p_on -= pst_array->u_recsize;
                pst_array->u_used--;
            }


        } while (u_idx--);
    }
    else
        pst_array->u_used = 0;

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      recarray_cat
// PURPOSE
//
// PARAMETERS
//      HRECARRAY   h_tgt --
//      HRECARRAY   h_src --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL recarray_cat(HRECARRAY     h_tgt,
                  HRECARRAY     h_src)
{
    BOOL            b_result    = false;
    PRECARRAY       pst_tgt     = (PRECARRAY)h_tgt;
    PRECARRAY       pst_src     = (PRECARRAY)h_src;

    ENTER(pst_tgt && pst_src);

    if (  (pst_tgt->u_recsize == pst_src->u_recsize)    &&
          (pst_src->u_used)                             )
    {
        if (  ((pst_tgt->u_total -
                    pst_tgt->u_used) > pst_src->u_used) ||
              _recarray_realloc(pst_tgt,
                    pst_src->u_used)                    )
        {
            memcpy(  pst_tgt->p_data + (pst_tgt->u_used *
                                pst_tgt->u_recsize),
                     pst_src->p_data, (pst_src->u_used *
                                pst_src->u_recsize));

            pst_tgt->u_used += pst_src->u_used;
            b_result         = true;
        }
    }

    RETURN(b_result);
}
/*

// ***************************************************************************
// FUNCTION
//      recarray_load
// PURPOSE
//      Load the stored Record Array into the memory
// PARAMETERS
//      PSTR         psz_filename -- Pointer to file name and path
// RESULT
//      HRECARRAY    -- Pointer to the just created Record Array control
//                         structure if all is ok or NULL if not
// ***************************************************************************
HRECARRAY recarray_load_raw(PCSTR         psz_filename,
                            UINT          u_rec_size,
                            UINT          u_reloc_amount)
{
    PRECARRAY       pst_array    = NULL;
    UINT            size;
    PVOID           data;
    ldiv_t          res;

    ENTER(psz_filename);

    if ((data = la6_file_load((PSTR)psz_filename, &size)) != NULL)
    {
        res = ldiv(size, u_rec_size);

        if (res.quot && !res.rem)
        {
            if ((pst_array = CREATE(RECARRAY)) != NULL)
            {
                pst_array->p_data       = data;
                pst_array->u_total      = res.quot;
                pst_array->u_used       = res.quot;
                pst_array->u_amount     = u_reloc_amount;
                pst_array->u_recsize    = u_rec_size;
            }
            else
                FREE(data);
        }
        else
            FREE(data);
    }

    RETURN((HRECARRAY)pst_array);
}
*/
// ***************************************************************************
// FUNCTION
//      recarray_serialize
// PURPOSE
//
// PARAMETERS
//      HRECARRAY h_array --
//      PFNRECSER pfn_ser --
//      PUINT     pu_size --
// RESULT
//      PVOID --
// ***************************************************************************
PVOID recarray_serialize(HRECARRAY          h_array,
                         PFNRECSER          pfn_ser,
                         PUINT              pu_size)
{
    PU8               serialized  = NULL;
    PRECARRAY           pst_A       = (PRECARRAY)h_array;
    PFILEHEADER         hdr;
    PU8               onS;
    UINT                onT;
    UINT                idx;
    UINT                u_size;
    PVOID               tmp;
    UINT                newSize;
    UINT                saveSize;
    PVOID               saveData;
    PFILEENTRY          ent;

    ENTER(pst_A && pu_size);

    u_size  = sizeof(FILEHEADER) + (pst_A->u_used * (pst_A->u_recsize + sizeof(FILEENTRY)));

    if ((serialized = (PU8)MALLOC(u_size)) != NULL)
    {
        hdr             = (PFILEHEADER)serialized;
        hdr->u_checksum = 0;
        hdr->u_amount   = pst_A->u_amount;
        hdr->u_recsize  = pst_A->u_recsize;
        hdr->u_total    = pst_A->u_total;
        hdr->u_used     = pst_A->u_used;

        onS             = (PU8)pst_A->p_data;
        onT             = sizeof(FILEHEADER);

        for (  idx = 0;
               serialized && (idx < pst_A->u_used);
               idx++, onS += pst_A->u_recsize)
        {
            saveSize = pst_A->u_recsize;

            if ((saveData = pfn_ser ? (*pfn_ser)(onS, &saveSize) : onS) != NULL)
            {
                if (saveSize > pst_A->u_recsize)
                {
                    newSize  = u_size + (saveSize - pst_A->u_recsize);

                    if ((tmp = REALLOC(serialized, newSize)) != NULL)
                    {
                        u_size      = newSize;
                        serialized  = tmp;
                    }
                    else
                    {
                        FREE(serialized);
                        serialized = NULL;
                    }
                }

                if (serialized)
                {
                    ent         = (PFILEENTRY)(serialized + onT);
                    ent->u_size = saveSize;
                    onT         += (sizeof(FILEENTRY) + saveSize);

                    memcpy(ent + 1, saveData, saveSize);
                }

                if (saveData != onS)
                {
                    FREE(saveData);
                }
            }
            else
            {
                FREE(serialized);
                serialized = NULL;
            }
        }

        *pu_size = u_size;
    }

    RETURN((PVOID)serialized);
}
// ***************************************************************************
// FUNCTION
//      recarray_unserialize_x
// PURPOSE
//
// PARAMETERS
//      PVOID       p_serialized --
//      PFNRECUNSER pfn_unser    --
//      UINT        u_size       --
//      PFNRECCLEAN pfn_clean    --
//      PVOID       p_ptr        --
// RESULT
//      HRECARRAY --
// ***************************************************************************
HRECARRAY recarray_unserialize_x(PVOID              p_serialized,
                                 PFNRECUNSER        pfn_unser,
                                 UINT               u_size,
                                 PFNRECCLEAN        pfn_clean,
                                 PVOID              p_ptr)
{
    HRECARRAY           result          = NULL;
    PRECARRAY           pst_A;
    PFILEHEADER         hdr;
    PU8               onS;
    PU8               onT;
    PFILEENTRY          ent;
    PVOID               loadData;
    UINT                loadSize;
    UINT                left;

    ENTER(p_serialized && u_size);

    if (u_size >= sizeof(FILEHEADER))
    {
        hdr  = (PFILEHEADER)p_serialized;
        left = u_size - sizeof(FILEHEADER);

        if ((result = recarray_create_x(hdr->u_recsize,
                                        hdr->u_total,
                                        hdr->u_amount,
                                        pfn_clean,
                                        p_ptr)) != NULL)
        {
            if (hdr->u_used)
            {
                pst_A           = (PRECARRAY)result;
                onS             = (PU8)(hdr + 1);
                onT             = (PU8)pst_A->p_data;

                for (; result && left; onT += hdr->u_recsize)
                {
                    if (left > sizeof(FILEENTRY))
                    {
                        ent         = (PFILEENTRY)onS;
                        loadSize    = ent->u_size;
                        onS        += sizeof(FILEENTRY);
                        left       -= sizeof(FILEENTRY);

                        if (  (left >= ent->u_size)                             &&
                              ((loadData = pfn_unser ?
                                  (*pfn_unser)(onS, &loadSize) : onS) != NULL)  )
                        {
                            if (loadSize == hdr->u_recsize)
                            {
                                memcpy(onT, loadData, hdr->u_recsize);

                                pst_A->u_used++;
                            }
                            else
                                result = recarray_destroy(result);

                            if (loadData != onS)
                            {
                                FREE(loadData);
                            }

                            onS  += ent->u_size;
                            left -= ent->u_size;
                        }
                        else
                            result = recarray_destroy(result);
                    }
                    else
                        result = recarray_destroy(result);
                }
            }
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      recarray_save
// PURPOSE
//
// PARAMETERS
//      HRECARRAY pst_array    --
//      UINT      version      --
//      PSTR      psz_filename --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL recarray_save(HRECARRAY    h_array,
                   PCSTR        psz_filename)
{
    BOOL            result          = false;
    PRECARRAY       array           = (PRECARRAY)h_array;
    FILE *          outfile;
    FILEHEADER      fh;
    UINT            size;


    ENTER(array && psz_filename);

    if ((outfile = fopen(psz_filename, "w+b")) != NULL)
    {
        size = array->u_used * array->u_recsize;

        memset(&fh, 0, sizeof(fh));
        fh.u_total      = array->u_total;
        fh.u_used       = array->u_used;
        fh.u_recsize    = array->u_recsize;
        fh.u_amount     = array->u_amount;
        fh.u_checksum   = crc32_calc(array->p_data, size);

        if ( (fwrite(&fh,   sizeof(FILEHEADER), 1, outfile) == 1)   &&
             (fwrite(array->p_data, size,       1, outfile) == 1)   )
        {
            result = true;
        }

        fclose(outfile);
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      recarray_load
// PURPOSE
//
// PARAMETERS
//      UINT version      --
//      PSTR psz_filename --
// RESULT
//      HRECARRAY --
// ***************************************************************************
HRECARRAY recarray_load(PCSTR       psz_filename)
{
    PRECARRAY       result       = NULL;
    FILE *          infile;
    UINT            tSize;
    UINT            uSize;

    FILEHEADER      fh;

    ENTER(psz_filename);

    if ((infile = fopen(psz_filename, "rb")) != NULL)
    {
        if (fread(&fh, sizeof(FILEHEADER), 1, infile) == 1)
        {
            if ((result = CREATE(RECARRAY)) != NULL)
            {
                tSize = fh.u_recsize * fh.u_total;
                uSize = fh.u_recsize * fh.u_used;

                if ( (tSize >= uSize)                                           &&
                     ((result->p_data = MALLOC(tSize)) != NULL)                 &&
                     (fread(result->p_data, uSize, 1, infile) == 1)             &&
                     (crc32_calc(result->p_data, uSize) == fh.u_checksum)   )
                {
                    result->u_total     = fh.u_total;
                    result->u_used      = fh.u_used;
                    result->u_recsize   = fh.u_recsize;
                    result->u_amount    = fh.u_amount;
                }
                else
                    result = (PRECARRAY)recarray_destroy((HRECARRAY)result);
            }
        }

        fclose(infile);
    }

    RETURN((HRECARRAY)result);
}
// ***************************************************************************
// FUNCTION
//      recarray_data_size
// PURPOSE
//
// PARAMETERS
//      HRECARRAY h_array --
// RESULT
//      UINT --
// ***************************************************************************
UINT recarray_data_size(HRECARRAY          h_array)
{
    UINT            u_size          = 0;
    PRECARRAY       pst_array       = (PRECARRAY)h_array;

    ENTER(pst_array);

    u_size = pst_array->u_used * pst_array->u_recsize;

    RETURN(u_size);
}
// ***************************************************************************
// FUNCTION
//      recarray_data_copy
// PURPOSE
//
// PARAMETERS
//      HRECARRAY h_array --
//      PVOID     data    --
//      UINT      first   --
//      UINT      count   --
// RESULT
//      UINT --
// ***************************************************************************
UINT recarray_data_copy(HRECARRAY          h_array,
                        PVOID              data,
                        UINT               first,
                        UINT               count)
{
    UINT            u_size          = 0;
    PU8             on;
    PRECARRAY       pst_array       = (PRECARRAY)h_array;

    ENTER(pst_array);

    if (first < pst_array->u_used)
    {
        if ((first + count) > pst_array->u_used)
        {
            count = pst_array->u_used - first;
        }

        if ((u_size = count * pst_array->u_recsize) > 0)
        {
            on = pst_array->p_data + (first * pst_array->u_recsize);

            memcpy(data, on, u_size);
        }
    }

    RETURN(u_size);
}
// ***************************************************************************
// FUNCTION
//      recarray_data_get
// PURPOSE
//
// PARAMETERS
//      HRECARRAY h_array --
// RESULT
//      PVOID --
// ***************************************************************************
PVOID recarray_data_get(HRECARRAY          h_array)
{
    PVOID           result          = nil;
    PRECARRAY       pst_array       = (PRECARRAY)h_array;

    ENTER(true);

    result = pst_array->p_data;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      la6_recarray_freeze
// PURPOSE
//
// PARAMETERS
//      HRECARRAY pst_array --
//      PDWORD       pu_used   -- Pointer to DWORD to which the number of
//                                   records will be stored
// RESULT
//      PVOID   -- Pointer to records plain data
// ***************************************************************************
// ***************************************************************************
// FUNCTION
//      recarray_freeze
// PURPOSE
//      Free control structure and return pure data array
// PARAMETERS
//      HRECARRAY h_array -- Pointer to Rec Array control structure
//      PUINT     pu_used -- Pointer to variable which will be contained
//                           number of actual records in array
// RESULT
//      PVOID --
// ***************************************************************************
PVOID recarray_freeze(HRECARRAY h_array,
                      PUINT     pu_used)
{
    PVOID           p_ret       = NULL;
    UINT            u_used      = 0;
    PRECARRAY       pst_array   = (PRECARRAY)h_array;

    ENTER(true);

    if (pst_array->p_data)
    {
        if (pst_array->u_used)
        {
            if ((p_ret = REALLOC(pst_array->p_data,
                    pst_array->u_used * pst_array->u_recsize)) == NULL)
            {
                p_ret = pst_array->p_data;
            }

            u_used = pst_array->u_used;
        }
        else
        {
            FREE(pst_array->p_data);
        }
    }

    FREE(pst_array);

    if (pu_used)
    {
        *pu_used = u_used;
    }

    RETURN(p_ret);
}

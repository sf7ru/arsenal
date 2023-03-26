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

#include <stdlib.h>
#include <string.h>

#include <arsenal.h>

#include <records.h>
#include <pvt_records.h>


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// STRUCTURE
//      ENTRY
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_ENTRY
{
    UINT            u_size;
    PVOID           p_data;
} ENTRY, * PENTRY;

// ***************************************************************************
// STRUCTURE
//      FILEHEADER
// ***************************************************************************
typedef struct __tag_FILEHEADER
{
    U32             u_checksum;
    UINT            u_version;
    UINT            u_total;
    UINT            u_initial;
} FILEHEADER, * PFILEHEADER;

// ***************************************************************************
// STRUCTURE
//      FILEENTRY
// ***************************************************************************
typedef struct __tag_FILEENTRY
{
    UINT            u_id;
    UINT            u_size;
} FILEENTRY, * PFILEENTRY;

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------
// ***************************************************************************
// FUNCTION
//      _clean
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PENTRY       pst_entry --
//      PFNRECFREE   pfn_free  --
// RESULT
//      BOOL --
// ***************************************************************************
static BOOL _clean(PENTRY       pst_entry,
                   PFNRECFREE   pfn_free)
{
    BOOL            b_result        = true;

    ENTER(true);

    if (pst_entry->p_data)
    {
        if (pfn_free)
        {
            if ((pst_entry->p_data = (*pfn_free)(pst_entry->p_data)) != NULL)
            {
                b_result = false;
            }
        }
        else
        {
            FREE(pst_entry->p_data);

            pst_entry->p_data = NULL;
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      recset_destroy
// PURPOSE
//
// PARAMETERS
//      HRECSET   h_set --
// RESULT
//      HRECSET  --
// ***************************************************************************
HRECSET recset_destroy(HRECSET h_set)
{
    PRECSET         pst_set     = (PRECSET)h_set;

    ENTER(pst_set);

    if ( !pst_set->h_array                      ||
         !recarray_destroy(pst_set->h_array)    )
    {
        FREE(pst_set);

        pst_set = NULL;
    }

    RETURN((HRECSET)pst_set);
}
// ***************************************************************************
// FUNCTION
//      recset_destroy_nofree
// PURPOSE
//
// PARAMETERS
//      HRECSET h_set --
// RESULT
//      HRECSET --
// ***************************************************************************
HRECSET recset_destroy_nofree(HRECSET h_set)
{
    PRECSET         pst_set     = (PRECSET)h_set;

    ENTER(pst_set);

    if (pst_set->h_array)
        recarray_destroy_noclean(pst_set->h_array);

    FREE(pst_set);

    RETURN(NULL);
}
// ***************************************************************************
// FUNCTION
//      recset_create_x
// PURPOSE
//
// PARAMETERS
//      UINT         u_initial --
//      PFNRECFREE   pfn_free  --
// RESULT
//      HRECSET  --
// ***************************************************************************
HRECSET recset_create_x(UINT          u_initial,
                        UINT          u_reloc,
                        PFNRECFREE    pfn_free)
{
    PRECSET         pst_set = NULL;

    ENTER(u_initial);

    if ((pst_set = CREATE(RECSET)) != NULL)
    {
        if ((pst_set->h_array = recarray_create_x(
            sizeof(ENTRY), u_initial, u_reloc,
            (PFNRECCLEAN)_clean, pfn_free)) != NULL)
        {
            pst_set->pfn_free = pfn_free;
        }
        else
            pst_set = (PRECSET)recset_destroy((HRECSET)pst_set);
    }

    RETURN((HRECSET)pst_set);
}
// ***************************************************************************
// FUNCTION
//      recset_add_fast
// PURPOSE
//
// PARAMETERS
//      HRECSET   h_set  --
//      PVOID     p_data --
//      UINT      u_size --
// RESULT
//      UINT  --
// ***************************************************************************
UINT recset_add_fast(HRECSET    h_set,
                     PVOID      p_data,
                     UINT       u_size)
{
    UINT            u_idx       = AXII;
    PRECSET         pst_set     = (PRECSET)h_set;
    ENTRY           st_rec;

    ENTER(pst_set && p_data);

    st_rec.p_data = p_data;
    st_rec.u_size = u_size;

    u_idx = recarray_add(pst_set->h_array, &st_rec);

    RETURN(u_idx);
}
// ***************************************************************************
// FUNCTION
//      recset_add
// PURPOSE
//
// PARAMETERS
//      HRECSET   h_set  --
//      PVOID     p_data --
//      UINT      u_size --
// RESULT
//      UINT  --
// ***************************************************************************
UINT recset_add(HRECSET     h_set,
                PVOID       p_data,
                UINT        u_size)
{
    UINT            u_result    = AXII;
    PRECSET         pst_set     = (PRECSET)h_set;
    PRECARRAY       pst_array;
    PENTRY          pst_start;
    PENTRY          pst_on;
    PENTRY          pst_end;

    ENTER(pst_set && p_data);

    pst_array   = (PRECARRAY)pst_set->h_array;
    pst_start   = (PENTRY)pst_array->p_data;
    pst_end     = pst_start + pst_array->u_used;

    for (pst_on = pst_start; pst_on < pst_end; pst_on++)
    {
        if (!pst_on->p_data)
        {
            u_result        = pst_on - pst_start;
            pst_on->p_data  = p_data;
            pst_on->u_size  = u_size;
            break;
        }
    }

    if (u_result == AXII)
    {
        u_result = recset_add_fast(h_set, p_data, u_size);
    }

    RETURN(u_result);
}
// ***************************************************************************
// FUNCTION
//      recset_add_copy
// PURPOSE
//
// PARAMETERS
//      HRECSET   h_set  --
//      PVOID     p_data --
//      UINT      u_size --
// RESULT
//      UINT  --
// ***************************************************************************
UINT recset_add_copy(HRECSET     h_set,
                     PVOID       p_data,
                     UINT        u_size)
{
    UINT            u_result    = AXII;
//    PRECSET         pst_set     = (PRECSET)h_set;
    PVOID           p_dup;

    ENTER(pst_set && p_data && u_size);

    if ((p_dup = MALLOC(u_size)) != NULL)
    {
        memcpy(p_dup, p_data, u_size);

        if ((u_result = recset_add(h_set, p_dup, u_size)) == AXII)
        {
            FREE(p_data);
        }
    }

    RETURN(u_result);
}
// ***************************************************************************
// FUNCTION
//      recset_count
// PURPOSE
//
// PARAMETERS
//      HRECSET   h_set --
// RESULT
//      UINT  --
// ***************************************************************************
UINT recset_count(HRECSET h_set)
{
    UINT            u_result    = 0;
    PRECSET         pst_set     = (PRECSET)h_set;
    PRECARRAY       pst_array;
    PENTRY          pst_start;
    PENTRY          pst_on;
    PENTRY          pst_end;

    ENTER(pst_set);

    pst_array   = (PRECARRAY)pst_set->h_array;
    pst_start   = (PENTRY)pst_array->p_data;
    pst_end     = pst_start + pst_array->u_used;

    for (pst_on = pst_start; pst_on < pst_end; pst_on++)
    {
        if (pst_on->p_data)
        {
            u_result++;
        }
    }

    RETURN(u_result);
}
// ***************************************************************************
// FUNCTION
//      recset_del
// PURPOSE
//      Delete record
// PARAMETERS
//      HRECSET   h_set   --
//      UINT      u_index --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL recset_del(HRECSET     h_set,
                UINT        u_index)
{
    BOOL            b_result    = false;
    PRECSET         pst_set     = (PRECSET)h_set;
    PENTRY          pst_rec;

    ENTER(true);

    if ((pst_rec = (PENTRY)recarray_get(pst_set->h_array, u_index)) != NULL)
    {
        b_result = _clean(pst_rec, pst_set->pfn_free);
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      recset_del_nofree
// PURPOSE
//      Delete message without cleaning and freeing
// PARAMETERS
//      HRECSET   h_set   --
//      UINT      u_index --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL recset_del_nofree(HRECSET      h_set,
                       UINT         u_index)
{
    BOOL            b_result    = false;
    PRECSET         pst_set     = (PRECSET)h_set;
    PENTRY          pst_rec;

    ENTER(true);

    if ((pst_rec = (PENTRY)recarray_get(pst_set->h_array, u_index)) != NULL)
    {
        if (pst_rec->p_data)
        {
            pst_rec->p_data = NULL;
            b_result        = true;
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      recset_get
// PURPOSE
//
// PARAMETERS
//      HRECSET   h_set   --
//      UINT      u_index --
// RESULT
//      PVOID  --
// ***************************************************************************
PVOID recset_get_x(HRECSET    h_set,
                   UINT       u_index,
                   PUINT      pu_size)
{
    PVOID           p_data      = NULL;
    PRECSET         pst_set     = (PRECSET)h_set;
    PENTRY          pst_rec;

    ENTER(pst_set);

    if ((pst_rec = (PENTRY)recarray_get(pst_set->h_array, u_index)) != NULL)
    {
        p_data          = pst_rec->p_data;

        if (pu_size)
            *pu_size    = pst_rec->u_size;
    }

    RETURN(p_data);
}
// ***************************************************************************
// FUNCTION
//      recset_enum
// PURPOSE
//
// PARAMETERS
//      HRECSET      h_recset --
//      PFNRECENUM   pfn_enum --
//      PVOID        p_ptr    --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL recset_enum(HRECSET        h_set,
                 PFNRECENUM     pfn_enum,
                 PVOID          p_ptr)
{
    BOOL            b_result        = false;
    PRECSET         pst_set         = (PRECSET)h_set;
    PRECARRAY       pst_array;
    PENTRY          pst_start;
    PENTRY          pst_on;
    PENTRY          pst_end;
    UINT            u_idx           = 0;

    ENTER(pst_set && pfn_enum);

    pst_array   = (PRECARRAY)pst_set->h_array;
    pst_start   = (PENTRY)pst_array->p_data;
    pst_end     = pst_start + pst_array->u_used;

    for (  pst_on = pst_start, b_result = true;
           b_result && (pst_on < pst_end);
           pst_on++, u_idx++)
    {
        if (pst_on->p_data)
        {
            b_result = (*pfn_enum)(p_ptr, pst_on->p_data, pst_on->u_size, u_idx);
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      recset_find
// PURPOSE
//
// PARAMETERS
//      HRECSET    h_set    --
//      PFNRECENUM pfn_enum --
//      PVOID      p_ptr    --
// RESULT
//      UINT --
// ***************************************************************************
UINT recset_find(HRECSET            h_set,
                 PFNRECENUM         pfn_enum,
                 PVOID              p_ptr)
{
    UINT            result          = AXII;
    PRECSET         pst_set         = (PRECSET)h_set;
    PRECARRAY       pst_array;
    PENTRY          pst_start;
    PENTRY          pst_on;
    PENTRY          pst_end;
    UINT            u_idx           = 0;

    ENTER(pst_set && pfn_enum);

    pst_array   = (PRECARRAY)pst_set->h_array;
    pst_start   = (PENTRY)pst_array->p_data;
    pst_end     = pst_start + pst_array->u_used;

    for (  pst_on = pst_start;
           (result == AXII) && (pst_on < pst_end);
           pst_on++, u_idx++)
    {
        if (pst_on->p_data)
        {
            if ((*pfn_enum)(p_ptr, pst_on->p_data, pst_on->u_size, u_idx))
            {
                result = u_idx;
            }
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      recset_cat_fast
// PURPOSE
//
// PARAMETERS
//      HRECSET h_set --
//      HRECSET h_add --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL recset_cat_fast_x(HRECSET     h_set,
                       HRECSET     h_add,
                       BOOL        clean)
{
    BOOL            b_result        = false;
    PRECSET         pst_set     = (PRECSET)h_set;
    PRECSET         pst_add     = (PRECSET)h_add;
    PRECARRAY       array;

    ENTER(true);

    if (recarray_cat(pst_set->h_array, pst_add->h_array))
    {
        array = (PRECARRAY)pst_add->h_array;

        if (clean)
            array->u_used   = 0;

        b_result            = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      recset_cat
// PURPOSE
//
// PARAMETERS
//      HRECSET h_set --
//      HRECSET h_add --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL recset_cat_x(HRECSET     h_set,
                  HRECSET     h_add,
                  BOOL        clean)
{
    BOOL            b_result        = false;
    PRECSET         pst_set         = (PRECSET)h_set;
    PRECSET         pst_add         = (PRECSET)h_add;
    PRECARRAY       pst_set_a;
    PRECARRAY       pst_add_a;
    PENTRY          pst_on;
    PENTRY          pst_end;
    UINT            count;

    ENTER(pst_set && pst_add);

    pst_set_a   = (PRECARRAY)pst_set->h_array;
    pst_add_a   = (PRECARRAY)pst_add->h_array;

    if (pst_add_a->u_used)
    {
        pst_on      = (PENTRY)pst_set_a->p_data;
        pst_end     = pst_on + pst_set_a->u_used;

        for (  count = 0;
               (pst_on < pst_end) && (count < pst_add_a->u_used);
               pst_on++)
        {
            if (pst_on->p_data == NULL)
            {
                count++;
            }
            else
                count = 0;
        }

        if (pst_on < pst_end)
        {
            pst_on -= count;

            memcpy(pst_on, pst_add_a->p_data, pst_add_a->u_used * sizeof(ENTRY));

            if (clean)
                pst_add_a->u_used   = 0;

            b_result                = true;
        }
        else
            b_result = recset_cat_fast_x(h_set, h_add, clean);
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      recset_clean
// PURPOSE
//
// PARAMETERS
//      HRECSET h_set --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL recset_clean(HRECSET     h_set)
{
    BOOL            result          = false;
    PRECSET         pst_set         = (PRECSET)h_set;
    PENTRY          pst_rec;
    UINT            u_index         = 0;

    ENTER(pst_set);

    result = true;

    while ((pst_rec = (PENTRY)recarray_get(pst_set->h_array, u_index++)) != NULL)
    {
        if (!_clean(pst_rec, pst_set->pfn_free))
        {
            result = false;
        }
    }

    RETURN(result);
}
/*
// ***************************************************************************
// FUNCTION
//      recset_save
// PURPOSE
//
// PARAMETERS
//      UINT      version      --
//      HRECSET   h_set        --
//      PFNRECSER pfn_ser      --
//      PCSTR     psz_filename --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL recset_save(UINT               version,
                 HRECSET            h_set,
                 PFNRECSER          pfn_ser,
                 PCSTR              psz_filename)
{
    BOOL            b_result        = false;
    PRECSET         pst_set         = (PRECSET)h_set;
    HA7FILE         h_file;
    PRECARRAY       pst_array;
    PENTRY          pst_start;
    PENTRY          pst_on;
    PENTRY          pst_end;
    FILEHEADER      hdr;
    FILEENTRY       ent;
    PVOID           saveData;

    ENTER(pst_set && pfn_ser && psz_filename);

    pst_array       = (PRECARRAY)pst_set->h_array;
    pst_start       = (PENTRY)pst_array->p_data;
    pst_end         = pst_start + pst_array->u_used;

    hdr.u_version   = version;
    hdr.u_total     = pst_array->u_total;
    hdr.u_initial   = pst_array->u_amount;

    if ((h_file = a7file_open(psz_filename, O_RDWR | O_CREAT)) != NULL)
    {
        hdr.u_checksum  = 0;
        hdr.u_checksum  = la6_CRC32_calc(&hdr, sizeof(hdr));

        if (a7file_write(h_file, &hdr, sizeof(hdr)) == sizeof(hdr))
        {
            for (  pst_on = pst_start, b_result = true, ent.u_id = 0;
                   b_result && (pst_on < pst_end);
                   pst_on++, ent.u_id++)
            {
                if (pst_on->p_data)
                {
                    ent.u_size  = pst_on->u_size;

                    if ((saveData = pfn_ser ?
                                    (*pfn_ser)(pst_on->p_data, &ent.u_size) :
                                    pst_on->p_data) != NULL)
                    {
                        b_result = (a7file_write(h_file, &ent, sizeof(ent)) == sizeof(ent))     &&
                                   (a7file_write(h_file, saveData, ent.u_size) == ent.u_size)   ;

                        if (saveData != pst_on->p_data)
                        {
                            FREE(saveData);
                        }
                    }
                    else
                        b_result = false;
                }
            }
        }

        a7file_close(h_file);

        if (!b_result)
            a7file_remove(psz_filename);
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      recset_load
// PURPOSE
//
// PARAMETERS
//      UINT        version      --
//      PCSTR       psz_filename --
//      PFNRECUNSER pfn_unser    --
//      PFNRECFREE  pfn_free     --
// RESULT
//      HRECSET --
// ***************************************************************************
HRECSET recset_load(UINT               version,
                    PCSTR              psz_filename,
                    PFNRECUNSER        pfn_unser,
                    PFNRECFREE         pfn_free)
{
    HRECSET         h_set           = NULL;
    PRECSET         pst_set         = NULL;
    PRECARRAY       pst_array;
    HA7FILE         h_file;
    FILEHEADER      hdr;
    FILEENTRY       ld;
    PENTRY          entry;
    PVOID           p_buff;
    UINT32          checksum;

    ENTER(pfn_unser && psz_filename);

    if ((h_file = a7file_open(psz_filename, O_RDONLY)) != NULL)
    {
        if (a7file_read(h_file, &hdr, sizeof(hdr)) == sizeof(hdr))
        {
            checksum        = hdr.u_checksum;
            hdr.u_checksum  = 0;

            if (  (checksum == la6_CRC32_calc(&hdr, sizeof(hdr)))   &&
                  (hdr.u_version == version)                        )
            {
                if ((h_set = recset_create(hdr.u_total, pfn_free)) != NULL)
                {
                    pst_set             = (PRECSET)h_set;
                    pst_array           = (PRECARRAY)pst_set->h_array;
                    pst_array->u_amount = hdr.u_initial;

                    memset(pst_array->p_data, 0, pst_array->u_recsize * pst_array->u_total);

                    while (h_set && (a7file_read(h_file, &ld, sizeof(ld)) == sizeof(ld)))
                    {
                        if (  (ld.u_id < hdr.u_total)                   &&
                              ((p_buff = MALLOC(ld.u_size)) != NULL)    )
                        {
                            entry           = (PENTRY)pst_array->p_data;
                            entry          += ld.u_id;
                            entry->u_size   = ld.u_size;

                            if (  (a7file_read(h_file, p_buff, ld.u_size) == ld.u_size) &&
                                  ((entry->p_data = pfn_unser ?
                                              (*pfn_unser)(p_buff, &entry->u_size) :
                                              p_buff) != NULL)                          )
                            {
                                if (entry->p_data != p_buff)
                                {
                                    FREE(p_buff);
                                }

                                pst_array->u_used   = (ld.u_id + 1);
                            }
                            else
                            {
                                FREE(p_buff);

                                h_set = recset_destroy(h_set);
                            }
                        }
                        else
                            h_set = recset_destroy(h_set);
                    }
                }
            }
        }

        a7file_close(h_file);
    }


//
//    pst_array       = (PRECARRAY)pst_set->h_array;
//    pst_start       = (PENTRY)pst_array->p_data;
//    pst_end         = pst_start + pst_array->u_used;
//
//    hdr.u_checksum  = 0;
//    hdr.u_initial   = pst_array->u_amount;
//
//    if ((h_file = a7file_open(psz_filename, O_RDWR | O_CREAT)) != NULL)
//    {
//        if (a7file_write(h_file, &hdr, sizeof(hdr)) == sizeof(hdr))
//        {
//            for (  pst_on = pst_start, b_result = true, ent.u_id = 0;
//                   b_result && (pst_on < pst_end);
//                   pst_on++, ent.u_id++)
//            {
//                if (pst_on->p_data)
//                {
//                    ent.u_size  = pst_on->u_size;
//                    if ((saveData = pfn_ser ?
//                                  (*pfn_ser)(pst_on->p_data, &ent.u_size) :
//                                  pst_on->p_data) != NULL)
//                    {
//                        b_result = (a7file_write(h_file, &ent, sizeof(ent)) == sizeof(ent))     &&
//                                   (a7file_write(h_file, saveData, ent.u_size) == ent.u_size)   ;
//
//                        if (saveData != pst_on->p_data)
//                        {
//                            FREE(saveData);
//                        }
//                    }
//                    else
//                        b_result = false;
//                }
//                else
//                {
//                    ent.u_size  = 0;
//                    b_result    = a7file_write(h_file, &ent, sizeof(ent)) == sizeof(ent);
//                }
//            }
//        }
//
//        a7file_close(h_file);
//
//        if (!b_result)
//            a7file_remove(psz_filename);
//    }

    RETURN(h_set);
}
*/
// ***************************************************************************
// FUNCTION
//      recset_get_last
// PURPOSE
//
// PARAMETERS
//      HRECSET h_set --
// RESULT
//      PVOID --
// ***************************************************************************
PVOID recset_get_last(HRECSET            h_set)
{
    PVOID           result      = NULL;
    PRECSET         set         = (PRECSET)h_set;
    PENTRY          entry;
    UINT            idx;

    idx = recarray_count(set->h_array);

    while ((result == NULL) && idx)
    {
        if ((entry = (PENTRY)recarray_get(set->h_array, --idx)) != NULL)
        {
            result = entry->p_data;
        }
    }

    return result;
}

/*
BOOL recset_ovr(HRECSET            h_set,
                UINT               u_index,
                PVOID              p_data,
                UINT               u_size)
{
    BOOL            b_result        = false;
    PVOID           p_tmp;
    PLA6RECSETREC   pst_rec;

    ENTER(pst_set && pst_set->pst_array && p_data && (!b_copy || d_size));

    if ((pst_rec = _get_internal_rec(pst_set, d_index)) != NULL)
    {
        if (b_copy)
        {
            if ((p_tmp = REALLOC_D(pst_rec->p_data, d_size, "RECSET data")) != NULL)
            {
                memcpy(p_tmp, p_data, d_size);

                pst_rec->d_size = d_size;
                pst_rec->p_data = p_tmp;
                b_result        = true;;
            }
        }
        else
        {
            if (pst_rec->p_data)
            {
                FREE(pst_rec->p_data);
            }

            pst_rec->p_data = p_data;
            pst_rec->d_size = d_size;
            b_result        = true;
        }
    }

    RETURN(b_result);
}
*/
/*
BOOL                recset_clean                (HRECSET            h_set);


PVOID               recset_get_last             (HRECSET            h_set);

UINT                recset_get_size             (HRECSET            h_set,
                                                 UINT               u_index);

UINT                recset_add_copy             (HRECSET            h_set,
                                                 PVOID              p_data,
                                                 UINT               u_size);

BOOL                recset_ovr_copy             (HRECSET            h_set,
                                                 UINT               u_index,
                                                 PVOID              p_data,
                                                 UINT               u_size);


// ***************************************************************************
// STATIC FUNCTION
//      _get_internal_rec
// PURPOSE
//      Retrieve pointer to Internal Record Description structure from Array
// PARAMETERS
//      PLA6RECSET    pst_set -- Pointer to the Record Array
//      DWORD         d_index    -- Index of Record
// RESULT
//      PLA6RECSETREC   -- Pointer to Internal Record Description structure
// ***************************************************************************
static PLA6RECSETREC _get_internal_rec(PLA6RECSET pst_set, DWORD d_index)
{
    PLA6RECSETREC pst_rec = NULL;

    if (pst_set && pst_set->pst_array)
        pst_rec = la6_recarray_rec_get_ptr(pst_set->pst_array, d_index);

    return pst_rec;
}

// ***************************************************************************
// FUNCTION
//      la6_recset_clean
// PURPOSE
//
// PARAMETERS
//      PLA6RECSET  pst_set -- Pointer to Record Set control structure
//      PLA6RECFREE pfn_free   -- Pinter to user freeing function
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
BOOL la6_recset_clean_x(PLA6RECSET pst_set, PLA6RECFREE pfn_free)
{
    BOOL                b_result        = false;
    DWORD               d_index         = 0;
    PLA6RECSETREC       pst_rec;

    if (pst_set)
    {
        while ((pst_rec = la6_recarray_rec_get_ptr(pst_set->pst_array, d_index++)) != NULL)
        {
            if (pst_rec->p_data)
            {
                if (pfn_free)
                    (*pfn_free)(pst_rec->p_data);
                else
                    FREE(pst_rec->p_data);
            }

            memset(pst_rec, 0, sizeof(LA6RECSETREC));
        }

        la6_recarray_clean(pst_set->pst_array);
    }

    return b_result;
}

// ***************************************************************************
// FUNCTION
//      la6_recset_destroy_ex
// PURPOSE
//      Destroy Record Set and Control structure
// PARAMETERS
//      PLA6RECSET  pst_set -- Pointer to Record Set control structure
//      PLA6RECFREE pfn_free   -- Pinter to user freeing function
// RESULT
//      PLA6RECSET  -- Always NULL
// ***************************************************************************
PLA6RECSET la6_recset_destroy_ex(PLA6RECSET pst_set, PLA6RECFREE pfn_free)
{
    if (pst_set)
    {
        if (pst_set->pst_array)
        {
            la6_recset_clean_x(pst_set, pfn_free);
            la6_recarray_destroy(pst_set->pst_array);
        }

        FREE(pst_set);
    }

    return NULL;
}

// ***************************************************************************
// FUNCTION
//      la6_recset_create
// PURPOSE
//      Create Record Set
// PARAMETERS
//      DWORD      d_initial -- Initial naumber of records
// RESULT
//      PLA6RECSET  -- Pointer to the just created Record Set control
//                     structure if all is ok or NULL if not
// ***************************************************************************
PLA6RECSET la6_recset_create(DWORD d_initial)
{
    PLA6RECSET pst_set = NULL;

    if (d_initial)
    {
        if ((pst_set = MALLOC_D(sizeof(LA6RECSET), "RECSET")) != NULL)
        {
            memset(pst_set, 0 , sizeof(LA6RECSET));

            if ((pst_set->pst_array = la6_recarray_create(sizeof(LA6RECSETREC), d_initial)) == NULL)
                pst_set = la6_recset_destroy(pst_set);
        }
        else
            pst_set = la6_recset_destroy(pst_set);
    }

    return pst_set;
}

// ***************************************************************************
// FUNCTION
//      la6_recset_count
// PURPOSE
//      Retrieve number of Records in the Set
// PARAMETERS
//      PLA6RECSET pst_set -- Pointer to Record Set control structure
// RESULT
//      DWORD   -- Number of Records in the Set
// ***************************************************************************
DWORD la6_recset_count(PLA6RECSET pst_set)
{
    DWORD           d_cnt   = 0;


    if (pst_set && pst_set->pst_array)
        d_cnt = la6_recarray_get_count(pst_set->pst_array);

    return d_cnt;
}

// ***************************************************************************
// FUNCTION
//      la6_recset_rec_get_ptr
// PURPOSE
//      Retrieve pointer to Record
// PARAMETERS
//      PLA6RECSET pst_set -- Pointer to Record Set control structure
//      DWORD      d_index    -- Index of Record
// RESULT
//      PVOID   -- Pointer to Record data if record is exist or NULL if not
// ***************************************************************************
PVOID la6_recset_rec_get_ptr(PLA6RECSET pst_set, DWORD d_index)
{
    PVOID           p_data  = NULL;
    PLA6RECSETREC   pst_rec;

    if ((pst_rec = _get_internal_rec(pst_set, d_index)) != NULL)
        p_data = pst_rec->p_data;

    return p_data;
}

// ***************************************************************************
// FUNCTION
//      la6_recset_rec_get_type
// PURPOSE
//      Retrieve User's Type of Record
// PARAMETERS
//      PLA6RECSET pst_set -- Pointer to Record Set control structure
//      DWORD      d_index    -- Index of Record
// RESULT
//      WORD    -- Value of User's Type of Record
// ***************************************************************************
WORD la6_recset_rec_get_type(PLA6RECSET pst_set, DWORD d_index)
{
    WORD            w_type  = 0;
    PLA6RECSETREC   pst_rec;

    if ((pst_rec = _get_internal_rec(pst_set, d_index)) != NULL)
    {
        w_type = pst_rec->w_user_type;
    }

    return w_type;
}

// ***************************************************************************
// FUNCTION
//      la6_recset_rec_get_size
// PURPOSE
//      Retrieve size of Record
// PARAMETERS
//      PLA6RECSET pst_set -- Pointer to Record Set control structure
//      DWORD      d_index    -- Index of Record
// RESULT
//      DWORD   -- Size of Record if exist or zero if not
// ***************************************************************************
DWORD la6_recset_rec_get_size(PLA6RECSET pst_set, DWORD d_index)
{
    DWORD           d_size  = 0;
    PLA6RECSETREC   pst_rec;

    if ((pst_rec = _get_internal_rec(pst_set, d_index)) != NULL)
        d_size = pst_rec->d_size;

    return d_size;
}

// ***************************************************************************
// FUNCTION
//      la6_recset_rec_add_ex
// PURPOSE
//      Add Record to the Set
// PARAMETERS
//      PLA6RECSET pst_set  -- Pointer to Record Set control structure
//      WORD       w_user_type -- User's Type of Record
//      PVOID      p_data      -- Pointer to data of Record
//      DWORD      d_size      -- Size of Record
//      DWORD      d_flags     -- Record manipulation options
//          LA6REC_COPY:        Do the copying of Record's data
//          LA6REC_RECURSE:     Use recursion to find deleted record for replacing
// RESULT
//      DWORD
//          AXII:     Record can not be added
//          Other:              Index of Record in Set
// ***************************************************************************
DWORD la6_recset_rec_add_ex(PLA6RECSET      pst_set,
                            WORD            w_user_type,
                            PVOID           p_data,
                            DWORD           d_size,
                            DWORD           d_flags)
{
    DWORD               d_index     = AXII;
    DWORD               d_replace   = AXII;
    DWORD               d_cnt       = 0;
    PLA6RECSETREC       pst_rec;
    LA6RECSETREC        st_rec;

    ENTER(pst_set && pst_set->pst_array &&
                 p_data && (!(d_flags & LA6REC_COPY) || d_size));

    memset(&st_rec, 0, sizeof(st_rec));
    st_rec.w_user_type  = w_user_type;
    st_rec.d_size       = d_size;

// ------------------------------ Recursing ----------------------------------

    if (d_flags & LA6REC_RECURSE)
    {
        while ((pst_rec = la6_recarray_rec_get_ptr(pst_set->pst_array, d_cnt)) != NULL)
        {
            if (!pst_rec->p_data)
            {
                d_replace = d_cnt;
                break;
            }

            d_cnt++;
        }
    }

// ------------------------------ Creating -----------------------------------

    if (d_flags & LA6REC_COPY)
    {
        if ((st_rec.p_data = MALLOC_D(d_size, "RECSET data")) != NULL)
            memcpy(st_rec.p_data, p_data, d_size);
    }
    else
        st_rec.p_data = p_data;

    if (st_rec.p_data)
    {
// ------------------------------- Adding ------------------------------------

        if (d_replace == AXII)
        {
            d_index = la6_recarray_rec_add(pst_set->pst_array, &st_rec);
        }
        else
        {
            if (la6_recarray_rec_ovr(pst_set->pst_array, d_replace, &st_rec))
            {
                d_index = d_replace;
            }
        }

        if (d_index == AXII)
        {
            if (d_flags & LA6REC_COPY)
                FREE(st_rec.p_data);
        }
    }

    RETURN(d_index);
}

// ***************************************************************************
// FUNCTION
//      la6_recset_rec_ovr
// PURPOSE
//      Overwrite Record in the Set
// PARAMETERS
//      PLA6RECSET pst_set  -- Pointer to Record Set control structure
//      DWORD      d_index     -- Index of Record
//      PVOID      p_data      -- Pointer to data of Record
//      DWORD      d_size      -- Size of Record
//      BOOL       b_copy      -- Do the copying of Record's data or just
//                                save given pointer
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
BOOL la6_recset_rec_ovr(PLA6RECSET  pst_set,     DWORD       d_index,
                        PVOID       p_data,         DWORD       d_size,
                        BOOL        b_copy)
{
    BOOL            b_result = false;
    PVOID           p_tmp;
    PLA6RECSETREC   pst_rec;

    ENTER(pst_set && pst_set->pst_array && p_data && (!b_copy || d_size));

    if ((pst_rec = _get_internal_rec(pst_set, d_index)) != NULL)
    {
        if (b_copy)
        {
            if ((p_tmp = REALLOC_D(pst_rec->p_data, d_size, "RECSET data")) != NULL)
            {
                memcpy(p_tmp, p_data, d_size);

                pst_rec->d_size = d_size;
                pst_rec->p_data = p_tmp;
                b_result        = true;;
            }
        }
        else
        {
            if (pst_rec->p_data)
            {
                FREE(pst_rec->p_data);
            }

            pst_rec->p_data = p_data;
            pst_rec->d_size = d_size;
            b_result        = true;
        }
    }

    RETURN(b_result);
}

// ***************************************************************************
// FUNCTION
//      la6_recset_rec_del
// PURPOSE
//      Delete Record from Set
// PARAMETERS
//      PLA6RECSET  pst_set -- Pointer to Record Set control structure
//      DWORD       d_index    -- Index of Record
//      PLA6RECFREE pfn_free   -- Pinter to user freeing function
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
BOOL la6_recset_rec_del_ex(PLA6RECSET       pst_set,
                           DWORD            d_index,
                           PLA6RECFREE      pfn_free)
{
    BOOL            b_result    = false;
    PLA6RECSETREC   pst_rec;

    ENTER(pst_set);

    if ((pst_rec = _get_internal_rec(pst_set, d_index)) != NULL)
    {
        if (pst_rec->p_data)
        {
            if (pfn_free)
                (*pfn_free)(pst_rec->p_data);
            else
                FREE(pst_rec->p_data);
        }

        memset(pst_rec, 0, sizeof(LA6RECSETREC));

        b_result = true;
    }

    RETURN(b_result);
}

*/

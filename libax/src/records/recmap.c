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
#include <axutils.h>
#include <axstring.h>

#include <records.h>
#include <pvt_records.h>


// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define REMAINDER(a)        ((a) % pst_map->u_divider)
#define GET_X_SZ(a)         ((a)->u_id_size > sizeof(UINT) ? (a)->u_id_size - sizeof(UINT) : 0)


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

// ***************************************************************************
// STRUCTURE
//      RECMAPOFFSET
// ***************************************************************************
typedef struct __tag_RECMAPOFFSET
{
    PVOID           p_first;
    PVOID           p_last;
} RECMAPOFFSET, * PRECMAPOFFSET;

// ***************************************************************************
// STRUCTURE
//      RECMAPENTRY
// ***************************************************************************
typedef struct __tag_RECMAPENTRY
{
    PVOID           p_next;
    PVOID           p_data;
    UINT            u_size;
    UINT            u_id;
} RECMAPENTRY, * PRECMAPENTRY;

/*
// ***************************************************************************
// STRUCTURE
//      FILEHEADER
// ***************************************************************************
#pragma pack(1)
typedef AXPACKED(struct) __tag_FILEHEADER
{
    UINT32          u_checksum;
    UINT32          u_version;
    UINT32          u_id_size;
    UINT32          u_divider;
} FILEHEADER, * PFILEHEADER;
#pragma pack()
*/

// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      _recmap_entry_free
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PRECMAP        pst_map   --
//      PRECMAPENTRY   pst_entry --
// RESULT
//      BOOL --
// ***************************************************************************
static BOOL _recmap_entry_free(PRECMAP         pst_map,
                               PRECMAPENTRY    pst_entry)
{
    BOOL            b_result    = true;

    ENTER(true);

    if (pst_entry->p_data)
    {
        if (pst_map->pfn_free)
        {
            if ((pst_entry->p_data = (*pst_map->pfn_free)(pst_entry->p_data)) != NULL)
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
//      _entry_find
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PRECMAP          pst_map   --
//      UINT             u_id      --
//      PRECMAPENTRY *   ppst_prev --
// RESULT
//      PRECMAPENTRY --
// ***************************************************************************
static PRECMAPENTRY _entry_find(PRECMAP           pst_map,
                                PVOID             p_id,
                                PRECMAPENTRY *    ppst_prev)
{
    PRECMAPENTRY    pst_prev    = NULL;
    PRECMAPENTRY    pst_entry   = NULL;
    PRECMAPENTRY    pst_found   = NULL;
    UINT            u_id;
    PRECMAPOFFSET   offsets;

    ENTER(true);

    u_id    = read_int(p_id, MAC_MIN(pst_map->u_id_size, sizeof(UINT)), false);
    offsets = (PRECMAPOFFSET)pst_map->p_offsets;

    for ( pst_entry = (offsets + REMAINDER(u_id))->p_first;
            !pst_found && pst_entry;
            pst_entry = pst_entry->p_next)
    {
//                printf("ID      %X %X\n", pst_entry->u_id, u_id);
//        la6_dump_print("SRC     ", &pst_entry->u_id, pst_map->u_id_size);
//        la6_dump_print("    CMP ", p_id, pst_map->u_id_size);


        if (  (pst_entry->u_id == u_id)                             &&
              !memcmp(&pst_entry->u_id, p_id, pst_map->u_id_size)   )
        {
            pst_found = pst_entry;
        }
        else
            pst_prev = pst_entry;
    }

    if (pst_found && ppst_prev)
        *ppst_prev = pst_prev;

    //printf(" _entry_find = %p\n", pst_found);

    RETURN(pst_found);
}
BOOL recmap_clean(HRECMAP h_recmap)
{
    BOOL            b_result    = true;
    PRECMAP         pst_map     = (PRECMAP)h_recmap;
    PRECMAPOFFSET   pst_offs;
    PRECMAPENTRY    pst_curr;
    PRECMAPENTRY    pst_tmp;
    UINT            u_idx;

    ENTER(pst_map);

    if ((pst_offs = (PRECMAPOFFSET)pst_map->p_offsets) != NULL)
    {
        for ( u_idx = 0;
              b_result && (u_idx < pst_map->u_divider);
              u_idx++, pst_offs++)
        {
            pst_curr = pst_offs->p_first;

            while (b_result && pst_curr)
            {
                if (_recmap_entry_free(pst_map, pst_curr))
                {
                    pst_tmp  = pst_curr;
                    pst_curr = pst_curr->p_next;

                    FREE(pst_tmp);
                }
                else
                    b_result = false;
            }

            pst_offs->p_first = pst_curr;
        }

        memset(pst_map->p_offsets, 0, pst_map->u_divider * sizeof(RECMAPOFFSET));
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      recmap_destroy
// PURPOSE
//      Destroy Record Map and control structure
// PARAMETERS
//      PRECMAP     pst_map  -- Pointer to Record Map control structure
// RESULT
//      PRECMAP  -- Always NULL
// ***************************************************************************
HRECMAP recmap_destroy(HRECMAP h_recmap)
{
    PRECMAP         pst_map     = (PRECMAP)h_recmap;

    ENTER(pst_map);

    if (recmap_clean(h_recmap))
    {
        if (pst_map->p_offsets)
        {
            FREE(pst_map->p_offsets);
        }

        FREE(pst_map);
        pst_map = NULL;
    }

    RETURN((HRECMAP)pst_map);
}
// ***************************************************************************
// FUNCTION
//      recmapx_create
// PURPOSE
//
// PARAMETERS
//      UINT          u_hash      -- Size of hash map
//      PFNRECCLEAN   pfn_free   --
// RESULT
//      HRECMAP  --
// ***************************************************************************
HRECMAP recmap_create(UINT          u_hash,
                      UINT          u_id_size,
                      PFNRECFREE    pfn_free)
{
    PRECMAP         pst_map     = NULL;
    UINT            u_size;
    UINT            u_divider;

    ENTER(u_hash && u_id_size);

    if ((u_divider = u_hash / sizeof(RECMAPOFFSET)) > 0)
    {
        if ((pst_map = CREATE(RECMAP)) != NULL)
        {
            pst_map->u_id_size  = u_id_size;
            pst_map->u_divider  = u_divider;
            pst_map->pfn_free   = pfn_free;
            u_size              = u_divider * sizeof(RECMAPOFFSET);

            if ((pst_map->p_offsets = MALLOC(u_size)) != NULL)
            {
                memset(pst_map->p_offsets, 0, u_size);
            }
            else
                pst_map = (PRECMAP)recmap_destroy((HRECMAP)pst_map);
        }
    }

    RETURN((HRECMAP)pst_map);
}
BOOL recmap_check(HRECMAP       h_recmap,
                  UINT          u_hash,
                  UINT          u_id_size)
{
    BOOL            result      = false;
    PRECMAP         pst_map     = (PRECMAP)h_recmap;

    ENTER(true);

    if (  (pst_map->u_id_size == u_id_size)                         &&
          (pst_map->u_divider == (u_hash / sizeof(RECMAPOFFSET)))   )
    {
        result = true;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      recmapx_add_fast
// PURPOSE
//
// PARAMETERS
//      HRECMAP   h_recmap --
//      UINT      u_id     --
//      PVOID     p_data   --
//      UINT      u_size   --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL recmap_add_fast(HRECMAP        h_recmap,
                     PVOID          p_id,
                     PVOID          p_data,
                     UINT           u_size)
{
    BOOL                b_result    = false;
    PRECMAP             pst_map     = (PRECMAP)h_recmap;
    PRECMAPOFFSET       pst_offs;
    PRECMAPENTRY        pst_prev;
    PRECMAPENTRY        pst_new;

    ENTER(pst_map && p_data);

    if ((pst_new = CREATE_X(RECMAPENTRY, GET_X_SZ(pst_map))) != NULL)
    {
        pst_new->p_data = p_data;
        pst_new->u_size = u_size;
        pst_new->u_id   = 0;

        memcpy(&pst_new->u_id, p_id, pst_map->u_id_size);

        pst_offs        = (((PRECMAPOFFSET)pst_map->p_offsets) + REMAINDER(pst_new->u_id));
        pst_prev        = pst_offs->p_last;
        b_result        = true;

        if (!pst_offs->p_first)
            pst_offs->p_last = pst_offs->p_first = pst_new;

        if (pst_prev == pst_offs->p_last)
            pst_offs->p_last = pst_new;

        if (pst_prev)
            pst_prev->p_next = pst_new;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      recmapx_add
// PURPOSE
//
// PARAMETERS
//      HRECMAP   h_recmap --
//      UINT      u_id     --
//      PVOID     p_data   --
//      UINT      u_size   --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL recmap_add(HRECMAP         h_recmap,
                PVOID           p_id,
                PVOID           p_data,
                UINT            u_size)
{
    BOOL                b_result    = false;
    PRECMAP             pst_map     = (PRECMAP)h_recmap;

    ENTER(pst_map);

    if (!_entry_find(pst_map, p_id, NULL))
    {
        b_result = recmap_add_fast(h_recmap, p_id, p_data, u_size);
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      recmapx_add_copy
// PURPOSE
//
// PARAMETERS
//      HRECMAP   h_recmap --
//      UINT      u_id     --
//      PVOID     p_data   --
//      UINT      u_size   --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL recmap_add_copy(HRECMAP    h_recmap,
                      PVOID      p_id,
                      PVOID      p_data,
                      UINT       u_size)
{
    BOOL            b_result        = false;
    PVOID           p_new;

    ENTER(true);

    if ((p_new = strz_memdup(p_data, u_size)) != NULL)
    {
        if ((b_result = recmap_add(h_recmap, p_id, p_new, u_size)) == false)
        {
            FREE(p_new);
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      recmapx_add_copy_fast
// PURPOSE
//
// PARAMETERS
//      HRECMAP   h_recmap --
//      UINT      u_id     --
//      PVOID     p_data   --
//      UINT      u_size   --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL recmap_add_copy_fast(HRECMAP   h_recmap,
                          PVOID     p_id,
                          PVOID     p_data,
                          UINT      u_size)
{
    BOOL            b_result        = false;
    PVOID           p_new;

    ENTER(true);

    if ((p_new = strz_memdup(p_data, u_size)) != NULL)
    {
        if ((b_result = recmap_add_fast(h_recmap, p_id, p_new, u_size)) == false)
        {
            FREE(p_new);
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      recmapx_ovr
// PURPOSE
//
// PARAMETERS
//      HRECMAP   h_recmap --
//      UINT      u_id     --
//      PVOID     p_data   --
//      UINT      u_size   --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL recmap_ovr(HRECMAP         h_recmap,
                PVOID           p_id,
                PVOID           p_data,
                UINT            u_size)
{
    BOOL                b_result        = false;
    PRECMAP             pst_map         = (PRECMAP)h_recmap;
    PRECMAPENTRY        pst_curr;

    ENTER(pst_map && p_data);

    if ((pst_curr = _entry_find(pst_map, p_id, NULL)) != NULL)
    {
        if (_recmap_entry_free(pst_map, pst_curr))
        {
            pst_curr->p_data    = p_data;
            pst_curr->u_size    = u_size;
            b_result            = true;
        }
    }
    else
        b_result = recmap_add(h_recmap, p_id, p_data, u_size);

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      recmapx_ovr_copy
// PURPOSE
//
// PARAMETERS
//      HRECMAP   h_recmap --
//      UINT      u_id     --
//      PVOID     p_data   --
//      UINT      u_size   --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL recmap_ovr_copy(HRECMAP        h_recmap,
                     PVOID          p_id,
                     PVOID          p_data,
                     UINT           u_size)
{
    BOOL                b_result        = false;
    PRECMAP             pst_map         = (PRECMAP)h_recmap;
    PVOID               p_new;
    PRECMAPENTRY        pst_curr;

    ENTER(pst_map && p_data && u_size);

    if ((pst_curr = _entry_find(pst_map, p_id, NULL)) != NULL)
    {
        if ((p_new = strz_memdup(p_data, u_size)) != NULL)
        {
            if (_recmap_entry_free(pst_map, pst_curr))
            {
                pst_curr->p_data = p_new;
                pst_curr->u_size = u_size;
                b_result         = true;
            }
            else
            {
                FREE(p_new);
            }
        }
    }
    else
        b_result = recmap_add_copy(h_recmap, p_id, p_data, u_size);

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      la6_recmapx_rec_get
// PURPOSE
//      Retrieve pointer to Record's data
// PARAMETERS
//      PRECMAP pst_map -- Pointer to Record Map control structure
//      UINT      u_id    -- Record ID
//      PUINT     pu_size -- Pointer to UINT to which will be stored
//                            size of Record
// RESULT
//      PVOID   -- Pointer to data of Record
// ***************************************************************************
PVOID recmap_get(HRECMAP        h_recmap,
                 PVOID          p_id,
                 PUINT          pu_size)
{
    PVOID               p_record    = NULL;
    PRECMAP             pst_map     = (PRECMAP)h_recmap;
    PRECMAPENTRY        pst_entry;

    ENTER(pst_map);

    if ((pst_entry = _entry_find(pst_map, p_id, NULL)) != NULL)
    {
        if (pu_size)
            *pu_size = pst_entry->u_size;

        p_record     = pst_entry->p_data;
    }

    RETURN(p_record);
}
// ***************************************************************************
// FUNCTION
//      recmapx_del
// PURPOSE
//
// PARAMETERS
//      HRECMAP   h_recmap --
//      UINT      u_id     --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL recmap_del(HRECMAP         h_recmap,
                PVOID           p_id)
{
    BOOL                b_result    = false;
    PRECMAP             pst_map     = (PRECMAP)h_recmap;
    PRECMAPOFFSET       pst_offs;
    PRECMAPENTRY        pst_prev;
    PRECMAPENTRY        pst_entry;
    UINT                u_id;

    ENTER(pst_map);

    if ((pst_entry = _entry_find(pst_map, p_id, &pst_prev)) != NULL)
    {
        u_id = read_int(p_id, MAC_MIN(pst_map->u_id_size, sizeof(UINT)), false);

        pst_offs = (((PRECMAPOFFSET)pst_map->p_offsets) + REMAINDER(u_id));

        if (_recmap_entry_free(pst_map, pst_entry))
        {
            if(pst_prev)
                pst_prev->p_next = pst_entry->p_next;

            if(pst_offs->p_first == pst_entry)
                pst_offs->p_first = pst_entry->p_next;

            if(pst_offs->p_last == pst_entry)
                pst_offs->p_last = pst_prev;

            FREE(pst_entry);

            b_result = true;
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      recmap_enum
// PURPOSE
//
// PARAMETERS
//      HRECMAP     h_recmap --
//      PFNRECENUMX pfn_enum --
//      PVOID       p_ptr    --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL recmap_enum(HRECMAP            h_recmap,
                 PFNRECENUMX        pfn_enum,
                 PVOID              p_ptr)
{
    BOOL                b_result    = true;
    PRECMAP             pst_map     = (PRECMAP)h_recmap;
    PRECMAPOFFSET       pst_offs;
    PRECMAPENTRY        pst_curr;
    UINT               d_cnt;

    ENTER(pst_map && pfn_enum);

    if (pst_map->p_offsets)
    {
        for ( d_cnt = 0, pst_offs = (PRECMAPOFFSET)pst_map->p_offsets;
              b_result && (d_cnt < pst_map->u_divider);
              d_cnt++, pst_offs++)
        {
            for ( pst_curr = pst_offs->p_first;
                  b_result && pst_curr;
                  pst_curr = pst_curr->p_next)
            {
                b_result = (*pfn_enum)(p_ptr, pst_curr->p_data, pst_curr->u_size, &pst_curr->u_id);
            }
        }
    }

    RETURN(b_result);
}
/*
// ***************************************************************************
// FUNCTION
//      recmap_save
// PURPOSE
//      Save the Record Map data to file
// PARAMETERS
//      HRECMAP    h_map      -- Pointer to Record Map control structure
//      PSTR       psz_filename -- Pointer to file name and path
// RESULT
//      BOOL
//          true:  All is ok
//          false: An error has occured
// ***************************************************************************
BOOL recmap_save(UINT           version,
                 HRECMAP        h_map,
                 PFNRECSER      pfn_ser,
                 PCSTR          psz_filename)
{
    BOOL                b_result    = false;
    PRECMAP             pst_map     = (PRECMAP)h_map;
    HA7FILE             h_file;
    FILEHEADER          hdr;
    PRECMAPOFFSET       pst_offs;
    PRECMAPENTRY        ent;
    UINT                u_cnt;
    UINT                rec_size;
    PVOID               saveData;

    ENTER(pst_map && psz_filename);

    if (pst_map->p_offsets)
    {
        if ((h_file = a7file_open(psz_filename, O_RDWR | O_CREAT)) != NULL)
        {
            hdr.u_version   = version;
            hdr.u_id_size   = pst_map->u_id_size;
            hdr.u_divider   = pst_map->u_divider;
            rec_size        = sizeof(RECMAPENTRY) + GET_X_SZ(pst_map);

            hdr.u_checksum  = 0;
            hdr.u_checksum  = la6_CRC32_calc(&hdr, sizeof(hdr));

            if (a7file_write(h_file, &hdr, sizeof(hdr)) == sizeof(hdr))
            {
                for (   b_result = true, u_cnt = 0, pst_offs = (PRECMAPOFFSET)pst_map->p_offsets;
                        b_result && (u_cnt < pst_map->u_divider);
                        u_cnt++, pst_offs++)
                {
                    for (  ent = pst_offs->p_first;
                           b_result && ent;
                           ent = ent->p_next)
                    {
                        if (ent->p_data)
                        {
                            saveData = pfn_ser ?
                                       (*pfn_ser)(ent->p_data, &ent->u_size) :
                                       ent->p_data;

                            if ( !(
                                    ent->u_size                                             &&
                                    saveData                                                &&
                                    (a7file_write(h_file, ent, rec_size) == rec_size)       &&
                                    (a7file_write(h_file, saveData, ent->u_size) == ent->u_size)
                               )  )
                            {
                                b_result = false;
                            }

                            if (saveData && (saveData != ent->p_data))
                            {
                                FREE(saveData);
                            }
                        }
                    }
                }
            }

            a7file_close(h_file);

            if (!b_result)
                a7file_remove(psz_filename);
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      recmap_load
// PURPOSE
//      Load the Record Map into the memory
// PARAMETERS
//      PSTR       psz_filename -- Pointer to file name and path
// RESULT
//      PLA6RECARRAY    -- Pointer to the just created Record Map control
//                         structure if all is ok or NULL if not
// ***************************************************************************
HRECMAP recmap_load(UINT            version,
                    PCSTR           psz_filename,
                    PFNRECUNSER     pfn_unser,
                    PFNRECFREE      pfn_free)
{
    HRECMAP             h_map       = NULL;
    HA7FILE             h_file;
    FILEHEADER          hdr;
    PRECMAPENTRY        rec;
    PVOID               p_buff;
    UINT                rec_size;
    PVOID               loadData;
    UINT                loadSize;
    UINT32              checksum;

    ENTER(psz_filename);

    if ((h_file = a7file_open(psz_filename, O_RDONLY)) != NULL)
    {
        if (a7file_read(h_file, &hdr, sizeof(hdr)) == sizeof(hdr))
        {
            checksum        = hdr.u_checksum;
            hdr.u_checksum  = 0;

            if (  (checksum == la6_CRC32_calc(&hdr, sizeof(hdr)))   &&
                  (hdr.u_version == version)                        )
            {
                if ((h_map = recmap_create(hdr.u_divider * sizeof(RECMAPOFFSET),
                                           hdr.u_id_size,
                                           pfn_free)) != NULL)
                {
                    rec_size = sizeof(RECMAPENTRY) + GET_X_SZ((PRECMAP)h_map);

                    if ((rec = MALLOC(rec_size)) != NULL)
                    {
                        //printf("recmap load 1, rec_size = %d, ex = %d\n", rec_size, GET_X_SZ((PRECMAP)h_map));

                        while (h_map && (a7file_read(h_file, rec, rec_size) == rec_size))
                        {
                            //printf("recmap load 2: id = %d, size = %d\n", rec->u_id, rec->u_size);
                            loadSize = rec->u_size;

                            if ((p_buff = MALLOC(loadSize)) != NULL)
                            {
                                if ( (a7file_read(h_file, p_buff, loadSize) == loadSize)                            &&
                                     ((loadData = pfn_unser ? (*pfn_unser)(p_buff, &loadSize) : p_buff) != NULL)    )
                                {
                                    if (recmap_add_fast(h_map, &rec->u_id, loadData, loadSize))
                                    {
                                        if (loadData != p_buff)
                                        {
                                            FREE(p_buff);
                                        }
                                    }
                                    else
                                    {
                                        printf("recmap load error 1\n");

                                        h_map = recmap_destroy(h_map);

                                        FREE(p_buff);

                                        if (loadData != p_buff)
                                        {
                                            FREE(loadData);
                                        }
                                    }
                                }
                                else
                                {
                                    printf("recmap load error 2\n");

                                    h_map = recmap_destroy(h_map);

                                    FREE(p_buff);
                                }
                            }
                            else
                                h_map = recmap_destroy(h_map);
                        }

                        FREE(rec);
                    }
                }
            }
        }

        a7file_close(h_file);
    }

    RETURN(h_map);
}
*/
// ***************************************************************************
// FUNCTION
//      recmap_cat
// PURPOSE
//
// PARAMETERS
//      HRECMAP    h_recmap --
//      HRECMAP    h_add    --
//      PFNRECCOPY pfn_copy --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL recmap_cat(HRECMAP            h_recmap,
                HRECMAP            h_add,
                PFNRECCOPY         pfn_copy)
{
    BOOL                b_result    = true;
    PRECMAP             pst_map     = (PRECMAP)h_add;
    PRECMAPOFFSET       pst_offs;
    PRECMAPENTRY        pst_curr;
    UINT                d_cnt;
    PVOID               p_data;

    ENTER(pst_map && h_recmap && pfn_copy);

    if (pst_map->p_offsets)
    {
        for ( d_cnt = 0, pst_offs = (PRECMAPOFFSET)pst_map->p_offsets;
              b_result && (d_cnt < pst_map->u_divider);
              d_cnt++, pst_offs++)
        {
            for ( pst_curr = pst_offs->p_first;
                  b_result && pst_curr;
                  pst_curr = pst_curr->p_next)
            {
                if ((p_data = (*pfn_copy)(pst_curr->p_data, pst_curr->u_size)) != NULL)
                {
                    if ((b_result = recmap_add_fast(h_recmap, &pst_curr->u_id, p_data, pst_curr->u_size)) == false)
                    {
                        if (pst_map->pfn_free)
                        {
                            (*pst_map->pfn_free)(p_data);
                        }
                    }
                }
            }
        }
    }

    RETURN(b_result);
}

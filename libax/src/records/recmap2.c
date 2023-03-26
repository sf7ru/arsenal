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

#include <records.h>
#include <pvt_records.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define GET_IDMAP(a)        (map->maps + ((a) & map->mask))

// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      recmap2_clean
// PURPOSE
//
// PARAMETERS
//      HRECMAP2 h_recmap --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL recmap2_clean(HRECMAP2 h_recmap)
{
    BOOL                result      = true;
    PRECMAP2            map         = (PRECMAP2)h_recmap;
    HIDMAP *            on;
    HIDMAP *            end;

    ENTER(map);

    on  = map->maps;
    end = map->maps + map->mask;

    for (; result && (on <= end); on++)
    {
        if (*on)
            idmap_destroy(*on);
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      recmap2_destroy
// PURPOSE
//      Destroy Record Map and control structure
// PARAMETERS
//      PNEWRECMAP     pst_map  -- Pointer to Record Map control structure
// RESULT
//      PNEWRECMAP  -- Always NULL
// ***************************************************************************
HRECMAP2 recmap2_destroy(HRECMAP2 h_recmap)
{
    PRECMAP2         map     = (PRECMAP2)h_recmap;

    ENTER(map);

    if (map->maps)
    {
        if (recmap2_clean(h_recmap))
        {
            FREE(map->maps);
        }

        FREE(map);
        map = NULL;
    }

    RETURN((HRECMAP2)map);
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
//      HRECMAP2  --
// ***************************************************************************
HRECMAP2 recmap2_create(UINT          hashBits,
                        UINT          idSize,
                        UINT          reallocAmount,
                        PFNRECFREE    pfn_free)
{
    PRECMAP2            map     = NULL;
    UINT                size;

    ENTER(hashBits && idSize);

    if ((map = CREATE(RECMAP2)) != NULL)
    {
        size        = (1 << hashBits);
        map->mask   = size - 1;
        size       *= sizeof(PVOID);

        if ((map->maps = MALLOC(size)) != NULL)
        {
            memset(map->maps, 0, size);

            map->idSize     = idSize;
            map->amount     = reallocAmount;
            map->pfn_free   = pfn_free;
        }
        else
            map = (PRECMAP2)recmap2_destroy((HRECMAP2)map);
    }

    RETURN((HRECMAP2)map);
}
// ***************************************************************************
// FUNCTION
//      recmapx_add_fast
// PURPOSE
//
// PARAMETERS
//      HRECMAP2   h_recmap --
//      UINT      u_id     --
//      PVOID     p_data   --
//      UINT      u_size   --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL recmap2_add(HRECMAP2       recmap,
                 PIDREC         rec)
{
    BOOL                b_result    = false;
    PRECMAP2          map         = (PRECMAP2)recmap;
    HIDMAP *            on;

    ENTER(map && rec);

    on = GET_IDMAP(rec->id);

    if (*on || ((*on = idmap_create(map->amount, map->amount, map->idSize, map->pfn_free)) != NULL))
    {
        b_result = idmap_add(*on, rec);
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      recmapx_add_fast
// PURPOSE
//
// PARAMETERS
//      HRECMAP2   h_recmap --
//      UINT      u_id     --
//      PVOID     p_data   --
//      UINT      u_size   --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL recmap2_add_fast(HRECMAP2      recmap,
                      PIDREC        rec,
                      UINT          whereto)
{
    BOOL                b_result    = false;
    PRECMAP2            map         = (PRECMAP2)recmap;
    HIDMAP *            on;

    ENTER(map && rec);

    on = GET_IDMAP(rec->id);

    if (*on)
    {
        b_result = idmap_add_fast(*on, rec, whereto);
    }
    else if ((*on = idmap_create(map->amount, map->amount, map->idSize, map->pfn_free)) != NULL)
    {
        b_result = idmap_add_fast(*on, rec, whereto);
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      recmap2_get
// PURPOSE
//
// PARAMETERS
//      HRECMAP2 h_recmap --
//      PVOID    id       --
// RESULT
//      PIDREC --
// ***************************************************************************
PIDREC recmap2_get(HRECMAP2     h_recmap,
                   PVOID        id,
                   PUINT        whereto)
{
    PIDREC              result      = NULL;
    PRECMAP2            map         = (PRECMAP2)h_recmap;
    HIDMAP *            on;

    ENTER(map);

    on = GET_IDMAP(*(PUINT)id);

    if (*on != NULL)
    {
        result = idmap_get(*on, id, whereto);
    }
    else
    {
        if (whereto)
            *whereto = 0;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      recmap2_del
// PURPOSE
//
// PARAMETERS
//      HRECMAP2 h_recmap --
//      PVOID    p_id     --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL recmap2_del(HRECMAP2        h_recmap,
                 PVOID           id)
{
    BOOL                result      = false;
    PRECMAP2            map         = (PRECMAP2)h_recmap;
    HIDMAP *            on;

    ENTER(map);

    on = GET_IDMAP(*(PUINT)id);

    if (*on != NULL)
    {
        result = idmap_del(*on, id);
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      recmap2_stat
// PURPOSE
//
// PARAMETERS
//      HRECMAP2 h_recmap --
// RESULT
//      void --
// ***************************************************************************
void recmap2_stat(HRECMAP2 h_recmap)
{
    PRECMAP2            map         = (PRECMAP2)h_recmap;
    HIDMAP *            on;
    HIDMAP *            end;

    ENTER(map);

    on  = map->maps;
    end = map->maps + map->mask;

    while (on <= end)
    {
//        printf("[%d]", *on ? idmap_count(*on) : 0);

        on++;
    }

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      recmap2_get_first
// PURPOSE
//
// PARAMETERS
//      HRECMAP2 h_recmap --
// RESULT
//      PIDREC --
// ***************************************************************************
PIDREC recmap2_get_first(HRECMAP2     h_recmap)
{
    PIDREC              result      = NULL;
    PRECMAP2            map         = (PRECMAP2)h_recmap;
    HIDMAP *            on;
    HIDMAP *            end;

    ENTER(map);

    on  = map->maps;
    end = map->maps + map->mask;

    for (; !result && (on <= end); on++)
    {
        if (*on)
            result = idmap_get_first(*on);
    }

    RETURN(result);
}

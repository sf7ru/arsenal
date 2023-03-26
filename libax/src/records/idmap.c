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
#include <records.h>
#include <pvt_records.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _where
// PURPOSE
//
// PARAMETERS
//      PIDMAP map     --
//      PVOID  pid     --
//      PUINT  whereto --
// RESULT
//      UINT --
// ***************************************************************************
static UINT _where(PIDMAP  map,
                   PVOID    pid,
                   PUINT    whereto)
{
#define ID(a) (*(map->map + (a)))->id

    UINT        result  = AXII;
    UINT        first   = 0;
    UINT        last    = map->used;
    UINT        mid;
    UINT        id;
    PIDREC *   on;
    PIDREC *   end;

    ENTER(true);

    *whereto    = AXII;
    id          = *(PUINT)pid;

    if (map->used)
    {
        if (last && (ID(first) <= id))
        {
            if (ID(last - 1) >= id)
            {
                while (first < last)
                {
                    mid = first + ((last - first) >> 1);

                    if (id <= ID(mid))
                    {
                        last = mid;
                    }
                    else
                    {
                        first = mid + 1;
                    }
                }

                if (ID(last) == id)
                {
                    if (map->idSize > sizeof(UINT))
                    {
                        on  = map->map  + last;
                        end = map->map  + map->used;

                        while ((on < end) && ((*on)->id == id))
                        {
                            if (!memcmp(pid, &(*on)->id, map->idSize))
                            {
                                result = last;
                                break;
                            }
                            else
                            {
                                on++;
                                last++;
                            }
                        }

                        if (result == AXII)
                            *whereto = last;
                    }
                    else
                        result = last;
                }
                else
                    *whereto = last;
            }
            else
                *whereto = map->used;
        }
        else
            *whereto = 0;
    }
    else
        *whereto = 0;

    RETURN(result);

#undef  ID
}
// ***************************************************************************
// FUNCTION
//      idmap_clean
// PURPOSE
//
// PARAMETERS
//      HIDMAP h_map --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL idmap_clean(HIDMAP h_map)
{
    BOOL            result          = true;
    PIDMAP          map             = (PIDMAP)h_map;
    PIDREC *        on;
    PIDREC *        end;

    ENTER(map);

    if (map->pfn_free)
    {
        on  = map->map;
        end = on + map->used;

        while (on < end)
        {
            (*map->pfn_free)(*on);
            on++;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      idmap_destroy
// PURPOSE
//
// PARAMETERS
//      HIDMAP h_map --
// RESULT
//      HIDMAP --
// ***************************************************************************
HIDMAP idmap_destroy(HIDMAP h_map)
{
    PIDMAP     map         = (PIDMAP)h_map;

    ENTER(map);

    if (map->map)
    {
        idmap_clean(h_map);

        FREE(map->map);
    }

    FREE(map);

    RETURN(NULL);
}
// ***************************************************************************
// FUNCTION
//      idmap_create
// PURPOSE
//
// PARAMETERS
//      UINT       init        --
//      UINT       allocAmount --
//      UINT       idSize      --
//      PFNRECFREE pfn_free    --
// RESULT
//      HIDMAP --
// ***************************************************************************
HIDMAP idmap_create(UINT        init,
                    UINT        allocAmount,
                    UINT        idSize,
                    PFNRECFREE  pfn_free)
{
    PIDMAP             result          = NULL;

    ENTER(true);

    if (idSize >= sizeof(UINT))
    {
        if ((result = CREATE(IDMAP)) != NULL)
        {
            result->size            = init;
            result->idSize          = idSize;
            result->allocAmount     = allocAmount;
            result->pfn_free        = pfn_free;

            if ((result->map  = (PIDREC*)MALLOC(init * sizeof(PIDREC))) == NULL)
            {
                result = (PIDMAP)idmap_destroy((HIDMAP)result);
            }
        }
    }

    RETURN((HIDMAP)result);
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _realloc
// PURPOSE
//
// PARAMETERS
//      PIDMAP map --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
static BOOL _realloc(PIDMAP map)
{
    BOOL            result          = false;
    UINT            tmpSize;
    PIDREC *       tmpMap;

    ENTER(true);

    if (map->allocAmount)
    {
        tmpSize = map->size + map->allocAmount;

        if ((tmpMap = (PIDREC*)REALLOC(map->map, tmpSize * sizeof(PIDREC))) != NULL)
        {
            map->map    = tmpMap;
            map->size   = tmpSize;
            result      = true;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      idmap_add
// PURPOSE
//
// PARAMETERS
//      HIDMAP h_map --
//      PIDREC rec   --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL idmap_add(HIDMAP     h_map,
               PIDREC     rec)
{
    BOOL            result          = false;
    PIDMAP          map             = (PIDMAP)h_map;
    UINT            whereto;

    ENTER(true);

    if (_where(map, rec, &whereto) == AXII)
    {
        result = idmap_add_fast(h_map, rec, whereto);
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      idmap_add
// PURPOSE
//
// PARAMETERS
//      HIDMAP h_map --
//      PIDREC rec   --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL idmap_add_fast(HIDMAP     h_map,
                    PIDREC     rec,
                    UINT       whereto)
{
    BOOL            result          = false;
    PIDMAP          map             = (PIDMAP)h_map;
    PIDREC *        on;

    ENTER(true);

    if (whereto != AXII)
    {
        if ((map->used < map->size) || _realloc(map))
        {
            on = map->map + whereto;

            if (whereto < map->used)
            {
                memmove(on + 1, on, (map->used - whereto) * sizeof(PIDREC));
            }

            *on     = rec;
            result  = true;

            map->used++;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      idmap_get
// PURPOSE
//
// PARAMETERS
//      HIDMAP h_map --
//      PVOID  pid   --
// RESULT
//      PIDREC --
// ***************************************************************************
PIDREC idmap_get(HIDMAP     h_map,
                 PVOID      pid,
                 PUINT      whereto)
{
    PIDREC          result          = NULL;
    PIDMAP          map             = (PIDMAP)h_map;
    UINT            idx;
    UINT            wt;

    ENTER(true);

    if ((idx = _where(map, pid, &wt)) != AXII)
    {
        result = *(map->map + idx);
    }

    if (whereto)
        *whereto = wt;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      idmap_get
// PURPOSE
//
// PARAMETERS
//      HIDMAP h_map --
//      PVOID  pid   --
// RESULT
//      PIDREC --
// ***************************************************************************
PIDREC idmap_get_first(HIDMAP     h_map)
{
    PIDREC          result          = NULL;
    PIDMAP          map             = (PIDMAP)h_map;

    ENTER(true);

    if (map->used)
    {
        result = *map->map;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      idmap_del
// PURPOSE
//
// PARAMETERS
//      HIDMAP h_map --
//      PVOID  pid   --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL idmap_del(HIDMAP     h_map,
               PVOID      pid)
{
    BOOL            result          = false;
    PIDMAP          map             = (PIDMAP)h_map;
    UINT            idx;
    UINT            whereto;

    ENTER(true);

    if ((idx = _where(map, pid, &whereto)) != AXII)
    {
        if (  !map->pfn_free                                ||
              ((*map->pfn_free)(*(map->map + idx)) == NULL) )
        {
            whereto = idx + 1;

            if (whereto < map->used)
            {
                memmove(map->map + idx, map->map + whereto, (map->used - whereto) * sizeof(PIDREC));
            }

            map->used--;
            result = true;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      idmap_count
// PURPOSE
//
// PARAMETERS
//      HIDMAP h_map --
// RESULT
//      UINT --
// ***************************************************************************
UINT idmap_count(HIDMAP     h_map)
{
    UINT            result          = 0;
    PIDMAP          map             = (PIDMAP)h_map;

    ENTER(true);

    result = map->used;

    RETURN(result);
}

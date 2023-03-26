// ***************************************************************************
// TITLE
//
// PROJECT
//
// ***************************************************************************
// FILE
//      $Id$
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************

#include <string.h>
#include <stdlib.h>

#include <arsenal.h>
#include <axdata.h>
#include <axmath.h>


// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// ---------------------------------------------------------------------------

#define TAIL_ONES(a)  ((1 << (a & 7)) - 1)


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// STRUCTURE
//      BINMAP
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_BINMAP
{
    PU8       p_data;
    UINT        u_banks;
    UINT        u_bits;

    UINT        u_first_zero;

    UINT        u_complete;
} BINMAP, * PBINMAP;

// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _set_tail
// PURPOSE
//
// PARAMETERS
//      PBINMAP pst_map --
// RESULT
//      void --
// ***************************************************************************
static void _set_tail(PBINMAP     pst_map)
{
    UINT u_tail = pst_map->u_bits & 7;

    if (u_tail)
    {
        *(pst_map->p_data + (pst_map->u_banks - 1)) = ~((1 << u_tail) - 1);
    }
}
// ***************************************************************************
// FUNCTION
//      binmap2_destroy
// PURPOSE
//
// PARAMETERS
//      HBINMAP   h_map --
// RESULT
//      HBINMAP  --
// ***************************************************************************
HBINMAP binmap2_destroy(HBINMAP h_map)
{
    PBINMAP     pst_map     = (PBINMAP)h_map;

    ENTER(pst_map);

    if (pst_map->p_data)
        FREE(pst_map->p_data);

    FREE(pst_map);

    RETURN(NULL);
}
// ***************************************************************************
// FUNCTION
//      binmap_create
// PURPOSE
//
// PARAMETERS
//      UINT   u_banks --
// RESULT
//      HBINMAP  --
// ***************************************************************************
HBINMAP binmap2_create(UINT u_bits,
                       BOOL b_ones)
{
    PBINMAP     pst_map     = NULL;

    ENTER(u_bits);

    if ((pst_map = CREATE(BINMAP)) != NULL)
    {
        pst_map->u_bits         = u_bits;
        pst_map->u_banks        = ax_parts(u_bits, 8);

        if ((pst_map->p_data = MALLOC(pst_map->u_banks)) != NULL)
        {
            if (b_ones)
            {
                binmap2_mark_all((HBINMAP)pst_map);
            }
            else
            {
                binmap2_unmark_all((HBINMAP)pst_map);
            }

//            printf("%d %d ", pst_map->u_bits, pst_map->u_banks);
//            la6_dump_print(" MAP ", pst_map->p_data, pst_map->u_banks);
        }
        else
            pst_map = (PBINMAP)binmap2_destroy((HBINMAP)pst_map);
    }

    RETURN((HBINMAP)pst_map);
}
//BOOL binmap2_expand(HBINMAP     h_map,
//                    UINT        u_bits,
//                    BOOL        b_ones)
//{
//    BOOL            result          = false;
//    PBINMAP         pst_map         = (PBINMAP)h_map;
//
//    ENTER(true);
//
//    if (u_bits > pst_map->u_bits)
//    {
//
//    }
//    else
//
//    RETURN(result);
//}
// ***************************************************************************
// FUNCTION
//      binmap_mark
// PURPOSE
//
// PARAMETERS
//      HBINMAP   h_map --
//      UINT      u_idx --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL binmap2_mark(HBINMAP    h_map,
                 UINT       u_idx)
{
    BOOL        b_result    = false;
    PBINMAP     pst_map     = (PBINMAP)h_map;
    UINT        u_bank;
    U8        b_val;
    PU8       p_on;

    ENTER(pst_map);

    if (u_idx < pst_map->u_bits)
    {
        u_bank      = u_idx >> 3;
        p_on        = pst_map->p_data + u_bank;
        b_val       = *p_on | (1 << (u_idx & 7));

        if (b_val == 0xFF)
        {
            if (u_bank == pst_map->u_first_zero)
            {
                pst_map->u_first_zero = 0;
            }

            if (b_val != *p_on)
                pst_map->u_complete++;
        }


        *p_on       = b_val;
        b_result    = true;
    }

    //printf("mark       %d\n", u_idx);

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      binmap_mark_all
// PURPOSE
//
// PARAMETERS
//      HBINMAP   h_map --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL binmap2_mark_all(HBINMAP h_map)
{
    BOOL        b_result    = false;
    PBINMAP     pst_map     = (PBINMAP)h_map;

    ENTER(pst_map);

    memset(pst_map->p_data, -1, pst_map->u_banks);

    pst_map->u_first_zero   = AXII; // Means no zeroes
    pst_map->u_complete     = pst_map->u_banks;
    b_result                = true;

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      binmap_unmark
// PURPOSE
//
// PARAMETERS
//      HBINMAP   h_map --
//      UINT      u_idx --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL binmap2_unmark(HBINMAP    h_map,
                   UINT       u_idx)
{
    BOOL        b_result    = false;
    PBINMAP     map         = (PBINMAP)h_map;
    UINT        u_bank;
    U8        b_val;
    PU8       p_on;

    ENTER(map);

    if (u_idx < map->u_bits)
    {
        u_bank          = u_idx >> 3;
        p_on            = map->p_data + u_bank;

        if (0xFF == *p_on)
            map->u_complete--;

        b_val           = (*p_on) & ~(1 << (u_idx & 7));
        *p_on           = b_val;

        if (u_bank < map->u_first_zero)
        {
            map->u_first_zero = u_bank;
        }

        b_result        = true;
    }

    //printf("mark       %d\n", u_idx);

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      binmap_unmark_all
// PURPOSE
//
// PARAMETERS
//      HBINMAP   h_map --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL binmap2_unmark_all(HBINMAP h_map)
{
    BOOL        b_result    = false;
    PBINMAP     pst_map     = (PBINMAP)h_map;

    ENTER(pst_map);

    memset(pst_map->p_data, 0, pst_map->u_banks);

    _set_tail(pst_map);

    pst_map->u_first_zero   = 0;
    pst_map->u_complete     = 0;
    b_result                = true;

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      binmap_get
// PURPOSE
//
// PARAMETERS
//      HBINMAP   h_map --
//      UINT      u_idx --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL binmap2_get(HBINMAP h_map, UINT u_idx)
{
    BOOL        b_result    = false;
    PBINMAP     pst_map     = (PBINMAP)h_map;
    PU8       p_on;

    ENTER(pst_map);

    if (u_idx < pst_map->u_bits)
    {
        p_on    = pst_map->p_data + (u_idx >> 3);
        b_result = *p_on & (1 << (u_idx & 7)) ? true : false;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      binmap2_get_first_zero
// PURPOSE
//
// PARAMETERS
//      HBINMAP h_map --
// RESULT
//      UINT --
// ***************************************************************************
UINT binmap2_get_first_zero(HBINMAP            h_map)
{
    UINT        u_result    = AXII;
    PBINMAP     pst_map     = (PBINMAP)h_map;
    PU8       on;
    UINT        idx;
    U8        val;

    ENTER(pst_map);

    if (pst_map->u_first_zero != AXII)
    {
        idx                     = pst_map->u_first_zero;
        on                      = pst_map->p_data + idx;
        pst_map->u_first_zero   = AXII;

        for (; idx < pst_map->u_banks; idx++, on++)
        {
            if (*on != 0xFF)
            {
                val                     = *on;
                u_result                = idx << 3;
                pst_map->u_first_zero   = idx;

                while (val & 1)
                {
                    u_result++;
                    val >>= 1;
                }
                break;
            }
        }
    }

    //printf("first zero %d\n", u_result);

    RETURN(u_result);
}
// ***************************************************************************
// FUNCTION
//      binmap2_get_next_one
// PURPOSE
//
// PARAMETERS
//      HBINMAP h_map --
//      UINT    start --
// RESULT
//      UINT --
// ***************************************************************************
UINT binmap2_get_next_one(HBINMAP            h_map,
                          UINT               start)
{
    UINT        u_result    = AXII;
    PBINMAP     map         = (PBINMAP)h_map;
    PU8       on;
    UINT        idx;
    UINT        tmp;
    UINT        bit;
    U8        val;

    ENTER(map);

//    printf("binmap2_get_next_one stage start = %d\n", start);
//    la6_dump_print("BITS ", map->p_data, 10);

    idx                     = start >> 3;
    on                      = map->p_data + idx;

    for (; idx < map->u_banks; idx++, on++)
    {
        if (*on)
        {
            val         = *on;
            tmp         = idx << 3;
            bit         = 0;

            if (tmp < start)
            {
                bit     = (start & 7);
                val   >>= bit;
            }

//            printf("binmap2_get_next_one stage tmp = %d, bit = %d val = %d\n", tmp, bit, val);

            while ((bit < 8) && !(val & 1))
            {
                val >>= 1;
                bit++;
            }

            if (val & 1)
            {
                tmp += bit;

                if (tmp < map->u_bits)
                {
                    u_result = tmp;
                }
                break;
            }
        }
    }

    RETURN(u_result);
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _get_next_x_slow
// PURPOSE
//
// PARAMETERS
//      PBINMAP map  --
//      BOOL    one  --
//      UINT    from --
//      UINT    to   --
// RESULT
//      UINT --
// ***************************************************************************
static UINT _get_next_x_slow(PBINMAP         map,
                             BOOL            one,
                             UINT            from,
                             UINT            to)
{
    UINT        result  = AXII;
    UINT        cur;

    for (cur = from; cur < to; cur++)
    {
        if (binmap2_get((HBINMAP)map, cur) == one)
        {
            result = cur;
            break;
        }
    }

    return result;
}
// ***************************************************************************
// FUNCTION
//      binmap2_get_next_zero
// PURPOSE
//
// PARAMETERS
//      HBINMAP h_map --
//      UINT    start --
// RESULT
//      UINT --
// ***************************************************************************
UINT binmap2_get_next_zero(HBINMAP            h_map,
                           UINT               from,
                           UINT               to)
{
    UINT        result      = AXII;
    PBINMAP     map         = (PBINMAP)h_map;
    PU8       on;
    UINT        cur;
    UINT        end;
    UINT        found;

    ENTER(map);

    if (from < map->u_banks)
    {
        cur     = from >> 3;
        end     = (to >> 3) + 1;
        on      = map->p_data + cur;

        while (cur < end)
        {
            if (*on != 0xFF)
            {
                found = cur << 3;

                if ((found > from) || (((0xFF << from) & *on) != (0xFF << from)))
                {
                    end     = MAC_MIN(to, map->u_bits);
                    result  = _get_next_x_slow(map, false, found, end);
                    break;
                }
            }

            cur++;
            on++;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      binmap2_raw_size
// PURPOSE
//
// PARAMETERS
//      HBINMAP h_map --
// RESULT
//      UINT --
// ***************************************************************************
UINT binmap2_raw_size(HBINMAP h_map)
{
    UINT            result          = 0;
    PBINMAP         map             = (PBINMAP)h_map;

    ENTER(true);

    result = map->u_banks;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      binmap2_raw_data
// PURPOSE
//
// PARAMETERS
//      HBINMAP h_map --
// RESULT
//      PVOID --
// ***************************************************************************
PVOID binmap2_raw_data(HBINMAP h_map)
{
    PVOID           result          = nil;
    PBINMAP         map             = (PBINMAP)h_map;

    ENTER(true);

    result  = map->p_data;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      binmap2_count
// PURPOSE
//
// PARAMETERS
//      HBINMAP h_map --
// RESULT
//      UINT --
// ***************************************************************************
UINT binmap2_count(HBINMAP h_map)
{
    UINT        u_bits      = 0;
    PBINMAP     pst_map     = (PBINMAP)h_map;

    ENTER(pst_map);

    u_bits = pst_map->u_bits;

    RETURN(u_bits);
}
// ***************************************************************************
// FUNCTION
//      binmap2_resize
// PURPOSE
//
// PARAMETERS
//      HBINMAP h_map  --
//      UINT    u_bits --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
//BOOL binmap2_resize(HBINMAP            h_map,
//                    UINT               u_bits)
//{
//    BOOL            result          = false;
//
//    ENTER(true);
//
//    RETURN(result);
//}
// ***************************************************************************
// FUNCTION
//      binmap2_is_complete
// PURPOSE
//
// PARAMETERS
//      HBINMAP h_map --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL binmap2_is_complete(HBINMAP h_map)
{
    BOOL        result        = false;
    PBINMAP     map             = (PBINMAP)h_map;

    ENTER(map);

//    printf("binmap2_is_complete stage %d/%d\n", map->u_complete, map->u_banks);
    result = (BOOL)(map->u_banks == map->u_complete);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      binmap2_recount
// PURPOSE
//
// PARAMETERS
//      HBINMAP h_map --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL binmap2_recalc(HBINMAP h_map)
{
    BOOL        b_result        = false;
    PU8       p_on;
    UINT        u_bank;
    PBINMAP     map             = (PBINMAP)h_map;

    ENTER(map);

    p_on                = map->p_data;
    map->u_first_zero   = map->u_banks;
    map->u_complete     = 0;
    b_result            = true;

    _set_tail(map);

    for (u_bank = 0; u_bank < map->u_banks; u_bank++, p_on++)
    {
        if (*p_on == 0xFF)
        {
            map->u_complete++;
        }
        else if (map->u_first_zero > u_bank)
        {
            map->u_first_zero = u_bank;
        }
    }

    if (map->u_first_zero == map->u_banks)
        map->u_first_zero = 0;

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      binmap2_fast_extract
// PURPOSE
//
// PARAMETERS
//      HBINMAP h_map    --
//      UINT    bankFrom --
//      UINT    bankTo   --
// RESULT
//      HBINMAP --
// ***************************************************************************
HBINMAP binmap2_fast_extract(HBINMAP   h_map,
                             UINT      bankFrom,
                             UINT      bankTo)
{
    PBINMAP         result      = nil;
    PBINMAP         pst_map     = (PBINMAP)h_map;

    ENTER(true);

    if ((result = CREATE(BINMAP)) != nil)
    {
        if (bankTo < (pst_map->u_banks - 1))
        {
            result->u_banks = bankTo - bankFrom + 1;
            result->u_bits  = result->u_banks << 3;
        }
        else
        {
            result->u_banks = pst_map->u_banks - bankFrom;
            result->u_bits  = pst_map->u_bits - (bankFrom << 3);
        }

        if ((result->p_data = MALLOC(result->u_banks)) != nil)
        {
            memcpy(result->p_data, pst_map->p_data, result->u_banks);
        }
        else
            result = (PBINMAP)binmap2_destroy((HBINMAP)result);
    }

    RETURN((HBINMAP)result);
}
// ***************************************************************************
// FUNCTION
//      binmap2_raw_copy
// PURPOSE
//
// PARAMETERS
//      HBINMAP h_map  --
//      PU8   target --
//      UINT    first  --
//      UINT    banks  --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL binmap2_raw_copy(HBINMAP   h_map,
                      PU8     target,
                      UINT      first,
                      UINT      size)
{
    BOOL            result      = false;
    PBINMAP         pst_map     = (PBINMAP)h_map;
    UINT            tail;

    ENTER(true);

//printf("binmap2_raw_copy stage %u %u (%u)\n", first, size, pst_map->u_banks);

    if (pst_map && (first < pst_map->u_banks))
    {
        tail = pst_map->u_banks - first;

        memcpy(target, pst_map->p_data + first, MAC_MIN(tail, size));

        if (size > tail)
        {
            //printf("binmap2_raw_copy stage size %u tail %u diff %d copied %u\n", size, tail, size - tail, MAC_MIN(tail, size));
            memset(target + tail, 0xFF, size - tail);
        }

        result = true;
    }

//    printf("binmap2_raw_copy stage - %s\n", MAC_TF(result));

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      binmap2_and
// PURPOSE
//
// PARAMETERS
//      HBINMAP h_taget  --
//      HBINMAP h_source --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL binmap2_and(HBINMAP   h_taget,
                 HBINMAP   h_source)
{
    BOOL            result          = false;
    PBINMAP         tgt         = (PBINMAP)h_taget;
    PBINMAP         src         = (PBINMAP)h_source;
    PU8           ont         = tgt->p_data;
    PU8           ons         = src->p_data;
    UINT            idx;

    ENTER(true);

    if (tgt->u_bits == src->u_bits)
    {
        for (idx = 0; idx < tgt->u_banks; idx++, ont++, ons++)
        {
            *ont |= *ons;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      binmap2_ones_count
// PURPOSE
//
// PARAMETERS
//      HBINMAP h_map --
// RESULT
//      UINT --
// ***************************************************************************
UINT binmap2_ones_count(HBINMAP h_map,
                        UINT    end)
{
    UINT        result      = 0;
    PBINMAP     map         = (PBINMAP)h_map;
    UINT        left;
    PU8       on;

    ENTER(map);

    end     = MAC_MIN(end, map->u_bits);
    on      = map->p_data;
    left    = ax_parts(end, 8);

    while (left--)
    {
        result += left ? ones_count(*on) : ones_count(*on & TAIL_ONES(end));

        on++;
    }

    RETURN(result);
}

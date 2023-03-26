// ***************************************************************************
// TITLE
//      Croesus Format Handing in Mastering Mode
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

#include <arsenal.h>

#include <pvt_resources.h>


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      croesus_mm_close
// PURPOSE
//
// PARAMETERS
//      HCROESUSMM   h_croesus --
// RESULT
//      HCROESUSMM  --
// ***************************************************************************
HCROESUSMM croesus_mm_close(HCROESUSMM h_croesus)
{
    PCROESUSMM      pst_M     = (PCROESUSMM)h_croesus;

    ENTER(pst_M);

    if (pst_M->pst_R)
        croesus_close((HCROESUS)pst_M->pst_R);

    if (pst_M->h_target)
    {
        fclose(pst_M->h_target);
    }

    FREE(pst_M);

    RETURN(NULL);
}
// ***************************************************************************
// FUNCTION
//      croesus_mm_open
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_filename --
//      UINT   u_flags      --
// RESULT
//      HCROESUSMM  --
// ***************************************************************************
HCROESUSMM croesus_mm_open(PSTR     psz_filename,
                           ENDIAN   v_endian)
{
    PCROESUSMM  pst_M     = NULL;

    ENTER(psz_filename);

    if ((pst_M = CREATE(CROESUSMM)) != NULL)
    {
        if (v_endian && (v_endian != ENDIAN_current))
        {
            pst_M->b_flip_bytes = true;
        }

        if ((pst_M->pst_R = (PCROESUS)croesus_open(psz_filename)) != NULL)
        {
            if ( !( ((pst_M->h_target = fopen(psz_filename, "r+b")) != NULL)        &&
                    !fseek(pst_M->h_target, pst_M->pst_R->u_enum_offset, SEEK_SET) ))
            {
                pst_M = (PCROESUSMM)croesus_mm_close((HCROESUSMM)pst_M);
            }
        }
        else
        {
            if (  (  ((pst_M->h_target = fopen(psz_filename, "r+b")) != NULL)   ||
                     ((pst_M->h_target = fopen(psz_filename, "w+b")) != NULL)   )   &&
                  ((pst_M->pst_R   = _croesus_open_base(psz_filename)) != NULL)     )
            {
                fseek(pst_M->h_target, 0, SEEK_END);

                pst_M->pst_R->u_main_offset = ftell(pst_M->h_target);
            }
            else
                pst_M = (PCROESUSMM)croesus_mm_close((HCROESUSMM)pst_M);
        }
    }

    RETURN((HCROESUSMM)pst_M);
}
// ***************************************************************************
// FUNCTION
//      _loc_saver
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PCROESUS   pst_M   --
//      PLOC       pst_loc --
// RESULT
//      BOOL --
// ***************************************************************************
static BOOL _loc_saver(PCROESUSMM   pst_M,
                       PLOC         pst_loc,
                       UINT         u_size,
                       PUINT        pu_id)
{
    BOOL            b_result        = false;
    OFFS            st_offs;
    POFFS           pst_offs;

    ENTER(pst_M && pst_loc);

    if (pst_M->b_flip_bytes)
    {
        st_offs.u_id        = MAC_FLIP_U32(pst_loc->st_offs.u_id);
        st_offs.u_type      = MAC_FLIP_U32(pst_loc->st_offs.u_type);
        st_offs.u_offset    = MAC_FLIP_U32(pst_loc->st_offs.u_offset);
        st_offs.u_size      = MAC_FLIP_U32(pst_loc->st_offs.u_size);

        pst_offs            = &st_offs;
    }
    else
        pst_offs            = &pst_loc->st_offs;

    b_result = fwrite(pst_offs, sizeof(OFFS), 1, pst_M->h_target);

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      croesus_save
// PURPOSE
//
// PARAMETERS
//      HCROESUS   h_croesus --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL croesus_mm_save(HCROESUSMM h_croesus)
{
    BOOL        b_result        = false;
    PCROESUSMM  pst_M           = (PCROESUSMM)h_croesus;
    HEADER      st_header;
    ENUM        st_enum;

    ENTER(pst_M);

    st_enum.u_id            = id_KRSM;
    st_enum.u_number        = pst_M->u_added + pst_M->pst_R->u_number;

    st_header.u_magic       = id_KREZ;
    st_header.u_enum_offset = ftell(pst_M->h_target) - sizeof(HEADER);
    st_header.u_main_offset = pst_M->pst_R->u_main_offset - sizeof(HEADER);

    if (pst_M->b_flip_bytes)
    {
        st_enum.u_id        = MAC_FLIP_U32(st_enum.u_id);
        st_enum.u_number    = MAC_FLIP_U32(st_enum.u_number);
    }

    if (  fwrite(&st_enum, sizeof(ENUM), 1, pst_M->h_target)                    &&
          recmap_enum(pst_M->pst_R->h_resmap, (PFNRECENUMX)_loc_saver, pst_M) )
    {
        st_header.u_enum_offset -= ftell(pst_M->h_target);
        st_header.u_main_offset -= ftell(pst_M->h_target);

        if (pst_M->b_flip_bytes)
        {
            st_header.u_magic        = MAC_FLIP_U32(st_header.u_magic);
            st_header.u_enum_offset  = MAC_FLIP_U32(st_header.u_enum_offset);
            st_header.u_main_offset  = MAC_FLIP_U32(st_header.u_main_offset);
        }

        if (fwrite(&st_header, sizeof(HEADER), 1, pst_M->h_target))
        {
            b_result = true;
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      croesus_mm_add
// PURPOSE
//
// PARAMETERS
//      HCROESUSMM   h_croesus --
//      UINT         u_id      --
//      UINT         u_type    --
//      PVOID        p_data    --
//      UINT         u_size    --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL croesus_mm_add(HCROESUSMM      h_croesus,
                    UINT            u_id,
                    UINT            u_type,
                    PVOID           p_data,
                    UINT            u_size)
{
    BOOL            b_result    = false;
    PCROESUSMM      pst_M           = (PCROESUSMM)h_croesus;
    LOC             st_loc;

    ENTER(pst_M);

    memset(&st_loc, 0, sizeof(LOC));
    st_loc.st_offs.u_id      = u_id;
    st_loc.st_offs.u_type    = u_type;
    st_loc.st_offs.u_size    = u_size;
    st_loc.st_offs.u_offset  = (signed)pst_M->pst_R->u_main_offset - ftell(pst_M->h_target);

    if (  recmap_add_copy(pst_M->pst_R->h_resmap,
                          &u_id, &st_loc, sizeof(LOC))   &&
          fwrite(p_data, u_size, 1, pst_M->h_target)    )
    {
        pst_M->u_added++;

        b_result = true;
    }

    RETURN(b_result);
}


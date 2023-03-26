// ***************************************************************************
// TITLE
//      Croesus Format Handing
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

#include <axsystem.h>

#include <pvt_resources.h>


// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      _res_clean
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PGROUP   pst_G --
// RESULT
//      BOOL --
// ***************************************************************************
static PLOC _res_free(PLOC pst_loc)
{
    ENTER(true);

    if (pst_loc)
    {
        if (pst_loc->p_at_mem)
        {
            FREE(pst_loc->p_at_mem);
        }

        FREE(pst_loc);
    }

    RETURN(NULL);
}
// ***************************************************************************
// FUNCTION
//      _load_info
// TYPE
//      static
// PURPOSE
//
// PARAMETERS
//      PCROESUS   pst_R  --
//      FILE *     pst_R->h_source --
// RESULT
//      BOOL --
// ***************************************************************************
static BOOL _load_info(PCROESUS pst_R)
{
    BOOL            b_result        = false;
    HEADER          st_header;
    ENUM            st_enum;
    LOC             st_loc;
    U32          u_cnt;

    ENTER(true);

    if (  !fseek(pst_R->h_source, -(signed)sizeof(HEADER), SEEK_END)         &&
          fread(&st_header, sizeof(HEADER), 1, pst_R->h_source)     &&
          (st_header.u_magic == id_KREZ)                            &&
          !fseek(pst_R->h_source, (signed)st_header.u_main_offset, SEEK_CUR) &&
          (((pst_R->u_main_offset = ftell(pst_R->h_source)) != 0) || true)   &&
          !fseek(pst_R->h_source, 0, SEEK_END)                               &&
          !fseek(pst_R->h_source, (signed)st_header.u_enum_offset, SEEK_CUR) &&
          fread(&st_enum, sizeof(ENUM), 1, pst_R->h_source)                  )
    {
        pst_R->u_enum_offset = ftell(pst_R->h_source) - sizeof(ENUM);

        switch (st_enum.u_id)
        {
            case id_KRSM:
                pst_R->u_number = st_enum.u_number;
                b_result        = true;

                memset(&st_loc, 0, sizeof(st_loc));

                for (u_cnt = st_enum.u_number; b_result && u_cnt; u_cnt--)
                {
                    if ( !( fread(&st_loc.st_offs, sizeof(OFFS), 1, pst_R->h_source) &&
                            recmap_add_copy_fast(pst_R->h_resmap,
                               &st_loc.st_offs.u_id, &st_loc, sizeof(LOC))   ))
                    {
                        b_result = false;
                    }
                }
                break;

            default:
                break;
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      croesus_close
// PURPOSE
//
// PARAMETERS
//      HCROESUS   h_croesus --
// RESULT
//      HCROESUS  --
// ***************************************************************************
HCROESUS croesus_close(HCROESUS h_croesus)
{
    PCROESUS    pst_R     = (PCROESUS)h_croesus;

    ENTER(pst_R);

    if (pst_R->h_resmap)
        recmap_destroy(pst_R->h_resmap);

    if (pst_R->h_source)
        fclose(pst_R->h_source);

    FREE(pst_R);

    RETURN(NULL);
}
// ***************************************************************************
// FUNCTION
//      _croesus_open_base
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_filename --
// RESULT
//      PCROESUS  --
// ***************************************************************************
PCROESUS _croesus_open_base(PCSTR psz_filename)
{
    PCROESUS        pst_R     = NULL;

    ENTER(psz_filename);

    if ((pst_R = CREATE(CROESUS)) != NULL)
    {
        if ( !( ((pst_R->h_source = fopen(psz_filename, "rb")) != NULL) &&
                ((pst_R->h_resmap = recmap_create(16 KB, sizeof(U32),
                                     (PFNRECFREE)_res_free)) != NULL) ))
        {
            pst_R = (PCROESUS)croesus_close((HCROESUS)pst_R);
        }
    }

    RETURN(pst_R);
}
static PCSTR _get_module(PSTR   pmodule,
                         UINT   size)
{
    PCSTR       result  = NULL;

    ENTER(pmodule && size);

    if (axmodule_get_filename(pmodule, size))
    {
        result = (PCSTR)pmodule;
    }

    RETURN(result);
}

// ***************************************************************************
// FUNCTION
//      croesus_open
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_filename --
// RESULT
//      HCROESUS  --
// ***************************************************************************
HCROESUS croesus_open(PCSTR psz_filename)
{
    PCROESUS        pst_R           = NULL;
    PCSTR           pmodule;
    CHAR            sz_module       [ AXLPATH ];

    ENTER(true);

    if (  ((pmodule = psz_filename) != NULL)                    ||
          ((pmodule = _get_module(sz_module, AXLPATH)) != NULL) )
    {
        if ((pst_R = _croesus_open_base(pmodule)) != NULL)
        {
            if (!_load_info(pst_R))
            {
                pst_R = (PCROESUS)croesus_close((HCROESUS)pst_R);
            }
        }
    }

    RETURN((HCROESUS)pst_R);
}
// ***************************************************************************
// FUNCTION
//      croesus_get
// PURPOSE
//      Get resource
// PARAMETERS
//      PCROESUS pst_R -- Pointer to Resource control structure
//      U32       d_id        -- ID of resource
//      PU32      pd_type     -- Pointer to U32 which receives type
//                                 of resource, can be NULL
//      PU32      pd_size     -- Pointer to U32 which receives size
//                                 of resource, can be NULL
// RESULT
//      PVOID -- Pointer to resource data
// ***************************************************************************
PVOID croesus_get(HCROESUS          h_croesus,
                  UINT              u_id,
                  PUINT             pu_type,
                  PUINT             pu_size)
{
    PVOID           p_ptr       = NULL;
    PCROESUS        pst_R       = (PCROESUS)h_croesus;
    INT             i_offset;
    PLOC            pst_loc;

    ENTER(pst_R && pst_R->h_source);

    if ((pst_loc = recmap_get(pst_R->h_resmap, &u_id, NULL)) != NULL)
    {
        if (!pst_loc->p_at_mem)
        {
            if ((pst_loc->p_at_mem = MALLOC(pst_loc->st_offs.u_size)) != NULL)
            {
                i_offset = (signed)pst_R->u_main_offset -
                           (signed)pst_loc->st_offs.u_offset;

                if ( !( !fseek(pst_R->h_source, i_offset, SEEK_SET)         &&
                        fread(pst_loc->p_at_mem,
                            pst_loc->st_offs.u_size, 1, pst_R->h_source)    ))
                {
                    FREE(pst_loc->p_at_mem);

                    pst_loc->p_at_mem = NULL;
                }
            }
        }

        p_ptr = pst_loc->p_at_mem;

        if (pu_type)
            *pu_type = pst_loc->st_offs.u_type;

        if (pu_size)
            *pu_size = pst_loc->st_offs.u_size;
    }

    RETURN(p_ptr);
}
// ***************************************************************************
// FUNCTION
//      croesus_free
// PURPOSE
//
// PARAMETERS
//      HCROESUS   h_croesus --
//                 UINT3u_id --
// RESULT
//      void  --
// ***************************************************************************
void croesus_free(HCROESUS          h_croesus,
                  U32            u_id)
{
    PCROESUS        pst_R       = (PCROESUS)h_croesus;
    PLOC            pst_loc;

    ENTER(pst_R && pst_R->h_source);

    if ((pst_loc = (PLOC)recmap_get(pst_R->h_resmap, &u_id, NULL)) != NULL)
    {
        if (pst_loc->p_at_mem)
        {
            FREE(pst_loc->p_at_mem);
            pst_loc->p_at_mem = NULL;
        }
    }

    QUIT;
}


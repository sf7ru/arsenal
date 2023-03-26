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

#ifndef __RECORDSH__
#define __RECORDSH__

#include <arsenal.h>

#include <axdata.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define recarray_create(u_rec_size,u_rec_num,u_reloc_amount,pfn_clean)  \
                            recarray_create_x(u_rec_size,u_rec_num,u_reloc_amount,pfn_clean,NULL)

#define recset_create(u_initial, pfn_free)      recset_create_x(u_initial, u_initial, pfn_free)


#define recarray_unserialize(a,b,c,d)   recarray_unserialize_x(a,b,c,d,NULL)


#define recset_get(a,b)         recset_get_x(a,b,nil)

#define recset_cat(a,b)         recset_cat_x(a,b,true)
#define recset_cat_fast(a,b)    recset_cat_fast_x(a,b,true)

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// ---------------------------------------------------------------------------

typedef BOOL  (*PFNRECCLEAN)(PVOID);
typedef BOOL  (*PFNRECCLEAN2)(PVOID,PVOID);
typedef PVOID (*PFNRECFREE)(PVOID);
typedef PVOID (*PFNRECSER)(PVOID,PUINT);
typedef PVOID (*PFNRECUNSER)(PVOID,PUINT);
typedef BOOL  (*PFNRECENUM)(PVOID,PVOID,UINT,UINT);
typedef PVOID (*PFNRECCOPY)(PVOID,UINT);
typedef BOOL  (*PFNRECENUMX)(PVOID,PVOID,UINT,PVOID);

typedef struct _tag_RECSET      { PVOID p; } * HRECSET;
typedef struct _tag_RECARRAY    { PVOID p; } * HRECARRAY;
typedef struct _tag_RECQUEUE    { PVOID p; } * HRECQUEUE;
typedef struct _tag_NDXQUEUE    { PVOID p; } * HNDXQUEUE;
typedef struct _tag_PTRQUEUE    { PVOID p; } * HPTRQUEUE;
typedef struct _tag_STRUCTQUEUE { PVOID p; } * HSTRUCTQUEUE;
typedef struct _tag_RECMAP      { PVOID p; } * HRECMAP;
typedef struct _tag_RECMAP2     { PVOID p; } * HRECMAP2;
typedef struct _tag_HIDMAP      { PVOID p; } * HIDMAP;

typedef struct __tag_IDREC
{
    UINT    id;                 // Identifier must be the first field to
                                // use with recmap2_* functions
} IDREC, * PIDREC;

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif                                      //  #ifdef __cplusplus


// RecArray

HRECARRAY           recarray_destroy_noclean    (HRECARRAY          h_array);

HRECARRAY           recarray_destroy            (HRECARRAY          h_array);

HRECARRAY           recarray_create_x           (UINT               u_rec_size,
                                                 UINT               u_rec_num,
                                                 UINT               u_reloc_amount,
                                                 PFNRECCLEAN        pfn_clean,
                                                 PVOID              p_ptr);

BOOL                recarray_clean              (HRECARRAY          h_array);

UINT                recarray_add                (HRECARRAY          h_array,
                                                 PVOID              p_data);

UINT                recarray_add_array          (HRECARRAY          h_array,
                                                 PVOID              p_data,
                                                 UINT               u_recs);

BOOL                recarray_del                (HRECARRAY          h_array,
                                                 UINT               u_index);

BOOL                recarray_ins                (HRECARRAY          h_array,
                                                 UINT               u_index,
                                                 PVOID              p_data);

BOOL                recarray_ovr                (HRECARRAY          h_array,
                                                 UINT               u_index,
                                                 PVOID              p_data);

PVOID               recarray_get                (HRECARRAY          h_array,
                                                 UINT               u_index);

PVOID               recarray_get_last           (HRECARRAY          h_array);

UINT                recarray_get_idx            (HRECARRAY          h_array,
                                                 PVOID              p_data);

UINT                recarray_count              (HRECARRAY          h_array);

UINT                recarray_total              (HRECARRAY          h_array);

UINT                recarray_recsize            (HRECARRAY          h_array);

BOOL                recarray_cat                (HRECARRAY          pst_target,
                                                 HRECARRAY          pst_source);

HRECARRAY           recarray_dup                (HRECARRAY          h_array);

HRECARRAY           recarray_conv               (UINT               u_recsize,
                                                 PVOID              p_data,
                                                 UINT               u_size);

PVOID               recarray_freeze             (HRECARRAY          h_array,
                                                 PUINT              pu_records);

UINT                recarray_data_size          (HRECARRAY          h_array);

UINT                recarray_data_copy          (HRECARRAY          h_array,
                                                 PVOID              data,
                                                 UINT               first,
                                                 UINT               count);

PVOID               recarray_data_get           (HRECARRAY          h_array);

BOOL                recarray_minimize           (HRECARRAY          h_array);

HRECARRAY           recarray_load               (PCSTR              psz_filename);

HRECARRAY           recarray_load_raw           (PCSTR              psz_filename,
                                                 UINT               u_rec_size,
                                                 UINT               u_reloc_amount);

BOOL                recarray_save               (HRECARRAY          h_array,
                                                 PCSTR              psz_filename);

PVOID               recarray_serialize          (HRECARRAY          h_array,
                                                 PFNRECSER          pfn_ser,
                                                 PUINT              pu_size);

HRECARRAY           recarray_unserialize_x      (PVOID              p_serialized,
                                                 PFNRECUNSER        pfn_unser,
                                                 UINT               u_size,
                                                 PFNRECCLEAN        pfn_clean,
                                                 PVOID              p_ptr);

// RecSet

BOOL                recset_cat_x                (HRECSET            h_set,
                                                 HRECSET            h_add,
                                                 BOOL               clean);

BOOL                recset_cat_fast_x           (HRECSET            h_set,
                                                 HRECSET            h_add,
                                                 BOOL               clean);


HRECSET             recset_destroy_nofree       (HRECSET            h_set);

HRECSET             recset_destroy              (HRECSET            h_recset);

HRECSET             recset_create_x             (UINT               u_initial,
                                                 UINT               u_reloc,
                                                 PFNRECFREE         pfn_free);


BOOL                recset_clean                (HRECSET            h_recset);

UINT                recset_count                (HRECSET            h_recset);

PVOID               recset_get_x                (HRECSET            h_recset,
                                                 UINT               u_index,
                                                 PUINT              pu_size);

PVOID               recset_get_last             (HRECSET            h_recset);

UINT                recset_add                  (HRECSET            h_recset,
                                                 PVOID              p_data,
                                                 UINT               u_size);

UINT                recset_add_fast             (HRECSET            h_recset,
                                                 PVOID              p_data,
                                                 UINT               u_size);

UINT                recset_add_copy             (HRECSET            h_recset,
                                                 PVOID              p_data,
                                                 UINT               u_size);

BOOL                recset_ovr                  (HRECSET            h_recset,
                                                 UINT               u_index,
                                                 PVOID              p_data,
                                                 UINT               u_size);

BOOL                recset_ovr_copy             (HRECSET            h_recset,
                                                 UINT               u_index,
                                                 PVOID              p_data,
                                                 UINT               u_size);

BOOL                recset_del                  (HRECSET            h_recset,
                                                 UINT               u_index);

BOOL                recset_del_nofree           (HRECSET            h_recset,
                                                 UINT               u_index);

BOOL                recset_enum                 (HRECSET            h_set,
                                                 PFNRECENUM         pfn_enum,
                                                 PVOID              p_ptr);

UINT                recset_find                 (HRECSET            h_set,
                                                 PFNRECENUM         pfn_enum,
                                                 PVOID              p_ptr);

BOOL                recset_save                 (UINT               version,
                                                 HRECSET            h_set,
                                                 PFNRECSER          pfn_serialize,
                                                 PCSTR              psz_filename);

HRECSET             recset_load                 (UINT               version,
                                                 PCSTR              psz_filename,
                                                 PFNRECUNSER        pfn_unser,
                                                 PFNRECFREE         pfn_free);

BOOL                recqueue_add                (HRECQUEUE          h_queue,
                                                 PVOID              p_data);

BOOL                recqueue_replace            (HRECQUEUE          h_queue,
                                                 PVOID              original,
                                                 PVOID              replacement);

HRECQUEUE           recqueue_destroy            (HRECQUEUE          h_queue);

HRECQUEUE           recqueue_create             (UINT               u_rec_size,
                                                 UINT               u_init_size,
                                                 UINT               u_max_size,
                                                 PFNRECCLEAN        pfn_clean);

PVOID               recqueue_peek               (HRECQUEUE          h_queue);

PVOID               recqueue_peek_x             (HRECQUEUE          h_queue,
                                                 UINT               index);

BOOL                recqueue_del                (HRECQUEUE          h_queue,
                                                 UINT               index);

BOOL                recqueue_enum               (HRECQUEUE          h_queue,
                                                 PFNRECENUM         pfn_enum,
                                                 PVOID              p_ptr);

UINT                recqueue_count              (HRECQUEUE          h_queue);

BOOL                recqueue_clean              (HRECQUEUE          h_queue);

HRECARRAY           recqueue_to_recarray        (HRECQUEUE          h_queue,
                                                 BOOL               clean);

BOOL                recqueue_get                (HRECQUEUE          h_queue,
                                                 PVOID              p_data);

//BOOL                recqueue_raw_get            (HRECQUEUE          h_queue,
//                                                 PA7DATAX           pst_data);
//
//BOOL                recqueue_raw_set            (HRECQUEUE          h_queue,
//                                                 PA7DATAX           pst_data);

UINT                recqueue_get_rollback_point (HRECQUEUE          h_queue);

void                recqueue_rollback           (HRECQUEUE          h_queue,
                                                 UINT               point);

BOOL                recqueue_save               (UINT               version,
                                                 HRECQUEUE          h_queue,
                                                 PFNRECSER          pfn_serialize,
                                                 PCSTR              psz_filename);

HRECQUEUE           recqueue_load               (UINT               version,
                                                 PCSTR              psz_filename,
                                                 PFNRECUNSER        pfn_unser,
                                                 PFNRECCLEAN        pfn_clean);


HSTRUCTQUEUE        structqueue_destroy         (HSTRUCTQUEUE       h_queue);

HSTRUCTQUEUE        structqueue_create          (UINT               structSize,
                                                 UINT               queueSize);

PVOID               structqueue_peek            (HSTRUCTQUEUE       h_queue);

PVOID               structqueue_get             (HSTRUCTQUEUE       h_queue);

PVOID               structqueue_put_begin       (HSTRUCTQUEUE       h_queue);

void                structqueue_put_commit      (HSTRUCTQUEUE       h_queue);



BOOL                ptrqueue_put                (HPTRQUEUE          h_queue,
                                                 PVOID              ptr);

PVOID               ptrqueue_get                (HPTRQUEUE          h_queue);

PVOID               ptrqueue_peek               (HPTRQUEUE          h_queue);

HPTRQUEUE           ptrqueue_create             (UINT               u_size);

HPTRQUEUE           ptrqueue_destroy            (HPTRQUEUE          h_queue);

UINT                ptrqueue_size               (HPTRQUEUE          h_queue);

UINT                ptrqueue_count              (HPTRQUEUE          h_queue);

void                ptrqueue_clean              (HPTRQUEUE          h_queue,
                                                 PFNRECFREE         pfn_free);


BOOL                recmap_clean                (HRECMAP            h_recmap);

HRECMAP             recmap_destroy              (HRECMAP            h_recmap);

HRECMAP             recmap_create               (UINT               u_hash,
                                                 UINT               u_id_size,
                                                 PFNRECFREE         pfn_free);

PVOID               recmap_get                  (HRECMAP            h_recmap,
                                                 PVOID              p_id,
                                                 PUINT              pu_size);

BOOL                recmap_del                  (HRECMAP            h_recmap,
                                                 PVOID              p_id);

BOOL                recmap_add                  (HRECMAP            h_recmap,
                                                 PVOID              p_id,
                                                 PVOID              p_data,
                                                 UINT               u_size);

BOOL                recmap_add_fast             (HRECMAP            h_recmap,
                                                 PVOID              p_id,
                                                 PVOID              p_data,
                                                 UINT               u_size);

BOOL                recmap_add_copy             (HRECMAP            h_recmap,
                                                 PVOID              p_id,
                                                 PVOID              p_data,
                                                 UINT               u_size);

BOOL                recmap_add_copy_fast        (HRECMAP            h_recmap,
                                                 PVOID              p_id,
                                                 PVOID              p_data,
                                                 UINT               u_size);

BOOL                recmap_ovr                  (HRECMAP            h_recmap,
                                                 PVOID              p_id,
                                                 PVOID              p_data,
                                                 UINT               u_size);

BOOL                recmap_ovr_copy             (HRECMAP            h_recmap,
                                                 PVOID              p_id,
                                                 PVOID              p_data,
                                                 UINT               u_size);

BOOL                recmap_enum                 (HRECMAP            h_recmap,
                                                 PFNRECENUMX        pfn_enum,
                                                 PVOID              p_ptr);

BOOL                recmap_save                 (UINT               version,
                                                 HRECMAP            h_map,
                                                 PFNRECSER          pfn_serialize,
                                                 PCSTR              psz_filename);

HRECMAP             recmap_load                 (UINT               version,
                                                 PCSTR              psz_filename,
                                                 PFNRECUNSER        pfn_unserialize,
                                                 PFNRECFREE         pfn_free);

BOOL                recmap_cat                  (HRECMAP            h_recmap,
                                                 HRECMAP            h_add,
                                                 PFNRECCOPY         pfn_copy);

BOOL                recmap_check                (HRECMAP            h_recmap,
                                                 UINT               u_hash,
                                                 UINT               u_id_size);


HIDMAP              idmap_destroy               (HIDMAP             h_map);

HIDMAP              idmap_create                (UINT               init,
                                                 UINT               allocAmount,
                                                 UINT               idSize,
                                                 PFNRECFREE         pfn_free);

BOOL                idmap_clean                 (HIDMAP             h_map);

BOOL                idmap_add                   (HIDMAP             h_map,
                                                 PIDREC             rec);

BOOL                idmap_add_fast              (HIDMAP             h_map,
                                                 PIDREC             rec,
                                                 UINT               whereto);

PIDREC              idmap_get                   (HIDMAP             h_map,
                                                 PVOID              pid,
                                                 PUINT              whereto);

PIDREC              idmap_get_first             (HIDMAP             h_map);

BOOL                idmap_del                   (HIDMAP             h_map,
                                                 PVOID              pid);

UINT                idmap_count                 (HIDMAP             h_map);




HRECMAP2            recmap2_destroy             (HRECMAP2           h_recmap);

HRECMAP2            recmap2_create              (UINT               hashBits,
                                                 UINT               idSize,
                                                 UINT               reallocAmount,
                                                 PFNRECFREE         pfn_free);

BOOL                recmap2_add                 (HRECMAP2           recmap,
                                                 PIDREC             rec);

BOOL                recmap2_add_fast            (HRECMAP2           recmap,
                                                 PIDREC             rec,
                                                 UINT               whereto);

PIDREC              recmap2_get                 (HRECMAP2           h_recmap,
                                                 PVOID              id,
                                                 PUINT              whereto);

PIDREC              recmap2_get_first           (HRECMAP2           h_recmap);

BOOL                recmap2_del                 (HRECMAP2           h_recmap,
                                                 PVOID              p_id);

BOOL                recmap2_clean               (HRECMAP2           h_recmap);

void                recmap2_stat                (HRECMAP2           h_recmap);

#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus

#endif                                      //  #ifndef __RECORDSH__


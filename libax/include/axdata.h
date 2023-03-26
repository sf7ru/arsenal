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


#ifndef __AXDATAH__
#define __AXDATAH__

#include <arsenal.h>
#include <axsystem.h>


// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#if (TARGET_ENDIAN == __AX_BE__)
#define ENDIAN_current              ENDIAN_big
#else
#define ENDIAN_current              ENDIAN_little
#endif

// -------------------------- Bitstream Options ------------------------------

#define AXBSF_BINARY        0x0
#define AXBSF_HEX           0x1



// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// ---------------------------------------------------------------------------

typedef struct _tag_HCROESUS    { UINT u_id; } * HCROESUS;
typedef struct _tag_HCROESUSMM  { UINT u_id; } * HCROESUSMM;
typedef struct _tag_HBINMAP     { UINT u_id; } * HBINMAP;
typedef struct _tag_HBITSTREAM  { UINT u_id; } * HBITSTREAM;

typedef BOOL    (*PFNREADERCB)(PVOID userptr, PVOID data, UINT size, UINT offset);

// ***************************************************************************
// STRUCTURE
//      AXDATA
// PURPOSE
//      Data
// ***************************************************************************
typedef struct __tag_AXDATA
{
    UINT            u_size;
    U8              ab_data	    [ 1 ];
} AXDATA, * PAXDATA;

// ***************************************************************************
// STRUCTURE
//      AXRANGE
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_AXRANGE
{
    UINT            from;
    UINT            to;
} AXRANGE, * PAXRANGE;

// ***************************************************************************
// STRUCTURE
//      AXRANGE
// PURPOSE
//
// ***************************************************************************
typedef struct __tag_AXRANGEX
{
    U64             from;
    U64             to;
} AXRANGEX, * PAXRANGEX;

// ***************************************************************************
// STRUCTURE
//      AXFRANGE
// PURPOSE
//      Numeric range (double)
// ***************************************************************************
typedef struct __tag_AXFRANGE
{
    double          from;
    double          to;
} AXRANGED, * PAXRANGED;

// ***************************************************************************
// STRUCTURE
//      AXDATAX
// PURPOSE
//      Data
// ***************************************************************************
typedef struct __tag_AXDATAX
{
    ULONG           u_size;
    PVOID           p_data;
} AXDATAX, * PAXDATAX;

// ***************************************************************************
// STRUCTURE
//      AXDATAX2
// PURPOSE
//      Data
// ***************************************************************************
typedef struct __tag_AXDATAX2
{
    ULONG           u_size;
    ULONG           u_used;
    PVOID           p_data;
} AXDATAX2, * PAXDATAX2;

// ***************************************************************************
// STRUCTURE
//      AXBUFF
// PURPOSE
//      Buffer with property
// ***************************************************************************
typedef struct __tag_AXBUFF
{
    UINT            u_index;
    UINT            u_used;
    UINT            u_size;
    U8              ab_data	    [ 1 ];
} AXBUFF, * PAXBUFF;


// ***************************************************************************
// STRUCTURE
//      AXRINGBUFF
// PURPOSE
//      Ring buffer with property
// ***************************************************************************
typedef struct __tag_AXRINGBUFF
{
    UINT            head;
    UINT            tail;
    UINT            size;
    U8              data	    [ 1 ];
} AXRINGBUFF, * PAXRINGBUFF;



// ***************************************************************************
// STRUCTURE
//      ENDIAN
// PURPOSE
//
// ***************************************************************************
typedef enum
{
    ENDIAN_default      = 0,
    ENDIAN_little,
    ENDIAN_big
} ENDIAN;

typedef enum
{
    EOLMODE_cr         = 0,
    EOLMODE_lf,
    EOLMODE_crlf
} EOLMODE;

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif                                      //  #ifdef __cplusplus


BOOL                axdatax_free                (PAXDATAX           pst_data);

PAXDATAX            axdatax_destroy             (PAXDATAX           pst_data);

PAXDATAX            axdatax_create              (ULONG              u_size);

PAXDATAX            axdatax_create2             (PVOID              p_data,
                                                 ULONG              u_size);

PAXDATAX            axdatax_dup                 (PAXDATAX           pst_data);

BOOL                axdatax_dup2                (PAXDATAX           pst_tgt,
                                                 PAXDATAX           pst_src);

BOOL                axdatax_resize              (PAXDATAX           pst_data,
                                                 ULONG              u_size);

#define axbuff_destroy(a)          ((PAXBUFF )axutils_destroy(a))
#define axbuff_pstr(a)             ((PSTR)(a)->ab_data)
#define axbuff_clean(a)            (a)->u_used = 0; (a)->u_index = 0
#define axbuff_size(a)             (a)->u_used

UINT                axbuff_pop                  (PAXBUFF            pst_fifo,
                                                 PVOID              p_data,
                                                 UINT               u_size);

UINT                axbuff_push                 (PAXBUFF            pst_fifo,
                                                 PVOID              p_data,
                                                 UINT               u_size);

PAXBUFF             axbuff_create               (UINT               u_size);

#define axbuff_gets_r(a,b,c)    axbuff_gets_x(a,b,'\r',c)
#define axbuff_gets(a,b,c)      axbuff_gets_x(a,b,'\n',c)

INT                 axbuff_gets_x               (PAXBUFF            pst_buff,
                                                 PAXDEV             pst_dev,
                                                 CHAR               c_n,
                                                 UINT               u_mTO);

INT                 axbuff_read                 (PAXBUFF            pst_buff,
                                                 PVOID              p_data,
                                                 UINT               u_size);

INT                 axbuff_append               (PAXBUFF            pst_buff,
                                                 PVOID              p_data,
                                                 UINT               u_size);


INT                 axbuff_recv                 (PAXBUFF            pst_buff,
                                                 PAXDEV             pst_dev,
                                                 UINT               u_mTO);

INT                 axbuff_get_count            (PAXBUFF            pst_buff);

HCROESUS            croesus_close               (HCROESUS           h_croesus);

HCROESUS            croesus_open                (PCSTR              psz_filename);

PVOID               croesus_get                 (HCROESUS           h_croesus,
                                                 UINT               u_id,
                                                 PUINT              pu_type,
                                                 PUINT              pu_size);

void                croesus_free                (HCROESUS           h_croesus,
                                                 U32                u_id);

HCROESUSMM          croesus_mm_close            (HCROESUSMM         h_croesus);

HCROESUSMM          croesus_mm_open             (PSTR               psz_filename,
                                                 ENDIAN             v_endian);

BOOL                croesus_mm_save             (HCROESUSMM         h_croesus);

BOOL                croesus_mm_add              (HCROESUSMM         h_croesus,
                                                 UINT               u_id,
                                                 UINT               u_type,
                                                 PVOID              p_data,
                                                 UINT               u_size);


HBINMAP             binmap_destroy              (HBINMAP            h_map);

HBINMAP             binmap_create               (UINT               u_bits);

BOOL                binmap_copy                 (HBINMAP            h_map,
                                                 PAXDATAX           pst_map);

PAXDATAX            binmap_freeze               (HBINMAP            h_map);

BOOL                binmap_get_raw              (HBINMAP            h_map,
                                                 PAXDATAX           pst_raw);

UINT                binmap_get_raw_size         (HBINMAP            h_map);

BOOL                binmap_get                  (HBINMAP            h_map,
                                                 UINT               u_idx);

BOOL                binmap_mark                 (HBINMAP            h_map,
                                                 UINT               u_idx);

BOOL                binmap_mark_all             (HBINMAP            h_map);

BOOL                binmap_unmark               (HBINMAP            h_map,
                                                 UINT               u_idx);

BOOL                binmap_unmark_all           (HBINMAP            h_map);

UINT                binmap_get_count            (HBINMAP            h_map);

UINT                binmap_get_count_marked     (HBINMAP            h_map,
                                                 UINT               banks);


BOOL                binmap_is_complete          (HBINMAP            h_map);

INT                 ones_count                  (U8                  val);

UINT                base64_dec                  (PVOID              tgt,
                                                 UINT               tgtsize,
                                                 PCSTR              src);


HBINMAP             binmap2_destroy             (HBINMAP            h_map);

HBINMAP             binmap2_create              (UINT               u_bits,
                                                 BOOL               b_ones);

BOOL                binmap2_get                 (HBINMAP            h_map,
                                                 UINT               u_idx);

BOOL                binmap2_mark                (HBINMAP            h_map,
                                                 UINT               u_idx);

BOOL                binmap2_mark_all            (HBINMAP            h_map);

BOOL                binmap2_unmark              (HBINMAP            h_map,
                                                 UINT               u_idx);

BOOL                binmap2_unmark_all          (HBINMAP            h_map);

UINT                binmap2_count               (HBINMAP            h_map);

UINT                binmap2_get_first_zero      (HBINMAP            h_map);

UINT                binmap2_get_next_one        (HBINMAP            h_map,
                                                 UINT               start);

UINT                binmap2_raw_size            (HBINMAP            h_map);

PVOID               binmap2_raw_data            (HBINMAP            h_map);

BOOL                binmap2_raw_copy            (HBINMAP            h_map,
                                                 PU8                target,
                                                 UINT               first,
                                                 UINT               banks);

BOOL                binmap2_is_complete         (HBINMAP            h_map);

BOOL                binmap2_recalc              (HBINMAP            h_map);

HBINMAP             binmap2_fast_extract        (HBINMAP            h_map,
                                                 UINT               bankFrom,
                                                 UINT               bankTo);

UINT                binmap2_ones_count          (HBINMAP            h_map,
                                                 UINT               end);

BOOL                binmap2_and                 (HBINMAP            h_taget,
                                                 HBINMAP            h_source);

HBITSTREAM          bitstream_create            (PVOID              p_data,
                                                 UINT               d_size,
                                                 U8                 b_stream_format);

HBITSTREAM          bitstream_destroy           (HBITSTREAM         h_bits);

U32                 bitstream_get               (HBITSTREAM         h_bits,
                                                 INT                b_number_of_bits);

INT                 bitstream_put               (HBITSTREAM         h_bits,
                                                 INT                b_number_of_bits,
                                                 U32                d_value);

UINT                bitstream_get_actual_size   (HBITSTREAM         h_bits);


PAXRINGBUFF         axringbuff_create           (UINT               size);

void                axringbuff_shake            (PAXRINGBUFF        pst_buff);

INT                 axringbuff_write            (PAXRINGBUFF        pst_buff,
                                                 PVOID              p_data,
                                                 INT                i_size);

#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus

#endif                                      //  #ifndef __AXDATAH__


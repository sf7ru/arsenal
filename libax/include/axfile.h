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

#ifndef __AXFILEH__
#define __AXFILEH__

#include <fcntl.h>

#include <arsenal.h>
#include <axsystem.h>
#include <axtime.h>
#include <axdata.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// ---------------------------------------------------------------------------

#define DEF_UPDIRNAME           ".."
#define DEF_CURRDIRNAME         "."

// axfile_find_x2 flags
#define AXFFF_recurse           0x0001
#define AXFFF_dir               0x0002
#define AXFFF_dir_after         0x0004

#define AXFFF_LIST_DELIMITER    ' '
#define AXFFF_LIST_QUOTE        '\"'

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// ---------------------------------------------------------------------------

typedef struct _tag_HAXFILE {UINT u_id;} * HAXFILE;

// ***************************************************************************
// STRUCTURE
//      AXSTATFS
// PURPOSE
//
// ***************************************************************************
typedef struct _tag_AXSTATFS
{
    UINT                u_type;
    FILESIZEINT         u_total;
    FILESIZEINT         u_free;
    FILESIZEINT         u_avail;
} AXSTATFS, * PAXSTATFS;

// ***************************************************************************
// STRUCTURE
//      AXFILEFIND
// PURPOSE
//
// ***************************************************************************
typedef struct _tag_AXFILEFIND
{
    AXUTIME         v_ctime;
    AXUTIME         v_atime;
    AXUTIME         v_mtime;

    UINT                u_mode;
    FILESIZEINT         u_size;

    PVOID               p_handle;

    CHAR                sz_name             [ AXLPATH ];
} AXFILEFIND, * PAXFILEFIND;


typedef BOOL (*PFNAXFILEFINDCB)(PAXFILEFIND,PSTR,PSTR,PVOID);


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif                                      //  #ifdef __cplusplus



HAXFILE             axfile_close                (HAXFILE            h_file);

HAXFILE             axfile_open                 (PCSTR              psz_fname,
                                                 UINT               u_mode);

INT                 axfile_read                 (HAXFILE            h_file,
                                                 PVOID              p_buffer,
                                                 INT                i_size);

INT                 axfile_write                (HAXFILE            h_file,
                                                 PVOID              p_buffer,
                                                 INT                i_size);

BOOL                axfile_seek                 (HAXFILE            h_file,
                                                 FILESIZEINT        i_offs,
                                                 UINT               u_whence);




// ---------------------



BOOL                axpath_create               (PSTR               psz_path);

BOOL                axpath_create_to_file       (PSTR               psz_path_and_filename);

BOOL                axfile_exist                (PCSTR              psz_filename);

BOOL                axfile_is_dir               (PSTR               psz_path);

FILESIZEINT         axfile_get_size             (PCSTR              psz_fname);

BOOL                axfile_move                 (PCSTR              psz_new,
                                                 PCSTR              psz_old,
                                                 BOOL               b_overwrite);

BOOL                axfile_copy                 (PCSTR              psz_new,
                                                 PCSTR              psz_old,
                                                 BOOL               b_overwrite);


BOOL                axfile_times_get            (PAXDATE            pst_c_time,
                                                 PAXDATE            pst_a_time,
                                                 PAXDATE            pst_m_time,
                                                 PSTR               psz_filename);

BOOL                fname_path_is_relative      (PCSTR              path);

BOOL                fname_change_ext            (PSTR               psz_filename,
                                                 INT                i_len,
                                                 PSTR               psz_ext);

BOOL                fname_change_name           (PSTR               psz_filename,
                                                 INT                i_len,
                                                 PSTR               psz_fname);

BOOL                fname_get_path              (PSTR               psz_path,
                                                 UINT               u_len,
                                                 PCSTR              psz_fname);

BOOL                fname_get_name              (PSTR               psz_name,
                                                 UINT               u_len,
                                                 PCSTR              psz_fname);

BOOL                fname_get_ext               (PSTR               psz_ext,
                                                 UINT               u_len,
                                                 PCSTR              psz_fname);

BOOL                fname_extract_path          (PSTR               psz_fname);

void                fname_slashes               (PSTR               psz_path,
                                                 BOOL               b_forward);

PAXFILEFIND         axfile_find_destroy         (PAXFILEFIND        pst_find);

PAXFILEFIND         axfile_find_first           (PSTR               psz_mask,
                                                 UINT               u_mode);

BOOL                axfile_find_next            (PAXFILEFIND        pst_find);

INT                 axfile_find_X               (PSTR               psz_path,
                                                 PSTR               psz_subpath,
                                                 PSTR               psz_mask,
                                                 UINT               u_flags,
                                                 PFNAXFILEFINDCB    pfn_callback,
                                                 PVOID              p_param);

INT                 axfile_find                 (PSTR               psz_path,
                                                 PSTR               psz_subpath,
                                                 PSTR               psz_mask,
                                                 UINT               u_flags,
                                                 PFNAXFILEFINDCB    pfn_callback,
                                                 PVOID              p_param);

FILESIZEINT         axfile_size_get             (HAXFILE            h_file);

HAXFILE             axfile_close                (HAXFILE            h_file);

HAXFILE             axfile_open                 (PCSTR              psz_fname,
                                                 UINT               u_mode);

INT                 axfile_read                 (HAXFILE            h_file,
                                                 PVOID              p_buffer,
                                                 INT                i_size);

INT                 axfile_write                (HAXFILE            h_file,
                                                 PVOID              p_buffer,
                                                 INT                i_size);

BOOL                axfile_seek                 (HAXFILE            h_file,
                                                 FILESIZEINT        i_offs,
                                                 UINT               u_whence);

FILESIZEINT         axfile_tell                 (HAXFILE            h_file);

BOOL                axfile_size_set             (HAXFILE            h_file,
                                                 FILESIZEINT        i_size);

FILESIZEINT         axfile_size_get             (HAXFILE            h_file);


PVOID               axfile_load                 (PCSTR              p_filename,
                                                 PUINT              pd_size,
                                                 UINT               d_additional_size);

BOOL                axfile_save_data            (PCSTR              p_filename,
                                                 PAXDATA            data);

BOOL                axfile_save                 (PCSTR              p_filename,
                                                 PCVOID             data,
                                                 UINT               size);

BOOL                axfile_remove               (PCSTR              psz_filename);



#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus

#endif  // #ifndef __AXFILEH__


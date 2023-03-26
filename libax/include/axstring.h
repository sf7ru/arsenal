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

#ifndef __AXSTRINGH__
#define __AXSTRINGH__

#include <stdarg.h>

#include <arsenal.h>

#include <axtime.h>
#include <axsystem.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define AXSLUT_FLAG_CASESENS    1

#define LOCHAR(a)       ((a) | 0x20)

#define HEX2NIBBLE(a)   if (((a) >= '0') && ((a) <= '9')) { (a) -= '0'; }    \
                        else { (a) |= 0x20; if (((a) >= 'a') && ((a) <= 'f'))\
                        { (a) -= ('a' - 10); } else { (a) = -1; } }

#define NIBBLE2HEX(a)   if (a < 0xA) (a) += '0'; else (a) += ('A' - 10);

#define SUBSTRS_SEPISFIELD       1
#define SUBSTRS_COPY             2


// ----------------------- Dump function options -----------------------------

//  1st byte - byte separator char
//  2nd byte - dword separator char
//  3rd byte - hex/ ASCII string separator char
//  4th byte - flags
#define AXDUMP_SEP_NONE        0x00202020  // No separators
#define AXDUMP_SEP_STR         0x007C2020  // Separated ASCII string
#define AXDUMP_SEP_STRA        0x00B32020  // Separated ASCII string with pseudo gfx
#define AXDUMP_SEP_DD_STR      0x007C7C20  // Separated dwords and string
#define AXDUMP_SEP_DD_STRA     0x00B3B320  // Separated dwords and string
#define AXDUMP_SHOW_UNPRINT    0x01000000  // Show unprila6ble
#define AXDUMP_ADD_SEP_SPACE   0x02000000  // Add extra spaces to separators
#define AXDUMP_SEP_DEFAULT     (AXDUMP_SEP_STR     | AXDUMP_ADD_SEP_SPACE)
#define AXDUMP_SEP_DEFAULTA    (AXDUMP_SEP_STRA    | AXDUMP_ADD_SEP_SPACE)
#define AXDUMP_SEP_QVIEW       (AXDUMP_SEP_DD_STR  | AXDUMP_ADD_SEP_SPACE)
#define AXDUMP_SEP_QVIEWA      (AXDUMP_SEP_DD_STRA | AXDUMP_ADD_SEP_SPACE)

// --------------------------- strz_to_date -------------------------------

#define DEFTFMT_DM              0x0001      // Date is day/month (YMD or DMY)
#define DEFTFMT_MD              0x0002      // Date is month/day (YDM or MDY)


#define DEF_OCTET_CHARS         2
#define MAC_HEX2BIN(a)          (((a) | 0x20) - ((a) > '9' ? 87 : 48))
#define MAC_BIN2HEX(a)          (((a) & 0xf) + (((a) & 0xf) > 9 ? 55 : 48))



#define AXLURL              256
#define axLURLX             2048

#define AXLTINY             32
#define AXLSHORT            64
#define AXLMEDIUM           128
#define AXLLONG             256
#define AXLBIG              512
#define AXLHUGE             1024
#define AXLTEXT             4096
#define AXLISTDELIM         ' '
#define AXLISTQUOTE         '\"'
#define AXLMED              AXLMEDIUM

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// ---------------------------------------------------------------------------

typedef struct _tag_HAXSLUT   { UINT u_id; } * HAXSLUT;

typedef struct _tag_HAXSTR    { UINT u_id; } * HAXSTR;
typedef struct _tag_HSUBSTRS  { UINT u_id; } * HSUBSTRS;

typedef INT (*PFNPUTCHAR)(PVOID,CHAR);

typedef PSTR (*PFNAXKEYCB)(PVOID,PSTR,UINT,BOOL*);

typedef PCSTR (*PFNTIMEUNITNAME)(TIMEUNIT uint, UINT num);



// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif                                      //  #ifdef __cplusplus

HAXSTR              axstr_destroy               (HAXSTR             h_str);

HAXSTR              axstr_create                (UINT               u_initial);

HAXSTR              axstr_dup                   (HAXSTR             h_source);

BOOL                axstr_add                   (HAXSTR             h_target,
                                                 HAXSTR             h_source);

BOOL                axstr_copy                  (HAXSTR             h_string,
                                                 PSTR               psz_string);

PSTR                axstr_c                     (HAXSTR             h_source);

PSTR                axstr_freeze_c              (HAXSTR             h_source);

void                axstr_set_max_len           (HAXSTR             h_source,
                                                 UINT               u_max_len);


HAXSTR              axstr_dup_c                 (PSTR               psz_source);

BOOL                axstr_add_c                 (HAXSTR             h_target,
                                                 PCSTR              psz_source);

BOOL                axstr_add_char              (HAXSTR             h_target,
                                                 CHAR               c_char);

BOOL                axstr_add_raw               (HAXSTR             h_target,
                                                 PVOID              data,
                                                 UINT               size);

BOOL                axstr_copy_c                (HAXSTR             h_string,
                                                 PSTR               psz_string);

BOOL                axstr_add_tabs              (HAXSTR             h_string,
                                                 UINT               tabs);

BOOL                axstr_add_tabs_w_spaces     (HAXSTR             h_string,
                                                 UINT               tabs,
                                                 UINT               tabSize);

UINT                axstr_size                  (HAXSTR             h_source);

UINT                axstr_len                   (HAXSTR             h_source);

UINT                axstr_truncate              (HAXSTR             h_source,
                                                 UINT               len);

PSTR                strz_trim                   (PSTR               psz_string);

PSTR                strz_trim_s                 (PSTR               psz_string,
                                                 UINT               u_len);

PSTR                strz_clean                  (PSTR               psz_string);

PSTR                strz_cpy                    (PSTR               psz_target,
                                                 PCSTR              psz_source,
                                                 UINT               u_size);

PSTR                strz_cat                    (PSTR               psz_target,
                                                 PCSTR              psz_source,
                                                 UINT               u_size);

BOOL                strz_copy                   (PSTR               psz_target,
                                                 UINT               u_tgt_size,
                                                 PCSTR              psz_source,
                                                 UINT               u_src_size);

PSTR                strz_replace                (PSTR               psz_string,
                                                 CHAR               from,
                                                 CHAR               to);

PSTR                strz_mem                    (PVOID              data,
                                                 UINT               size);

BOOL                strz_check                  (PCSTR              psz_string,
                                                 UINT               u_size);

PSTR                strz_dup                    (PCSTR              psz_string);

PVOID               strz_memdup                 (PCVOID             data,
                                                 UINT               size);

PSTR                strz_mem2strz               (PCVOID             data,
                                                 UINT               size);

UINT                strz_size                   (PCSTR              psz_string,
                                                 UINT               u_size);

UINT                strz_len                    (PCSTR              psz_string,
                                                 UINT               u_size);

//PSTR                strz_from_AXRDS             (AXRDS              v_status);
//
//PSTR                strz_from_MPRDS             (MPRDS              v_status);
//

UINT                strz_from_date              (PSTR               target,
                                                 UINT               size,
                                                 PCSTR              templ,
                                                 PAXDATE            pst_date);

UINT                strz_from_time              (PSTR               target,
                                                 UINT               size,
                                                 PCSTR              templ,
                                                 AXTIME             time,
                                                 BOOL               local);

PCSTR               strz_from_size              (PSTR               buff,
                                                 UINT               len,
                                                 U64                size,
                                                 PCSTR              postfix);



PCSTR               strz_timeunit_name_eng      (TIMEUNIT           unitId,
                                                 UINT               number);

PCSTR               strz_timeunit_name_rus_UTF8 (TIMEUNIT           unitId,
                                                 UINT               number);

PCSTR               strz_from_estimated_time    (PSTR               buff,
                                                 UINT               len,
                                                 AXUTIME            time,
                                                 PFNTIMEUNITNAME    cbUnitName);

//UINT32              strz_to_log_filter          (PSTR               string,
//                                                 CHAR               separator);
//
//
//PSTR                strz_desc_from_AXLOGDL      (AXLOGDL            v_level);
//
//AXLOGDL             strz_to_AXLOGDL             (PSTR               string);

PSTR                strz_utf8_to_cp1251_s       (PSTR               psz_out,
                                                 PVOID              p_in,
                                                 UINT               u_size);

PSTR                strz_utf8_to_cp1251         (PVOID              p_in,
                                                 UINT               u_size);

PSTR                strz_cp1251_to_utf8_s       (PSTR               psz_out,
                                                 PVOID              p_in,
                                                 UINT               u_size);

HSUBSTRS            strz_substrs_destroy        (HSUBSTRS           h_substr);

PSTR                strz_substrs_get            (HSUBSTRS           h_substr);

HSUBSTRS            strz_substrs_create         (PCSTR              psz_source,
                                                 UINT               u_len,
                                                 CHAR               c_sep,
                                                 UINT               u_flags);

BOOL                strz_substrs_sep            (HSUBSTRS           h_substr,
                                                 CHAR               c_sep);


PSTR                strz_substrs_get_u          (PSTR               str,
                                                 PUINT              plen,
                                                 CHAR               sep);

PSTR                strz_substrs_get_instant    (PSTR               str,
                                                 PUINT              plen,
                                                 CHAR               sep,
                                                 CHAR               quote);

void                strz_unquote                (PSTR               t,
                                                 PSTR               s,
                                                 UINT               len);


long                strz_to_seconds             (PCSTR              psz_string);

long long           strz_to_bytes               (PCSTR              psz_string);

long                strz_to_metric              (PCSTR              psz_string);

BOOL                strz_to_date                (PAXDATE            pst_time,
                                                 PCSTR              psz_string,
                                                 UINT               u_flags);

BOOL                strz_to_date_fmt            (PAXDATE            pst_time,
                                                 PSTR               psz_string,
                                                 PCSTR              psz_fmt);

UINT                strz_to_ip                  (PCSTR              psz_str);

UINT                strz_to_version             (PCSTR              psz_str);

BOOL                strz_is_num                 (PCSTR              psz_str);

BOOL                strz_is_float               (PCSTR              psz_str);

BOOL                strz_is_percent_n_cut       (PSTR               psz_str);

BOOL                strz_is_hex                 (PCSTR              psz_str);

BOOL                strz_is_ip                  (PCSTR              psz_str,
                                                 INT                octets);

BOOL                strz_is_urn                 (PCSTR              psz_str);

BOOL                strz_is_MSISDN              (PCSTR              psz_str);

BOOL                strz_is_printable           (PCSTR              psz_str,
                                                 INT                size);

INT                 strz_vformat                (PFNPUTCHAR         pfn_putchar,
                                                 PVOID              pfn_ptr,
                                                 PCSTR              format,
                                                 va_list            list);

INT                 strz_format                 (PFNPUTCHAR         pfn_putchar,
                                                 PVOID              pfn_ptr,
                                                 PCSTR              format,
                                                 ...);

INT                 strz_vsformat               (PSTR               buff,
                                                 INT                size,
                                                 PCSTR              format,
                                                 va_list            list);

INT                 strz_sformat                (PSTR               buff,
                                                 INT                size,
                                                 PCSTR              format,
                                                 ...);

INT                 strz_url_decode             (PSTR               target,
                                                 INT                size,
                                                 PCSTR              source);

PSTR                strz_esc_expand             (PSTR               psz_string);

void                strz_xml_unescape           (PSTR               str);

PCSTR               strz_xml_unescape2          (PCSTR              str);

PSTR                strz_xml_escape             (PCSTR              str);

BOOL                strz_wildcard               (PSTR               psz_name,
                                                 PSTR               psz_wildcard);

PSTR                strz_val2hex                (PSTR               psz_string,
                                                 U32                d_value,
                                                 UINT               d_size);


PSTR                strz_bin2BEhex              (PSTR               psz_string,
                                                 PVOID              p_data,
                                                 UINT               d_size);

PSTR                strz_BEhex2bin              (PVOID              p_data,
                                                 PSTR               psz_string,
                                                 UINT               d_size);

PSTR                strz_bin2hex                (PSTR               string,
                                                 PVOID              data,
                                                 UINT               size);

INT                 strz_hex2bin                (PVOID              data,
                                                 UINT               size,
                                                 PCSTR              string);

BOOL                strz_val2bits               (PSTR               targetString,
                                                 UINT               value,
                                                 UINT               bitsNum);

UINT                strz_bits2val               (PCSTR              psz_string);


// Substrings

PSTR                strz_substring_get          (PSTR               psz_tgt_string,
                                                 PSTR               psz_src_string,
                                                 UINT               d_tgt_size,
                                                 CHAR               b_separator);

PSTR                strz_substring_get_std      (PSTR               psz_tgt_string,
                                                 PSTR               psz_src_string,
                                                 UINT               u_tgt_size);


PSTR                strz_substring_get_w_quotes (PSTR               psz_tgt_string,
                                                 PSTR               psz_src_string,
                                                 UINT               d_tgt_size,
                                                 CHAR               b_separator,
                                                 CHAR               b_quote);

BOOL                strz_dump                   (PCSTR              psz_message,
                                                 PVOID              p_data,
                                                 UINT               u_size);

BOOL                strz_dump_w_txt             (PCSTR              psz_message,
                                                 PVOID              p_data,
                                                 UINT               u_size);

PSTR                strz_sdump                  (PSTR               psz_string,
                                                 UINT               d_string_width,
                                                 U32                d_separators,
                                                 PVOID              p_data,
                                                 UINT               d_size);

void                strz_sdump_to_dev           (PAXDEV             dev,
                                                 UINT               shownOffset,
                                                 UINT               stringWidth,
                                                 U32                separators,
                                                 PVOID              data,
                                                 UINT               size);

void                strz_sdump_print            (UINT               shownOffset,
                                                 UINT               stringWidth,
                                                 U32                separators,
                                                 PVOID              data,
                                                 UINT               size);

PSTR                strz_notchar                (PCSTR              psz_string,
                                                 UINT               len,
                                                 CHAR               validChar);

PVOID               strz_memnchr                (PVOID              data,
                                                 UINT               len,
                                                 U8                 valid);

PVOID               strz_mem_not_in_set         (PVOID              data,
                                                 UINT               len,
                                                 PCSTR              validSet);


PCSTR               strz_notspace               (PCSTR              psz_string,
                                                 UINT               len,
                                                 BOOL               b_stop_at_eol);

PCSTR               strz_eol                    (PCSTR              psz_string,
                                                 UINT               size);



PSTR                axslut_get_ptr_by_idx       (HAXSLUT            h_slut,
                                                 UINT               d_idx);

UINT                axslut_find_by_wc           (HAXSLUT            h_slut,
                                                 PSTR               psz_wildcard);

UINT                axslut_find                 (HAXSLUT            h_slut,
                                                 PSTR               psz_string);

HAXSLUT             axslut_conv_prepared        (PSTR               psz_table,
                                                 PVOID              p_digest);

HAXSLUT             axslut_create               (PSTR               psz_initial_table,
                                                 BOOL               b_case_sensitive);

HAXSLUT             axslut_destroy              (HAXSLUT            h_slut);



PSTR                strz_template_parse         (PUINT              pd_size,
                                                 PSTR               psz_template,
                                                 PSTR               psz_l_delim,
                                                 PSTR               psz_r_delim,
                                                 PFNAXKEYCB         pfn_proceed_key,
                                                 PVOID              p_ptr);

PCSTR               strz_from_month             (UINT               month);

PCSTR               strz_from_month_short       (UINT               month);

UINT                strz_base64_decode          (PVOID              p_data,
                                                 UINT               u_size,
                                                 PSTR               psz_source);

UINT                strz_base64_encode          (PSTR               psz_target,
                                                 UINT               u_tgt_size,
                                                 PVOID              p_data,
                                                 UINT               u_src_size);

double              strz_atof                   (PCSTR              str);



#ifdef __AXMOTION_H__
PCSTR               strz_from_MOTPHASE          (MOTPHASE           phase);
#endif

#ifdef __PORTPIN_H__
PCSTR               strz_from_GpioMode          (GpioMode           mode);
#endif

#ifdef __AXLOG_H__
PSTR                strz_from_AXLOGDL           (AXLOGDL            v_level);
PSTR                strz_sign_from_AXLOGDL      (AXLOGDL            v_level);
#endif

#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus

#endif                                      //  #ifndef __AXSTRINGH__




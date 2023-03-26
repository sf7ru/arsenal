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

#ifndef __AXCONFIGH__
#define __AXCONFIGH__

#include <arsenal.h>
#include <axstring.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// ---------------------------------------------------------------------------

#define AXCONF_TAG_SIZE         128

#define DEFSCR_DEF_EQU              '='
#define DEFSCR_DEF_REM              '#'
#define DEFSCR_DEF_CHAPOPEN         '['
#define DEFSCR_DEF_CHAPCLOSE        ']'


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

// ***************************************************************************
// ENUMERATION
//      PARSEL
// PURPOSE
//      Script parsing element
// ***************************************************************************
typedef enum
{
    PARSEL_error        = 0,                // Error occured
    PARSEL_end,                             // End of script reached
    PARSEL_name,                            // Field name
    PARSEL_value,                           // Field value
    PARSEL_struct                           // Structure
} PARSEL;

// ***************************************************************************
// STRUCTURE
//      CONFSCRIPT
// PURPOSE
//
// ***************************************************************************
typedef struct __tag__CONFSCRIPT
{
    PU8             p_script;
    UINT            d_curr_line;
    CHAR            c_equation;
    CHAR            c_remark;
    PU8             p_curr_position;
} CONFSCRIPT, * PCONFSCRIPT;


// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

#ifdef __cplusplus

// ***************************************************************************
// CLASS
//      AXDotConf

// PURPOSE
//
// ***************************************************************************
class AXDotConf
{
        PU8             scriptText;

//        UINT            d_curr_line;
        PU8             p_curr_position;

public:
        CHAR            c_remark;
        CHAR            c_equation;

                        AXDotConf               (void)
                        {
                            scriptText = NULL;
                            c_equation = '='; c_remark = '#';
                        }

                        ~AXDotConf              (void);

        BOOL            confLoad                (PSTR           fileName);

        void            confFree                (void);

        BOOL            confParse               (UINT           paramSize);

virtual BOOL            confParameter           (PSTR           paramTag,
                                                 PSTR           paramValue)
                        { return true; }
};

#endif

#ifdef __cplusplus

// ***************************************************************************
// CLASS
//      AXDotConf

// PURPOSE
//
// ***************************************************************************
class AXDotIni: public AXDotConf
{
        HAXSLUT         slut;
        UINT            chapterId;
        CHAR            chapter                 [ AXCONF_TAG_SIZE ];

        BOOL            confParameter           (PSTR           paramTag,
                                                 PSTR           paramValue);

public:
                        AXDotIni                (void)
                        {
                            slut = NULL; *chapter = 0; chapterId = AXII;
                            c_equation = '='; c_remark = ';';
                        }

        BOOL            iniParse                (HAXSLUT        slut,
                                                 UINT           paramSize);

virtual BOOL            iniChapter              (UINT           chapter)
                        { return true; }

virtual BOOL            iniChapter              (PSTR           chapter)
                        { return true; }

virtual BOOL            iniParameter            (UINT           chapter,
                                                 UINT           param,
                                                 PSTR           paramValue)
                        { return true; }

virtual BOOL            iniParameter            (PSTR           paramChapter,
                                                 PSTR           paramTag,
                                                 PSTR           paramValue)
                        { return true; }
};

#endif                                      //  #ifdef __cplusplus


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif

PCONFSCRIPT         dotdef_script_destroy       (PCONFSCRIPT        pst_script);

PCONFSCRIPT         dotdef_script_create        (PSTR               psz_script,
                                                 BOOL               b_copy);

PCONFSCRIPT         dotdef_script_open          (PSTR               psz_filename);

void                dotdef_script_reset         (PCONFSCRIPT        pst_script);


#define             dotconf_script_destroy      dotdef_script_destroy

#define             dotconf_script_create       dotdef_script_create

#define             dotconf_script_open         dotdef_script_open
#define             dotconf_script_close        dotdef_script_destroy

BOOL                dotconf_value_get           (PCONFSCRIPT        pst_ctl,
                                                 PSTR               psz_value,
                                                 UINT               d_size,
                                                 PSTR               psz_name);

PARSEL              dotconf_script_parse        (PCONFSCRIPT        pst_ctl,
                                                 PSTR               psz_tag,
                                                 UINT               u_tag_size,
                                                 PSTR               psz_param,
                                                 UINT               u_param_size);

BOOL                dotconf_value_get           (PCONFSCRIPT        pst_ctl,
                                                 PSTR               psz_value,
                                                 UINT               d_size,
                                                 PSTR               psz_name);


#define             dotini_script_destroy       dotdef_script_destroy

#define             dotini_script_create        dotdef_script_create

#define             dotini_script_load          dotdef_script_open

BOOL                dotini_value_get            (PCONFSCRIPT        pst_ctl,
                                                 PSTR               psz_value,
                                                 UINT               d_size,
                                                 PSTR               psz_chapter,
                                                 PSTR               psz_name);

PARSEL              dotini_script_parse         (PCONFSCRIPT        pst_ini,
                                                 PSTR               psz_tag,
                                                 UINT               d_tag_size,
                                                 PSTR               psz_param,
                                                 UINT               d_param_size);

#ifdef __cplusplus
}
#endif



#endif //  #ifndef __AXCONFIGH__


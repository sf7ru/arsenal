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

#ifndef __AXXMLH__
#define __AXXMLH__

#include <arsenal.h>

#include <scriptease.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// ---------------------------------------------------------------------------

#define DEFXML_NAME_MAX_LEN         64

#define XMLTAGS_BEGIN(a)    XMLTAG a[] = {


#define XMLTAG_(a,b,c)      XMLID_##b, (XMLTAGOPEN)&a::tag##c##Open, \
                            (XMLTAGCLOSE)&a::tag##c##Close,             \
                            (XMLTAGPARAM)&a::tag##c##Param,             \
                            (XMLTAGPARAMUNK)&a::tag##c##ParamUnk,       \
                            (XMLTAGOVER)&a::tag##c##Over,               \
                            (XMLTAGTEXT)&a::tag##c##Text, #c },


#define XMLTAG(a,b)         { a, XMLTAG_ b

#define XMLTAGS_END         { 0, 0, NULL, NULL, NULL, NULL } };


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// ---------------------------------------------------------------------------

class AXXML;

typedef BOOL (AXXML::*XMLTAGOPEN)(UINT);
typedef BOOL (AXXML::*XMLTAGCLOSE)(void);
typedef BOOL (AXXML::*XMLTAGPARAM)(UINT,PSTR);
typedef BOOL (AXXML::*XMLTAGPARAMUNK)(PSTR,PSTR);
typedef BOOL (AXXML::*XMLTAGOVER)(void);
typedef BOOL (AXXML::*XMLTAGTEXT)(PSTR,UINT);


// ***************************************************************************
// STRUCTURE
//      XMLTAG
// PURPOSE
//
// ***************************************************************************
class XMLTAG
{
public:

    UINT                level;
    UINT                id;
    XMLTAGOPEN          tagOpen;
    XMLTAGCLOSE         tagClose;
    XMLTAGPARAM         tagParam;
    XMLTAGPARAMUNK      tagParamUnk;
    XMLTAGOVER          tagOver;
    XMLTAGTEXT          tagText;
    CHAR                name            [ 32 ];
};

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class AXXML
{
        PSTR            scriptText;
        PSTR            onText;
        PSTR            textBegin;
        PSTR            errorDesc;
        PSTR            psz_name;
        PSTR            psz_val;
        UINT            u_val_size;
        BOOL            inTag;

        PSTR            onError;
        UINT            currLevel;
        XMLTAG *        xmlScheme;

        CHAR            fieldName               [ DEFXML_NAME_MAX_LEN ];
        CHAR            fieldValue              [ AXLHUGE ];

        PSTR            _copyString             (PSTR           psz_target,
                                                 UINT           u_size,
                                                 PSTR           psz_source);

        XMLTAG *        findTag                 (UINT           id,
                                                 XMLTAG *       parent);

        XMLTAG *        findBackTag             (UINT           id);

        XMLTAG *        findBackParent          (UINT           id);

        BOOL            tagOpen                 (PSTR           tagName);

        BOOL            tagClose                (void);

        BOOL            tagClose                (PSTR           tagName);

        BOOL            tagParam                (PSTR           paramName,
                                                 PSTR           paramValue);

        BOOL            tagOver                 (void);

protected:
        XMLTAG *        currTag;
        XMLTAG *        currParent;

public:

        BOOL            setXmlError             (PCSTR          msg,
                                                 ...);

        PLA6SLUT        slut;

                        AXXML                   (void);
virtual                 ~AXXML                  (void);

        BOOL            loadFile                (PCSTR          fileName);

        void            setText                 (PCSTR          text);


        BOOL            unloadFile              (void);


virtual BOOL            tagOpenUnknown          (PSTR           tagName)
                        { return FALSE; }

virtual BOOL            tagCloseUnknown         (PSTR           tagName)
                        { return FALSE; }

virtual BOOL            tagParamUnknown         (PSTR           paramName,
                                                 PSTR           paramValue)
                        { return FALSE; }

virtual BOOL            tagOverUnknown          (void)
                        { return FALSE; }

virtual BOOL            tagTextUnknown          (PSTR           text,
                                                 UINT           len)
                        { return FALSE; }


        BOOL            parse                   (XMLTAG *       xmlScheme);

        PSTR            getErrorPlace           (void)
                        { return onError; }

        PSTR            getErrorDesc            (void)
                        { return errorDesc; }
};

#endif //  #ifndef __AXXMLH__

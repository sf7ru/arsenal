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

//#define AXTRACE(a)     printf a

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <datastorm.h>
#include <axstring.h>

#include <AXXML.h>

// ---------------------------------------------------------------------------
// -------------------------- LOCAL DEFINITIONS ------------------------------
// ---------------------------------------------------------------------------

#define DEFXML_TAG_BEGIN            '<'
#define DEFXML_TAG_END              '>'
#define DEFXML_TAG_CLOSE            '/'
#define DEFXML_TAG_EQU              '='
#define DEFXML_TAG_QUOTE            '\"'
#define DEFXML_TAG_INVALID          "<>=\""


#define DEF_FIRST_BLANK         32
#define DEF_FIRST_NONBLANK      33

#define DEF_FIRST_BLANK         32
#define DEF_FIRST_NONBLANK      33

#define DEF_EXC_CLOSE_BEF_END   1

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      AXXML::AXXML
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//        --
// ***************************************************************************
AXXML::AXXML(void)
{
    ENTER(TRUE);

    scriptText  = NULL;

    onError     = NULL;
    errorDesc   = NULL;

//    xmlCtl  = NULL;

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      AXXML::~AXXML
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//        --
// ***************************************************************************
AXXML::~AXXML(void)
{
    ENTER(TRUE);

    if (scriptText)
    {
        FREE(scriptText);
    }

    if (errorDesc)
    {
        FREE(errorDesc);
    }

/*
    if (xmlCtl)
    {
        la6_FXML_destroy(xmlCtl);
    }
*/
    QUIT;
}
// ***************************************************************************
// FUNCTION
//      AXXML::setText
// PURPOSE
//
// PARAMETERS
//      PCSTR text --
// RESULT
//      void --
// ***************************************************************************
void AXXML::setText(PCSTR          text)
{
    scriptText          = (PSTR)text;
    inTag               = FALSE;
    textBegin           = NULL;
    onText              = scriptText;
    currTag             = NULL;
    onError             = onText;
    psz_name            = fieldName;
    psz_val             = fieldValue;
    u_val_size          = AXLHUGE;
}
// ***************************************************************************
// FUNCTION
//      AXXML::load
// PURPOSE
//
// PARAMETERS
//      PSTR   fileName --
// RESULT
//      AXXML * --
// ***************************************************************************
BOOL AXXML::loadFile(PCSTR fileName)
{
    BOOL            result        = FALSE;
    DWORD           size;

    ENTER(fileName);

    if ((scriptText = (PSTR)axfile_load((PSTR)fileName, &size, 1)) != NULL)
    {
        *(scriptText + size)   = 0;
        result            = TRUE;

        setText(scriptText);
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXML::unloadFile
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL AXXML::unloadFile(void)
{
    BOOL            b_result        = FALSE;

    ENTER(TRUE);

    if (scriptText)
    {
        FREE(scriptText);

        scriptText  = NULL;
        b_result    = TRUE;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXXML::findTag
// PURPOSE
//
// PARAMETERS
//      UINT   id --
// RESULT
//      XMLTAG*  --
// ***************************************************************************
XMLTAG* AXXML::findTag(UINT     id,
                       XMLTAG*  parent)
{
    XMLTAG*         tag     = NULL;
    XMLTAG*         on;

    ENTER(TRUE);

    for (on = parent ? parent : currParent; !tag && on->level; on++)
    {
        if ((on->level == currLevel) && (on->id == id))
        {
            tag = on;
        }
    }

    RETURN(tag);
}
// ***************************************************************************
// FUNCTION
//      AXXML::tagOpen
// PURPOSE
//
// PARAMETERS
//      PSTR   tagName --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL AXXML::tagOpen(PSTR tagName)
{
    BOOL            b_result        = FALSE;
    UINT            id;
    XMLTAG *        found;

    ENTER(TRUE);

    //AXTRACE((AXP, " OPEN: level %d, tag '%s' ", currLevel, tagName));

//    printf("  tagOpen  '%s' [currTag '%s'] [currParent '%s']\n",
//           tagName,
//           currTag ? currTag->name : "",
//           currParent ? currParent->name : "");

    if (  ((id = la6_slut_find(slut, tagName)) != AXII) &&
          ((found = findTag(id, currTag)) != NULL)      )
    {
        //AXTRACE((AXP, " - found (%p)", found));

        if (  currTag &&
              currTag->level < found->level)
        {
            currParent  = currTag;
        }

        currTag     = found;

        if ((this->*currTag->tagOpen)(id))
        {
            currLevel++;
            b_result = TRUE;
        }
        else
            setXmlError("opening tag '%s'", tagName);
    }
    else
    {
        //AXTRACE((AXP, "  tagName '%s' not in table\n", tagName));

        if (currTag)
        {
            currParent  = currTag;
        }

        currTag     = NULL;

        if (tagOpenUnknown(tagName))
        {
            currLevel++;
            b_result = TRUE;
        }
        else
            setXmlError("Unknown tag '%s'", tagName);
    }

    //AXTRACE((AXP, "  tagOpen = '%s'\n", MAC_TF(b_result)));

//    printf("      open [currTag '%s']\n",
//           currTag ? currTag->name : "");

//    printf("      open [currParent '%s']\n",
//           currParent ? currParent->name : "");

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXXML::findBackTag
// PURPOSE
//
// PARAMETERS
//      UINT   id --
// RESULT
//      XMLTAG*  --
// ***************************************************************************
XMLTAG* AXXML::findBackTag(UINT id)
{
    XMLTAG*         tag     = NULL;
    XMLTAG*         on;

    ENTER(TRUE);

    for (  on = currTag ? currTag : currParent ;
           !tag && (on >= xmlScheme) && on->level;
           on--)
    {
        if ((on->level == currLevel) && (on->id == id))
        {
            tag = on;
        }
    }

    RETURN(tag);
}
// ***************************************************************************
// FUNCTION
//      AXXML::findBackParent
// PURPOSE
//
// PARAMETERS
//      UINT   id --
// RESULT
//      XMLTAG*  --
// ***************************************************************************
XMLTAG* AXXML::findBackParent(UINT id)
{
    XMLTAG*         tag     = NULL;
    XMLTAG*         on;

    ENTER(TRUE);

    for (  on = currTag;
           on && !tag && (on >= xmlScheme);
           on--)
    {
        if ((on->level < currLevel))
        {
            tag = on;
        }
    }

    if (!tag)
    {
        tag = xmlScheme;
    }

    RETURN(tag);
}
// ***************************************************************************
// FUNCTION
//      AXXML::tagClose
// PURPOSE
//
// PARAMETERS
//      PSTR   tagName --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL AXXML::tagClose(PSTR tagName)
{
    BOOL            b_result        = TRUE;

    ENTER(TRUE);

//    printf("  tagClose '%s' [currTag '%s'] [currParent '%s']\n",
//           tagName,
//           currTag ? currTag->name : "",
//           currParent ? currParent->name : "");

    currLevel--;

    if (tagName)
    {
        currTag     = findBackTag(la6_slut_find(slut, tagName));
    }

    if (currTag)
    {
//         printf("      close currTag '%s'\n", currTag->name);

        if (!(this->*currTag->tagClose)())
        {
            b_result = setXmlError("closing tag '%s'", tagName);
        }
        else
        {
            currTag     = findBackParent(currTag->id);
            currParent  = findBackParent(currTag->id);
        }

//         printf("      close currParent '%s'\n", currParent->name);
    }
    else
    {
//         printf("      close currTag = UNKNOWN\n");

        if (!tagCloseUnknown(tagName))
        {
            b_result = setXmlError("closing unknown tag '%s'", tagName);
        }
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXXML::tagClose
// PURPOSE
//
// PARAMETERS
//      PSTR   tagName --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL AXXML::tagClose(void)
{
    BOOL            b_result        = FALSE;

    ENTER(TRUE);

    b_result = tagClose(NULL);

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXXML::tagParam
// PURPOSE
//
// PARAMETERS
//      PSTR   paramName  --
//      PSTR   paramValue --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL AXXML::tagParam(PSTR           paramName,
                     PSTR           paramValue)
{
    BOOL            b_result        = FALSE;
    UINT            id;

    ENTER(TRUE);

    if (currTag)
    {
        if ((id = la6_slut_find(slut, paramName)) != AXII)
        {
            b_result = (this->*currTag->tagParam)(id, paramValue);
        }
        else
        {
            b_result = (this->*currTag->tagParamUnk)(paramName, paramValue);
        }
    }
    else
    {
        if ((b_result = tagParamUnknown(paramName, paramValue)) == FALSE)
        {
            setXmlError("evaluating unknown parameter '%s=\"%s\"'", paramName, paramValue);
        }
    }

    //AXTRACE((AXP, "  tagParam = '%s'\n", MAC_TF(b_result)));

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXXML::tagOver
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL AXXML::tagOver(void)
{
    BOOL            b_result        = FALSE;

    ENTER(TRUE);

    if (currTag)
    {
        b_result = (this->*currTag->tagOver)();
    }
    else
    {
        if ((b_result = tagOverUnknown()) == FALSE)
        {
            setXmlError("handing end of unknown tag");
        }
    }

//    AXTRACE("  tagOver = '%s'\n", MAC_TF(b_result));

    RETURN(b_result);
}
// ***************************************************************************
// STATIC FUNCTION
//      _copyString
// PURPOSE
//
// PARAMETERS
//      PSTR psz_target --
//      UINT u_size     --
//      PSTR psz_source --
// RESULT
//      PSTR    --
// ***************************************************************************
PSTR AXXML::_copyString(PSTR       psz_target,
                         UINT       u_size,
                         PSTR       psz_source)
{
    PSTR        psz_on_src          = psz_source;
    PSTR        psz_on_tgt          = psz_target;
    PSTR        psz_stop            = psz_target + u_size;

    ENTER(TRUE);

    if (*psz_on_src == DEFXML_TAG_QUOTE)
    {
// ------------------------ Copying quoted string ----------------------------

        psz_on_src++;

        while ( *psz_on_src                         &&
                (psz_stop    >  psz_on_tgt)         &&
                (*psz_on_src != DEFXML_TAG_QUOTE)   )
//                (*psz_on_src != DEFXML_TAG_CLOSE)   &&
//                (*psz_on_src != DEFXML_TAG_END)     )
        {
            *(psz_on_tgt++) = *(psz_on_src++);
        }

        if (*psz_on_src == DEFXML_TAG_QUOTE)
            psz_on_src++;
    }
    else
    {
// ------------------------- Copying raw string ------------------------------

        while ( *psz_on_src                         &&
                (psz_stop    >  psz_on_tgt)         &&
                (*psz_on_src >  ' ')                &&
                (*psz_on_src != DEFXML_TAG_EQU)     &&
                (*psz_on_src != DEFXML_TAG_CLOSE)   &&
                (*psz_on_src != DEFXML_TAG_END)     )
        {
            *(psz_on_tgt++) = *(psz_on_src++);
        }
    }

    *psz_on_tgt = 0;

    RETURN(psz_on_src);
}
// ***************************************************************************
// FUNCTION
//      AXXML::parse
// PURPOSE
//
// PARAMETERS
//      PLA6SLUT   slut      --
//      XMLTAG *   xmlScheme --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL AXXML::parse(XMLTAG *      xmlScheme)
{
    BOOL            result              = TRUE;
    BOOL            b_close;
    UINT            len;

    ENTER(TRUE);

    this->currLevel     = 1;
    this->currTag       = NULL;
    this->currParent    = xmlScheme;
    this->xmlScheme     = xmlScheme;

    while (result && *onText)
    {
        if (inTag)
        {
            onText  = stz_notspace(onText, -1, FALSE);

            switch (*onText)
            {
                case DEFXML_TAG_BEGIN:
                    result = setXmlError("opening a new tag while old one is not closed");
                    break;

                case DEFXML_TAG_CLOSE:
                    if (!tagClose())
                    {
                        result = setXmlError("closing tag");
                    }
                    onText++;
                    break;

                case DEFXML_TAG_END:    // End of tag
                    if (!tagOver())
                    {
                        result = setXmlError("handing end of a tag");
                    }

                    inTag       = FALSE;
                    onText++;
                    break;

                default:                // Parameter

// ----------------------- Copying parameter name ----------------------------

                    onText = _copyString(psz_name,
                                        DEFXML_NAME_MAX_LEN, onText);
                    onText = stz_notspace(onText, -1, FALSE);

                    if (*onText == DEFXML_TAG_EQU)
                    {
// ----------------------- Copying parameter value ---------------------------

                        onText = stz_notspace(++onText, -1, FALSE);
                        onText = _copyString(psz_val, u_val_size, onText);
                    }
                    else
                        *psz_val = 0;

                    if (!tagParam(fieldName, fieldValue))
                    {
                        result = setXmlError("evaluating '%s=\"%s\"'",
                                          fieldName, fieldValue);
                    }
                    break;
            }
        }
        else
        {
            if (*onText == DEFXML_TAG_BEGIN)
            {
                onError = onText;

                if (textBegin)
                {
                    if ((len = onText - textBegin) > 0)
                    {
                        if (currTag)
                        {
                            if (!(this->*currTag->tagText)(textBegin, len))
                            {
                                result = setXmlError("handing text in tag '%s'", currTag->name);
                            }
                        }
                        else
                        {
                            if (!tagTextUnknown(textBegin, len))
                            {
                                result = setXmlError("handing text in unknown tag");
                            }
                        }
                    }

                    textBegin = NULL;
                }

                if (result)
                {
// --------------------------- Entering in tag -------------------------------

                    if (*(++onText) == DEFXML_TAG_CLOSE)
                    {
                        b_close = TRUE;
                        onText++;
                    }
                    else
                        b_close = FALSE;

                    inTag       = TRUE;
                    onText      = stz_notspace(onText, -1, FALSE);
                    onText      = _copyString(psz_name,
                                                    DEFXML_NAME_MAX_LEN, onText);

                    //AXTRACE("  TAG '%c' '%s'\n", b_close ? '/' : ' ', psz_name);

                    if (!(b_close ? tagClose(fieldName) : tagOpen(fieldName)))
                    {
                        result = setXmlError("%s tag '%s'",
                                  b_close ? "'closing'" : "'opening'", fieldName);
                    }
                }
            }
            else
            {
                if (!textBegin)
                    textBegin = onText;

                onText++;
            }
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXML::setXmlError
// PURPOSE
//
// PARAMETERS
//      PCSTR   msg --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL AXXML::setXmlError(PCSTR          msg,
                     ...)
{
    va_list         st_va_list;

    ENTER(TRUE);

    if (!errorDesc)
    {
        if ((errorDesc = (PSTR)MALLOC(AXLLONG)) != NULL)
        {
            va_start(st_va_list, msg);

            strz_vsformat(errorDesc, AXLLONG, msg, st_va_list);
        }
    }

    RETURN(FALSE);
}


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

#include <stdio.h>
#include <string.h>
#include <AXXmlDom.h>
#include <AXXML.h>

#define __LASLUTTABLES__                  // for SLUT body
#include <arsenal/libax/src/web/xmldom/xml.agi>

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class AXXmlParser: public AXXML
{
public:
        UINT            currNesting;

        AXXmlNode *     currentNode;
// ---------------------------------------------------------------------------

        PSTR            curNs;
        PSTR            curName;


//                        AXXmlParser                  ()
//        { currentNode = nil; }


        void            toCurName               (PSTR           tagName);

        BOOL            tagDocOpen              (UINT           id);

        BOOL            tagDocClose             (void)
                        { currentNode = nil; return TRUE; }

        BOOL            tagDocParam             (UINT           paramId,
                                                 PSTR           paramValue)
                        { return TRUE; }

        BOOL            tagDocParamUnk          (PSTR           param,
                                                 PSTR           paramValue)
        { return (*param != '?') ? tagParamUnknown(param, paramValue) : true; }

        BOOL            tagDocOver              (void)
                        { return TRUE; }

        BOOL            tagDocText              (PSTR           text,
                                                 UINT           len)
                        { return TRUE; }


// ---------------------------------------------------------------------------

        BOOL            tagOpenUnknown          (PSTR           tagName);

        BOOL            tagCloseUnknown         (PSTR           tagName);

        BOOL            tagParamUnknown         (PSTR           paramName,
                                                 PSTR           paramValue);

        BOOL            tagOverUnknown          (void);

        BOOL            tagTextUnknown          (PSTR           text,
                                                 UINT           len);

        BOOL            parse                   (AXXmlNode *    root,
                                                 PCSTR          source);
};

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      Parser::toCurName
// PURPOSE
//
// PARAMETERS
//      PSTR tagName --
// RESULT
//      void --
// ***************************************************************************
void AXXmlParser::toCurName(PSTR tagName)
{
    PSTR            on;

    if (tagName)
    {
        if ((on = strchr(tagName, ':')) != nil)
        {
            *on     = 0;
            curNs   = tagName;
            curName = (on + 1);
        }
        else
        {
            curNs   = nil;
            curName = tagName;
        }
    }
    else
    {
        curNs       = nil;
        curName     = nil;
    }
}
// ***************************************************************************
// FUNCTION
//      Parser::tagOpenUnknown
// PURPOSE
//
// PARAMETERS
//      PSTR tagName --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXXmlParser::tagOpenUnknown(PSTR           tagName)
{
    BOOL            result          = FALSE;

    ENTER(TRUE);

    if (currentNode)
    {
        toCurName(tagName);

        if ((currentNode = currentNode->addNode(curNs, curName)) != nil)
        {
            result = true;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      Parser::tagCloseUnknown
// PURPOSE
//
// PARAMETERS
//      PSTR tagName --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXXmlParser::tagCloseUnknown(PSTR           tagName)
{
    BOOL            result          = FALSE;

    ENTER(TRUE);

    if (currentNode)
    {
        toCurName(tagName);

        if (!tagName || currentNode->isNameEqual(curNs, curName))
        {
            if ((currentNode = currentNode->getParent()) != nil)
            {
                result = true;
            }
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      Parser::tagParamUnknown
// PURPOSE
//
// PARAMETERS
//      PSTR paramName  --
//      PSTR paramValue --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXXmlParser::tagParamUnknown(PSTR           paramName,
                                  PSTR           paramValue)
{
    BOOL            result          = FALSE;

    ENTER(TRUE);

    if (currentNode)
    {
        toCurName(paramName);

        if (currentNode->addProperty(curNs, curName, paramValue) != nil)
        {
            result = true;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      Parser::tagOverUnknown
// PURPOSE
//
// PARAMETERS
//        void -- None
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXXmlParser::tagOverUnknown(void)
{
    //printf("Parser::tagOverUnknown stage\n");
    return true;
}
// ***************************************************************************
// FUNCTION
//      Parser::tagTextUnknown
// PURPOSE
//
// PARAMETERS
//      PSTR text --
//      UINT len  --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXXmlParser::tagTextUnknown(PSTR           text,
                                 UINT           len)
{
    BOOL            result          = FALSE;
    PSTR            copy;

    ENTER(TRUE);

    if (currentNode)
    {
        if ((copy = strz_mem(text, len)) != nil)
        {
            currentNode->setValueDirect(copy);
            result = true;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//
// PURPOSE
//
// PARAMETERS
//      UINT id --
// RESULT
//      BOOL Parser::tagDocOpen             --
// ***************************************************************************
BOOL AXXmlParser::tagDocOpen(UINT           id)
{
    BOOL            result          = FALSE;

    ENTER(TRUE);

    if (currentNode)
    {
        if (currentNode->setName(nil, "xml"))
        {
            currentNode->doc    = true;
            result              = true;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      Parser::parse
// PURPOSE
//
// PARAMETERS
//      PCSTR source --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXXmlParser::parse(AXXmlNode *    root,
                        PCSTR          source)
{
    BOOL            b_result        = TRUE;
    PSTR            onError;
    CHAR            sz_error        [ 128 + 1 ];

    XMLTAGS_BEGIN(xmlScheme)
    XMLTAG(1, (AXXmlParser, _QRY_xml, Doc))
    XMLTAGS_END

    ENTER(TRUE);

    currentNode = root;

    if ((slut = la6_slut_conv_prepared(table_lut, digtable_lut)) != nil)
    {
        setText((PSTR)source);

        if (!AXXML::parse(xmlScheme))
        {
            if ((onError = getErrorPlace()) != nil)
            {
                strz_cpy(sz_error, onError, 128);

                printf("config parsing failed on %s:\n---\n%s\n---\n", getErrorDesc(), sz_error);
            }
            else
                printf("error parsing config\n");

            b_result = FALSE;
        }

        // or it will be refreed
        setText((PSTR)nil);

        slut = la6_slut_destroy(slut);
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlDoc::parse
// PURPOSE
//
// PARAMETERS
//      PCSTR source --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXXmlDoc::parse(PCSTR          source,
                     HAXLOG         logSession)
{
    BOOL            result          = FALSE;
    AXXmlParser          p;
    PSTR            onError;
    CHAR            sz_error        [ 128 + 1 ];

    ENTER(TRUE);

    if (logSession)
        axlog_msg_v(logSession, AXLOGDL_debug,
                  "------------------------------ ORIGINAL ---------------------------\nlen = %u\n%s",
                  (UINT)strlen(source), source);

    if (!p.parse(this, source))
    {
        if ((onError = p.getErrorPlace()) != nil)
        {
            strz_cpy(sz_error, onError, 128);

            printf("parsing failed on %s:\n---\n%s\n---\n", p.getErrorDesc(), sz_error);
        }
        else
            printf("error parsing config\n");

        result = FALSE;
    }
    else
    {
        PCSTR   str = toString();

        if (logSession)
            axlog_msg_v(logSession, AXLOGDL_debug,
                        "------------------------------ PARSED -----------------------------\nlen = %u\n%s",
                        (UINT)strlen(str), str);

        result = true;
    }

    RETURN(result);
}

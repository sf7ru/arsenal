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
#include <AXXmlDom.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      AXXmlDoc::AXXmlDoc
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
AXXmlDoc::AXXmlDoc()
{
}
// ***************************************************************************
// TYPE
//      Destructor
// FUNCTION
//      AXXmlDoc::~AXXmlDoc
// PURPOSE
//      Class destruction
// PARAMETERS
//          --
// ***************************************************************************
AXXmlDoc::~AXXmlDoc()
{

}
// ***************************************************************************
// FUNCTION
//      AXXmlDoc::init
// PURPOSE
//
// PARAMETERS
//      PCSTR encoding --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXXmlDoc::init(PCSTR          encoding)
{
    BOOL            result          = FALSE;

    ENTER(TRUE);

    if (AXXmlNode::init(0, nil, "xml"))
    {
        if (  (addProperty("version", "1.0")                    &&
              (!encoding || addProperty("encoding", encoding))) )
        {
            doc     = true;
            result  = true;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlDoc::create
// PURPOSE
//
// PARAMETERS
//      PCSTR encoding --
// RESULT
//      AXXmlDoc * --
// ***************************************************************************
AXXmlDoc * AXXmlDoc::create(PCSTR          encoding)
{
    AXXmlDoc *          result          = nil;

    ENTER(TRUE);

    if ((result = new AXXmlDoc) != nil)
    {
        if (!result->init(encoding))
        {
            SAFEDELETE(result);
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlDoc::parseDoc
// PURPOSE
//
// PARAMETERS
//      PCSTR source --
// RESULT
//      AXXmlDoc * --
// ***************************************************************************
AXXmlDoc * AXXmlDoc::create(PCSTR          source,
                            PCSTR          encoding,
                            HAXLOG         logSession)
{
    AXXmlDoc *       result          = FALSE;

    ENTER(TRUE);

    if ((result = new AXXmlDoc) != nil)
    {
        if (!result->parse(source, logSession))
        {
            SAFEDELETE(result);
        }
    }

    RETURN(result);
}

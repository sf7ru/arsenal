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

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _free_property
// PURPOSE
//
// PARAMETERS
//      AXXmlProperty * pro --
// RESULT
//      AXXmlProperty * --
// ***************************************************************************
static AXXmlProperty * _free_property(AXXmlProperty * pro)
{
    SAFEDELETE(pro);
    return nil;
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _free_node
// PURPOSE
//
// PARAMETERS
//      AXXmlNode * no --
// RESULT
//      AXXmlNode * --
// ***************************************************************************
static AXXmlNode * _free_node(AXXmlNode * no)
{
    SAFEDELETE(no);
    return nil;
}
// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      AXXmlNode::AXXmlNode
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
AXXmlNode::AXXmlNode()
{
    properties  = nil;
    nodes       = nil;
    nesting     = 0;
    doc         = false;
    parent      = nil;
}
// ***************************************************************************
// TYPE
//      Destructor
// FUNCTION
//      AXXmlNode::~AXXmlNode
// PURPOSE
//      Class destruction
// PARAMETERS
//          --
// ***************************************************************************
AXXmlNode::~AXXmlNode()
{
    SAFEDESTROY(recset, properties);
    SAFEDESTROY(recset, nodes);
}
// ***************************************************************************
// FUNCTION
//      AXXmlNode::init
// PURPOSE
//
// PARAMETERS
//      UINT  nesting   --
//      PCSTR nameSpace --
//      PCSTR name      --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXXmlNode::init(UINT           nesting,
                     PCSTR          nameSpace,
                     PCSTR          name)
{
    BOOL            result          = FALSE;

    ENTER(TRUE);

    if (AXXmlProperty::init(nameSpace, name))
    {
        if ((nodes = recset_create(5, (PFNRECFREE)_free_node)) != nil)
        {
            if ((properties = recset_create(5, (PFNRECFREE)_free_property)) != nil)
            {
                this->nesting   = nesting;
                result          = true;
            }
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlNode::create
// PURPOSE
//
// PARAMETERS
//      UINT  nesting   --
//      PCSTR nameSpace --
//      PCSTR name      --
// RESULT
//      AXXmlNode * --
// ***************************************************************************
AXXmlNode * AXXmlNode::create(UINT           nesting,
                              PCSTR          nameSpace,
                              PCSTR          name)
{
    AXXmlNode *         result          = nil;

    ENTER(TRUE);

    if ((result = new AXXmlNode) != nil)
    {
        if (!result->init(nesting, nameSpace, name))
        {
            SAFEDELETE(result);
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlNode::addNode
// PURPOSE
//
// PARAMETERS
//      PCSTR nameSpace --
//      PCSTR name      --
// RESULT
//      AXXmlNode * --
// ***************************************************************************
AXXmlNode * AXXmlNode::addNode(PCSTR          nameSpace,
                               PCSTR          name)
{
    AXXmlNode *     result          = nil;

    ENTER(TRUE);

    if ((result = create(nesting + (doc ? 0 : 1), nameSpace, name)) != nil)
    {
        result->parent = this;

        if (recset_add(nodes, result, 0) == AXII)
        {
            SAFEDELETE(result);
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlNode::addProperty
// PURPOSE
//
// PARAMETERS
//      PCSTR nameSpace --
//      PCSTR name      --
//      PCSTR value     --
// RESULT
//      AXXmlProperty * --
// ***************************************************************************
AXXmlProperty * AXXmlNode::addProperty(PCSTR          nameSpace,
                                       PCSTR          name,
                                       PCSTR          value)
{
    AXXmlProperty *     result          = nil;

    ENTER(TRUE);

    if ((result = AXXmlProperty::create(nameSpace, name)) != nil)
    {
        if (result->setValue(value))
        {
            if (recset_add(properties, result, 0) == AXII)
            {
                SAFEDELETE(result);
            }
        }
        else
            SAFEDELETE(result);
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlNode::addToStringProperties
// PURPOSE
//
// PARAMETERS
//      HAXSTR    str --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXXmlNode::addToStringProperties(HAXSTR         str)
{
    BOOL            result          = true;
    AXXmlProperty * pro;

    ENTER(TRUE);

    for (  UINT i = 0;
           result && (i < recset_count(properties));
           i++)
    {
        if ((pro = (AXXmlProperty*)recset_get(properties, i)) != nil)
        {
            result = //axstr_add_tabs(str, nesting + 1)   &&
                     axstr_add_c(str, " ")             &&
                     pro->addToString(str)              ;
                     //axstr_add_c(str, "\n")             ;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _add_value
// PURPOSE
//
// PARAMETERS
//      HAXSTR    str   --
//      PCSTR     value --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
static BOOL _add_value(HAXSTR           str,
                       PCSTR            value)
{
    BOOL            result          = FALSE;
    PCSTR           out;

    ENTER(TRUE);

    if ((out = strz_xml_escape(value)) != nil)
    {
        result = axstr_add_c(str, out);

        FREE(out);
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlNode::addToString
// PURPOSE
//
// PARAMETERS
//      HAXSTR    str --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXXmlNode::addToString(HAXSTR         str)
{
    BOOL            result          = FALSE;
    UINT            nn;
    AXXmlNode *     n;

    ENTER(TRUE);

    // <tag ...
    if (     axstr_add_tabs(str, nesting)       &&
             axstr_add_c(str, doc ? "<?" : "<") &&
             addToStringName(str)               &&
             addToStringProperties(str)         )
    {
        nn = recset_count(nodes);

        if (value || nn)
        {
            // tag> ...
            if (axstr_add_c(str, doc ? "?>" : ">"))
            {
                if (!value || _add_value(str, value))
                {
                    if (nn)
                    {
                        result = axstr_add_c(str, "\n");

                        for (  UINT i = 0;
                               result && (i < nn);
                               i++)
                        {
                            if ((n = (AXXmlNode *)recset_get(nodes, i)) != nil)
                            {
                                result = n->addToString(str);
                            }
                        }

                        if (result)
                            result = axstr_add_c(str, "\n")         &&
                                     axstr_add_tabs(str, nesting)   ;

                    }
                    else
                        result = true;

                    if (result)
                    {
                        if (!doc)
                        {
                            result = axstr_add_c(str, "</")         &&
                                     addToStringName(str)           &&
                                     axstr_add_c(str, ">\n")          ;
                        }
                        else
                            result = true;
                    }
                }
            }
        }
        else
        {
            // tag/> ...
            result = axstr_add_c(str, doc ? "?>\n" : "/>\n");
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlNode::findNode
// PURPOSE
//
// PARAMETERS
//      PCSTR nameSpace --
//      PCSTR name      --
//      UINT  index     --
// RESULT
//      AXXmlNode * --
// ***************************************************************************
AXXmlNode * AXXmlNode::findNode(PCSTR nameSpace,
                                PCSTR name,
                                UINT  index)
{
    AXXmlNode *     result          = nil;
    AXXmlNode *     tmp;
    UINT            curr            = index;

    ENTER(TRUE);

    for (  UINT i = 0;
           !result && (i < recset_count(nodes));
           i++)
    {
        if ((tmp = (AXXmlNode*)recset_get(nodes, i)) != nil)
        {
            if (tmp->isNameEqual(nameSpace, name))
            {
                if (!(curr--))
                    result = tmp;
            }
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlNode::findProperty
// PURPOSE
//
// PARAMETERS
//      PCSTR nameSpace --
//      PCSTR name      --
// RESULT
//      AXXmlProperty * --
// ***************************************************************************
AXXmlProperty * AXXmlNode::findProperty(PCSTR          nameSpace,
                                        PCSTR          name,
                                        UINT           index)
{
    AXXmlProperty * result          = nil;
    AXXmlProperty * tmp;
    UINT            curr            = index;

    ENTER(TRUE);

    for (  UINT i = 0;
           !result && (i < recset_count(properties));
           i++)
    {
        if ((tmp = (AXXmlProperty*)recset_get(properties, i)) != nil)
        {
            if (tmp->isNameEqual(nameSpace, name))
            {
                if (!(curr--))
                    result = tmp;
            }
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlNode::findNodeByPtr
// PURPOSE
//
// PARAMETERS
//      AXXmlNode * node --
// RESULT
//      UINT --
// ***************************************************************************
UINT AXXmlNode::findNodeByPtr(AXXmlNode *    node)
{
    UINT            result          = AXII;
    AXXmlNode *     tmp;

    ENTER(TRUE);

    for (  UINT i = 0; i < recset_count(nodes); i++)
    {
        if ((tmp = (AXXmlNode*)recset_get(nodes, i)) != nil)
        {
            if (tmp == node)
            {
                result = i;
                break;
            }
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlNode::delNode
// PURPOSE
//
// PARAMETERS
//      AXXmlNode * node --
// RESULT
//      AXXmlNode * --
// ***************************************************************************
AXXmlNode * AXXmlNode::delNode(AXXmlNode * node)
{
    UINT            idx;

    ENTER(TRUE);

    if ((idx = findNodeByPtr(node)) != AXII)
    {
        recset_del(nodes, idx);
    }
    else
        SAFEDELETE(node);

    RETURN(nil);
}
// ***************************************************************************
// FUNCTION
//      AXXmlNode::addNodeAndValueLong
// PURPOSE
//
// PARAMETERS
//      AXXmlNode * node  --
//      PCSTR       name  --
//      UINT64      value --
// RESULT
//      AXXmlNode * --
// ***************************************************************************
AXXmlNode * AXXmlNode::addNodeAndValue(PCSTR          name,
                                       PCSTR          value)
{
    AXXmlNode *         result      = nil;

    ENTER(TRUE);

    if ((result = addNode(name)) != nil)
    {
        if (!result->setValue(value))
        {
            result = delNode(result);
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlNode::addNodeAndValueLong
// PURPOSE
//
// PARAMETERS
//      AXXmlNode * node  --
//      PCSTR       name  --
//      UINT64      value --
// RESULT
//      AXXmlNode * --
// ***************************************************************************
AXXmlNode * AXXmlNode::addNodeAndValueLong(PCSTR          name,
                                           INT64          value)
{
    AXXmlNode *         result      = nil;

    ENTER(TRUE);

    if ((result = addNode(name)) != nil)
    {
        if (!result->setValueLong(value))
        {
            result = delNode(result);
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlNode::addNodeAndValueFloat
// PURPOSE
//
// PARAMETERS
//      PCSTR  name  --
//      double value --
// RESULT
//      AXXmlNode * --
// ***************************************************************************
AXXmlNode * AXXmlNode::addNodeAndValueFloat(PCSTR          name,
                                            double         value)
{
    AXXmlNode *         result      = nil;

    ENTER(TRUE);

    if ((result = addNode(name)) != nil)
    {
        if (!result->setValueFloat(value))
        {
            result = delNode(result);
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlNode::addNodeAndValueDate
// PURPOSE
//
// PARAMETERS
//      PCSTR  name  --
//      double value --
// RESULT
//      AXXmlNode * --
// ***************************************************************************
AXXmlNode * AXXmlNode::addNodeAndValueDate(PCSTR          name,
                                           AXTIME         value)
{
    AXXmlNode *         result      = nil;

    ENTER(TRUE);

    if ((result = addNode(name)) != nil)
    {
        if (!result->setValueDate(value))
        {
            result = delNode(result);
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
//      AXXmlNode * node  --
//      PCSTR       name  --
//      INT         value --
// RESULT
//      AXXmlNode * AXXmlNode::addNodeAndValueInt      --
// ***************************************************************************
AXXmlNode * AXXmlNode::addNodeAndValueInt(PCSTR          name,
                                          INT            value)
{
    AXXmlNode *         result      = nil;

    ENTER(TRUE);

    if ((result = addNode(name)) != nil)
    {
        if (!result->setValueInt(value))
        {
            result = delNode(result);
        }
    }

    RETURN(result);
}


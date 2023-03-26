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
#include <AXDom.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      static
// FUNCTION
//      _AXDomObjectFree
// PURPOSE
//
// PARAMETERS
//      AXDomProperty * obj --
// RESULT
//      AXDomProperty * --
// ***************************************************************************
AXDomProperty * _AXDomObjectFree(AXDomProperty * obj)
{
    ENTER(true);

    switch (obj->getType())
    {
        case AXDOMT_property:
            delete obj;
            break;

        case AXDOMT_array:
        case AXDOMT_object:
            delete (AXDomObject*)obj;
            break;

        default:
            AXTRACE("JSON OBJECT TYPE ERROR ON DELETE = %u!\n", obj->type);
            break;
    }

    RETURN(nil);
}
// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      AXDomObject::AXDomObject
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
AXDomObject::AXDomObject()
{
    type        = AXDOMT_object;
    collection  = nil;
}
// ***************************************************************************
// TYPE
//      Destructor
// FUNCTION
//      AXDomObject::~AXDomObject
// PURPOSE
//      Class destruction
// PARAMETERS
//          --
// ***************************************************************************
AXDomObject::~AXDomObject()
{
    SAFEDESTROY(recset, collection);
}
// ***************************************************************************
// FUNCTION
//      AXDomObject::initObject
// PURPOSE
//
// PARAMETERS
//      UINT initSize --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXDomObject::initObject(UINT           initSize)
{
    BOOL            result          = false;

    ENTER(true);

    if ((collection = recset_create(initSize, (PFNRECFREE)_AXDomObjectFree)) != nil)
    {
        result = true;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXDomObject::create
// PURPOSE
//
// PARAMETERS
//      PCSTR name     --
//      UINT  initSize --
// RESULT
//      AXDomObject * --
// ***************************************************************************
AXDomObject * AXDomObject::create(PCSTR          name,
                                UINT           initSize)
{
    AXDomObject *        result          = nil;

    ENTER(true);

    if ((result = new AXDomObject) != nil)
    {
        result->name = strz_dup(name);

        if (!result->initObject(initSize))
        {
            SAFEDELETE(result);
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXDomObject::addToCollection
// PURPOSE
//
// PARAMETERS
//      AXDomProperty * p --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXDomObject::addToCollection(AXDomProperty * p)
{
    BOOL            result          = false;

    ENTER(true);

    switch (recset_add(collection, p, 0))
    {
        case AXII:
            break;

        case 0:
            p->addFlag(AXDOMF_FIRST);
            //no break;

        default:
            result      = true;
            break;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXDomObject::addObject
// PURPOSE
//
// PARAMETERS
//      PCSTR name --
// RESULT
//      AXDomObject * --
// ***************************************************************************
AXDomObject * AXDomObject::addObject(PCSTR name,
                                     UINT  initSize)
{
    AXDomObject *        result          = nil;

    ENTER(true);

    if ((result = AXDomObject::create(name, initSize)) != nil)
    {
        result->setDepth(depth + 1);
        //result->setDepth(depth + ((flags & AXDOMF_SHEET) ? 0 : 1));

        if (!addToCollection(result))
        {
            SAFEDELETE(result);
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXDomObject::addProperty
// PURPOSE
//
// PARAMETERS
//      PCSTR name  --
//      PCSTR value --
// RESULT
//      AXDomProperty * --
// ***************************************************************************
AXDomProperty * AXDomObject::addProperty(PCSTR          name,
                                         PCSTR          value)
{
    AXDomProperty *        result          = nil;

    ENTER(true);

    if ((result = AXDomProperty::create(name, value)) != nil)
    {
        result->setDepth(depth + 1);

        if (!addToCollection(result))
        {
            SAFEDELETE(result);
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXDomObject::addPropertyUint
// PURPOSE
//
// PARAMETERS
//      PCSTR name  --
//      UINT  value --
// RESULT
//      AXDomProperty * --
// ***************************************************************************
AXDomProperty * AXDomObject::addPropertyUint(PCSTR          name,
                                           UINT           value)
{
    CHAR            tmp             [ AXLMEDIUM ];

    snprintf(tmp, AXLMEDIUM, "%u", value);

    return addProperty(name, tmp);
}
AXDomProperty * AXDomObject::addProperty(PCSTR          name,
                                         ULONG          value)
{
    CHAR            tmp             [ AXLMEDIUM ];

    snprintf(tmp, AXLMEDIUM, "%llu", (unsigned long long)value);

    return addProperty(name, tmp);
}
AXDomProperty *  AXDomObject::addProperty(PCSTR          name,
                                          double         value)
{
    CHAR            tmp             [ AXLMEDIUM ];

    snprintf(tmp, AXLMEDIUM, "%f", value);

    return addProperty(name, tmp);
}
// ***************************************************************************
// FUNCTION
//      AXDomObject::getElementByIndex
// PURPOSE
//
// PARAMETERS
//      UINT index --
// RESULT
//      AXDomProperty * --
// ***************************************************************************
AXDomProperty * AXDomObject::getElementByIndex(UINT           index)
{
    AXDomProperty*      result          = nil;

    ENTER(true);

    if (collection)
    {
        result =  (AXDomProperty*)recset_get(collection, index);
    }

    RETURN(result);
}

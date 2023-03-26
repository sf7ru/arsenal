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

#include <AXDom.h>

// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      AXDomProperty::AXDomProperty
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
AXDomProperty::AXDomProperty()
{
    name        = nil;
    value       = nil;
    flags       = 0;
    depth       = 0;
    type        = AXDOMT_property;
}
// ***************************************************************************
// TYPE
//      Destructor
// FUNCTION
//      AXDomProperty::~AXDomProperty
// PURPOSE
//      Class destruction
// PARAMETERS
//          --
// ***************************************************************************
AXDomProperty::~AXDomProperty()
{
    SAFEFREE(name);
    SAFEFREE(value);
}
// ***************************************************************************
// FUNCTION
//      AXDomProperty::create
// PURPOSE
//
// PARAMETERS
//      PCSTR name --
// RESULT
//      AXDomProperty * --
// ***************************************************************************
AXDomProperty * AXDomProperty::create(PCSTR          name,
                                    PCSTR          value)
{
    AXDomProperty *        result          = nil;

    ENTER(true);

    if ((result = new AXDomProperty) != nil)
    {
        if ((result->name = strz_dup(name)) != nil)
        {
            if (!result->setValue(value, true))
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
//      AXDomProperty::setValue
// PURPOSE
//
// PARAMETERS
//      PCSTR value --
//      BOOL  copy  --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXDomProperty::setValue(PCSTR value,
                            BOOL  copy)
{
    BOOL            result          = false;
    PSTR            dup             = nil;

    ENTER(true);

    SAFEFREE(this->value);

    if (copy)
    {
        if (!value || !*value || ((dup = strz_dup(value)) != nil))
        {
            SAFEFREE(this->value);

            this->value = dup;
            result      = true;
        }
    }
    else
    {
        SAFEFREE(this->value);

        this->value = value;
        result      = true;
    }

    RETURN(result);
}

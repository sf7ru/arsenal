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
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <axstring.h>
#include <AXXmlDom.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      AXXmlProperty::AXXmlProperty
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
AXXmlProperty::AXXmlProperty()
{
    name            = nil;
    nameSpace       = nil;
    value           = nil;
    toStringResult  = nil;
}
// ***************************************************************************
// TYPE
//      Destructor
// FUNCTION
//      AXXmlProperty::~AXXmlProperty
// PURPOSE
//      Class destruction
// PARAMETERS
//          --
// ***************************************************************************
AXXmlProperty::~AXXmlProperty()
{
    SAFEFREE(nameSpace);
    SAFEFREE(name);
    SAFEFREE(value);
    SAFEFREE(toStringResult);
}
// ***************************************************************************
// FUNCTION
//      AXXmlProperty::init
// PURPOSE
//
// PARAMETERS
//      PCSTR nameSpace --
//      PCSTR name      --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXXmlProperty::init(PCSTR          nameSpace,
                         PCSTR          name)
{
    BOOL            result          = false;

    ENTER(TRUE);

    if ((this->name = strz_dup(name)) != nil)
    {
        if (!nameSpace || ((this->nameSpace = strz_dup(nameSpace)) != nil))
        {
            result = true;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlProperty::create
// PURPOSE
//
// PARAMETERS
//      PCSTR nameSpace --
// RESULT
//      AXXmlProperty * --
// ***************************************************************************
AXXmlProperty * AXXmlProperty::create(PCSTR          nameSpace,
                                      PCSTR          name)
{
    AXXmlProperty *     result          = nil;

    ENTER(TRUE);

    if ((result = new AXXmlProperty) != nil)
    {
        if (!result->init(nameSpace, name))
        {
            SAFEDELETE(result);
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlProperty::setValue
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXXmlProperty::setValue(PCSTR value)
{
    BOOL            result          = false;
    PSTR            unescaped;

    ENTER(TRUE);

    SAFEFREE(this->value);

    if ((unescaped = (PSTR)strz_xml_unescape2(value)) != nil)
    {
        this->value = unescaped;
        result      = true;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlProperty::setValueDirect
// PURPOSE
//
// PARAMETERS
//      PCSTR value --
// RESULT
//      void --
// ***************************************************************************
void AXXmlProperty::setValueDirect(PCSTR value)
{
    ENTER(TRUE);

    SAFEFREE(this->value);

    strz_xml_unescape((PSTR)value);

    this->value = value;

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      AXXmlProperty::toString
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      PCSTR --
// ***************************************************************************
PCSTR AXXmlProperty::toString()
{
    HAXSTRING           str;

    ENTER(TRUE);

    SAFEFREE(toStringResult);

    if ((str = axstr_create(AXLSHORT)) != nil)
    {
        if (addToString(str))
        {
            toStringResult = axstr_freeze_c(str);
        }
        else
            axstr_destroy(str);
    }

    RETURN(toStringResult);
}
// ***************************************************************************
// FUNCTION
//      AXXmlProperty::addToStringName
// PURPOSE
//
// PARAMETERS
//      HAXSTRING str --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXXmlProperty::addToStringName(HAXSTRING      str)
{
    BOOL            result          = FALSE;

    ENTER(TRUE);

    result = (!nameSpace                            ||
             (  axstr_add_c(str, nameSpace)  &&
                axstr_add_c(str, ":")        )   )  &&
             axstr_add_c(str, name)                 ;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlProperty::addToString
// PURPOSE
//
// PARAMETERS
//      HAXSTRING str --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXXmlProperty::addToString(HAXSTRING      str)
{
    BOOL            result          = FALSE;

    ENTER(TRUE);

    result = addToStringName(str)                   &&
             axstr_add_c(str, "=\"")                &&
             (!value || axstr_add_c(str, value))    &&
             axstr_add_c(str, "\"")                 ;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlProperty::setName
// PURPOSE
//
// PARAMETERS
//      PCSTR nameSpace --
//      PCSTR name      --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXXmlProperty::setName(PCSTR   nameSpace,
                            PCSTR   name)
{
    BOOL            result          = FALSE;
    PCSTR           newNs           = nil;
    PCSTR           newName         = nil;


    ENTER(TRUE);

    if (!nameSpace || ((newNs = strz_dup(nameSpace)) != nil))
    {
        if ((newName = strz_dup(name)) != nil)
        {
            SAFEFREE(this->nameSpace);
            SAFEFREE(this->name);

            this->nameSpace = newNs;
            this->name      = newName;
            newNs           = nil;
            result          = true;
        }

        SAFEFREE(newNs);
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlProperty::getValueInt
// PURPOSE
//
// PARAMETERS
//      INT defaultValue --
// RESULT
//      INT --
// ***************************************************************************
INT AXXmlProperty::getValueInt(INT defaultValue)
{
    UINT            result          = defaultValue;

    ENTER(TRUE);

    if (value)
        result = atol(value);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlProperty::getValueInt
// PURPOSE
//
// PARAMETERS
//      INT defaultValue --
// RESULT
//      INT --
// ***************************************************************************
double AXXmlProperty::getValueDouble(double defaultValue)
{
    double          result          = defaultValue;

    ENTER(TRUE);

    if (value)
        result = atof(value);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlProperty::isNsEqual
// PURPOSE
//
// PARAMETERS
//      PCSTR nameSpace --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXXmlProperty::isNsEqual(PCSTR nameSpace)
{
    BOOL            result          = FALSE;

    ENTER(TRUE);

    if (nameSpace != this->nameSpace)
    {
        if (nameSpace && this->nameSpace)
        {
            result = !strcmp(nameSpace, this->nameSpace);
        }
    }
    else
        result = true;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlProperty::isNameEqual
// PURPOSE
//
// PARAMETERS
//      PCSTR name --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXXmlProperty::isNameEqual(PCSTR   name)
{
    BOOL            result          = FALSE;

    ENTER(TRUE);

    if (name != this->name)
    {
        if (name && this->name)
        {
            result = !strcmp(name, this->name);
        }
    }
    else
        result = true;

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlProperty::setValueInt
// PURPOSE
//
// PARAMETERS
//      INT value --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXXmlProperty::setValueInt(INT            value)
{
    BOOL            result          = FALSE;
    CHAR            buff            [ AXLLONG ];

    ENTER(TRUE);

    snprintf(buff, AXLLONG, "%d", value);

    result = setValue(buff);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlProperty::setValueLong
// PURPOSE
//
// PARAMETERS
//      INT64 value --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXXmlProperty::setValueLong(INT64          value)
{
    BOOL            result          = FALSE;
    CHAR            buff            [ AXLLONG ];

    ENTER(TRUE);

    snprintf(buff, AXLLONG, "%"LL"d", value);

    result = setValue(buff);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlProperty::setValueFloat
// PURPOSE
//
// PARAMETERS
//      double value --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXXmlProperty::setValueFloat(double    value)
{
    BOOL            result          = FALSE;
    CHAR            buff            [ AXLLONG ];

    ENTER(TRUE);

    snprintf(buff, AXLLONG, "%f", value);

    result = setValue(buff);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlProperty::setValueDate
// PURPOSE
//
// PARAMETERS
//      AXTIME time --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXXmlProperty::setValueDate(PAXDATE        pdate)
{
    BOOL            result          = FALSE;
    CHAR            buff            [ AXLLONG ];

    ENTER(TRUE);

    strz_from_date(buff, AXLLONG, "%Y-%M-%d %h:%m:%s", pdate);

    result = setValue(buff);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      AXXmlProperty::setValueDate
// PURPOSE
//
// PARAMETERS
//      AXTIME time --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL AXXmlProperty::setValueDate(AXTIME         time)
{
    BOOL            result          = FALSE;
    AXDATE          date;

    ENTER(TRUE);

    axtime_expand(&date, time, false);

    result = setValueDate(&date);

    RETURN(result);
}

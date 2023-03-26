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
#include <Json.h>


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      JsonSheet::JsonSheet
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
JsonSheet::JsonSheet()
{
    type    = AXDOMT_sheet;
    flags   = AXDOMF_FIRST;
}
// ***************************************************************************
// TYPE
//      Destructor
// FUNCTION
//      JsonSheet::~JsonSheet
// PURPOSE
//      Class destruction
// PARAMETERS
//          --
// ***************************************************************************
JsonSheet::~JsonSheet()
{

}
// ***************************************************************************
// FUNCTION
//      JsonSheet::create
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      JsonSheet * --
// ***************************************************************************
JsonSheet * JsonSheet::create()
{
    JsonSheet *     result          = nil;

    ENTER(true);

    if ((result = new JsonSheet) != nil)
    {
        //result->addFlag(AXDOMF_SHEET);

        if (!result->initObject(1))
        {

            SAFEDELETE(result);
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      JsonSheet::toString
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      PCSTR --
// ***************************************************************************
PCSTR JsonSheet::toString()
{
    PCSTR               result          = nil;
    HAXSTR              buff;

    ENTER(true);

//    printf("JsonSheet::toString stage 1\n");
    if ((buff = axstr_create(100)) != nil)
    {
        if (writeObject(buff, this))
        {
            result = axstr_freeze_c(buff);
        }
        else
            axstr_destroy(buff);
    }

//    printf("JsonSheet::toString stage 1 redsult = %p\n", result);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      JsonSheet::writeDepth
// PURPOSE
//
// PARAMETERS
//      HAXSTR         buff --
//      AXDomProperty* p    --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL JsonSheet::writeDepth(HAXSTR         buff,
                           AXDomProperty* p)
{
    BOOL            result          = false;
    UINT            de              = p->getDepth();

    ENTER(true);

    result = axstr_add_tabs_w_spaces(buff, de, 4);
    //result = axstr_add_tabs(buff, de);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      JsonSheet::writeProperty
// PURPOSE
//
// PARAMETERS
//      HAXSTR         buff --
//      AXDomProperty * p    --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL JsonSheet::writeProperty(HAXSTR         buff,
                              AXDomProperty * p)
{
    BOOL            result          = false;
    PCSTR           value           = p->getValue();

    ENTER(true);

    if (  ((p->getFlags() & AXDOMF_FIRST) || axstr_add_c(buff, ",\n"))  &&
          writeDepth(buff, p)                                           &&
          axstr_add_c(buff, "\"")                                       &&
          axstr_add_c(buff, p->getName())                               &&
          axstr_add_c(buff, "\": \"")                                   &&
          (!value || axstr_add_c(buff, value))                          &&
          axstr_add_c(buff, "\"")                                       )
    {
        result = true;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      JsonSheet::writeObject
// PURPOSE
//
// PARAMETERS
//      HAXSTR       buff --
//      AXDomObject * o    --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL JsonSheet::writeObject(HAXSTR         buff,
                            AXDomObject *    o)
{
    BOOL            result          = false;
    AXDomProperty * p;
    AXDOMT          type            = o->getType();
    PCSTR           name            = o->getName();

    ENTER(true);

    if (  ((o->getFlags() & AXDOMF_FIRST) || axstr_add_c(buff, ",\n"))  &&
//          ((o->getFlags() & AXDOMF_SHEET) ||
          (
              writeDepth(buff, o)                                           &&
              (  (!name && axstr_add_c(buff, "\n")) ||
                 ( axstr_add_c(buff, "\"")                           &&
                             axstr_add_c(buff, name)        &&
                             axstr_add_c(buff, "\":\n")     ))              &&
              writeDepth(buff, o)                                           &&
              axstr_add_c(buff, type == AXDOMT_array ? "[\n"  : "{\n")
          ))
//        )
    {
        result = true;

        for (UINT idx = 0; result && ((p = o->getElementByIndex(idx)) != nil); ++idx)
        {
            switch (p->getType())
            {
                case AXDOMT_property:
                    result = writeProperty(buff, p);
                    break;

                case AXDOMT_array:
                case AXDOMT_object:
                    result = writeObject(buff, (AXDomObject*)p);
                    break;

                default:
                    //AXTRACE("JSON OBJECT TYPE ERROR ON writeObject = %X!\n", obj->type);
                    result = false;
                    break;
            }
        }

        if (result)
        {
//            if (!(o->getFlags() & AXDOMF_SHEET))
            {
                result = axstr_add_c(buff, "\n") &&
                         writeDepth(buff, o) &&
                         axstr_add_c(buff, type == AXDOMT_array ? "]" : "}");
            }
        }
    }

    RETURN(result);
}

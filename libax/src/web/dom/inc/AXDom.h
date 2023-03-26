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

#ifndef __AXDOMH__
#define __AXDOMH__

#include <arsenal.h>
#include <axstring.h>
#include <records.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|(9)----------------|(30)---------------------------------------------

#define AXDOM_DEF_INITSIZE  10

//#define AXDOMF_SHEET        0x0200
#define AXDOMF_FIRST        0x0100


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef enum
{
    AXDOMT_blank            = 0,
    AXDOMT_property,
    AXDOMT_object,
    AXDOMT_sheet,
    AXDOMT_array
} AXDOMT;

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|(9)------------|(25)-------------------(|(50)----------|(65)---------


class AXDomProperty
{
//        friend          AXDomProperty * _AXDomObjectFree(AXDomProperty * obj);
protected:
        PCSTR           name;
        PCSTR           value;
        UINT            flags;
        AXDOMT          type;
        UINT            depth;
public:

                        AXDomProperty           ();

virtual                 ~AXDomProperty          ();

static  AXDomProperty * create                  (PCSTR          name,
                                                 PCSTR          value = nil);

        PCSTR           getName                 ()
        { return name; }

        PCSTR           getValue                ()
        { return value; }

        AXDOMT          getType                 ()
        { return type; }

        UINT            getFlags                ()
        { return flags; }

        void            addFlag                 (UINT           flag)
        {
            flags |= flag;
        }

        void            setDepth                (UINT           nDepth)
        { depth = nDepth; }

        UINT            getDepth                ()
        { return depth; }

        BOOL            setValue                (PCSTR          value,
                                                 BOOL           copy  = true);

};

class AXDomObject: public AXDomProperty
{
protected:
        HRECSET         collection;

static  AXDomObject *    create                 (PCSTR          name,
                                                 UINT           initSize);

        BOOL            addToCollection         (AXDomProperty * p);

        BOOL            initObject              (UINT           initSize);
public:
                        AXDomObject             ();

virtual                 ~AXDomObject            ();

        AXDomProperty * getElementByIndex       (UINT           index);

//        AXDomProperty * getElementByName        (PCSTR          name);

        void            setType                 (AXDOMT         newType)
        { type = newType; }

        AXDomObject *   addObject               (PCSTR          name,
                                                 UINT           initSize = AXDOM_DEF_INITSIZE);

        AXDomProperty * addProperty             (PCSTR          name,
                                                 PCSTR          value    = nil);

        AXDomProperty * addPropertyBool         (PCSTR          name,
                                                 BOOL           value)
        { return addProperty(name, value ? "true" : "false"); }

        AXDomProperty *  addPropertyUint        (PCSTR          name,
                                                 UINT           value);

        AXDomProperty *  addProperty            (PCSTR          name,
                                                 double         value);

        AXDomProperty *  addProperty            (PCSTR          name,
                                                 ULONG          value);
};

#endif                                      //  #ifndef __AXDOMH__

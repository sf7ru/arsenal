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

#ifndef __JSONSHEETH__
#define __JSONSHEETH__

#include <AXDom.h>

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|(9)------------|(25)-------------------(|(50)----------|(65)---------

class JsonSheet;
class JsonParser;

class JsonSheet: public AXDomObject
{
//        friend class    JsonParser;
protected:
//        HAXSTR          buff;

        BOOL            writeDepth              (HAXSTR         buff,
                                                 AXDomProperty* p);


        BOOL            writeProperty           (HAXSTR         buff,
                                                 AXDomProperty* p);

        BOOL            writeObject             (HAXSTR         buff,
                                                 AXDomObject *  o);

//        void            setType                 ()

public:
                        JsonSheet               ();

virtual                 ~JsonSheet              ();

static  JsonSheet *     create                  ();

        PCSTR           toString                ();
};


class JsonParser
{
//        AXDomObject *   prevObject;
//        AXDomObject *   currObject;
//        CHAR            name                    [ AXLLONG ];

        HAXSTR          name;
        HAXSTR          value;

        UINT            phase;

        void            addChar                 (CHAR           c);

        PCSTR           parseObject             (AXDomObject *  o,
                                                 PCSTR          source);

        PCSTR           notNullName             ();

public:
                        JsonParser              ();

                        ~JsonParser             ();

        PCSTR           getUnnamedName          ()
        { return " "; }

        JsonSheet *     parse                   (PCSTR          source);
};

#endif                                      //  #ifndef __JSONSHEETH__

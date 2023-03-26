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

#ifndef __AXXMLDOMH__
#define __AXXMLDOMH__

#include <arsenal.h>
#include <records.h>
#include <axstring.h>
#include <axlog.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class AXXmlProperty
{
        PCSTR           name;
        PCSTR           nameSpace;
protected:
        PCSTR           value;
        PCSTR           toStringResult;

                        AXXmlProperty           ();

static  AXXmlProperty * create                  (PCSTR          nameSpace,
                                                 PCSTR          name);

        BOOL            addToStringName         (HAXSTR         str);

virtual BOOL            init                    (PCSTR          nameSpace,
                                                 PCSTR          name);


        BOOL            isNsEqual               (PCSTR          nameSpace);

        BOOL            isNameEqual             (PCSTR          name);

public:
virtual                 ~AXXmlProperty          ();

        BOOL            setName                 (PCSTR          nameSpace,
                                                 PCSTR          name);

        PCSTR           getName                 ()
        { return name; }

        PCSTR           getNameSpace()
        { return nameSpace; }

        PCSTR           getValue                ()
        { return value; }

        INT             getValueInt             (INT            defaultValue);

        double          getValueDouble          (double         defaultValue);

        BOOL            setValue                (PCSTR          value);

        BOOL            setValueInt             (INT            value);

        BOOL            setValueLong            (INT64          value);

        BOOL            setValueFloat           (double         value);

        BOOL            setValueDate            (PAXDATE        time);

        BOOL            setValueDate            (AXTIME         time);

        void            setValueDirect          (PCSTR          value);

        PCSTR           toString                ();

virtual BOOL            addToString             (HAXSTR         str);

        BOOL            isNameEqual             (PCSTR          nameSpace,
                                                 PCSTR          name)
        { return isNsEqual(nameSpace) && isNameEqual(name); }
};

class AXXmlParser;

class AXXmlNode: public AXXmlProperty
{
                        friend                  class AXXmlParser;
protected:
        BOOL            doc;
        UINT            nesting;
        HRECSET         properties;
        HRECSET         nodes;

        AXXmlNode *     parent;

                        AXXmlNode               ();

        BOOL            init                    (UINT           nesting,
                                                 PCSTR          nameSpace,
                                                 PCSTR          name);

static  AXXmlNode *     create                  (UINT           nesting,
                                                 PCSTR          nameSpace,
                                                 PCSTR          name);

        BOOL            addToStringProperties   (HAXSTR         str);


virtual BOOL            addToString             (HAXSTR         str);


        UINT            findNodeByPtr           (AXXmlNode *    node);

public:

virtual                 ~AXXmlNode              ();


        AXXmlNode *     addNode                 (PCSTR          nameSpace,
                                                 PCSTR          name);

        AXXmlNode *     addNode                 (PCSTR          name)
        { return addNode(getNameSpace(), name); }

        AXXmlProperty * addProperty             (PCSTR          nameSpace,
                                                 PCSTR          name,
                                                 PCSTR          value);

        AXXmlProperty * addProperty             (PCSTR          name,
                                                 PCSTR          value)
        { return addProperty(getNameSpace(), name, value); }

        UINT            nodesCount              ()
        { return recset_count(nodes); }

        AXXmlNode *     getNode                 (UINT           position)
        { return (AXXmlNode*)recset_get(nodes, position); }

        AXXmlNode *     getParent               ()
        { return parent; }

        AXXmlNode *     findNode                (PCSTR          nameSpace,
                                                 PCSTR          name,
                                                 UINT           index);

        AXXmlNode *     findNode                (PCSTR          nameSpace,
                                                 PCSTR          name)
        { return findNode(nameSpace, name, 0); }

        AXXmlProperty * findProperty            (PCSTR          nameSpace,
                                                 PCSTR          name,
                                                 UINT           index);

        AXXmlProperty * findProperty            (PCSTR          nameSpace,
                                                 PCSTR          name)
        { return findProperty(nameSpace, name, 0); }


        AXXmlNode *     addNodeAndValue         (PCSTR          name,
                                                 PCSTR          value);

        AXXmlNode *     addNodeAndValueInt      (PCSTR          name,
                                                 INT            value);

        AXXmlNode *     addNodeAndValueLong     (PCSTR          name,
                                                 INT64          value);

        AXXmlNode *     addNodeAndValueFloat    (PCSTR          name,
                                                 double         value);

        AXXmlNode *     addNodeAndValueDate     (PCSTR          name,
                                                 AXTIME         value);

        AXXmlNode *     delNode                 (AXXmlNode *    node);

};
class AXXmlDoc: public AXXmlNode
{
protected:
                        AXXmlDoc                ();

        BOOL            init                    (PCSTR          encoding);
public:

virtual                 ~AXXmlDoc               ();

        BOOL            parse                   (PCSTR          source,
                                                 HAXLOG         logSession);

static  AXXmlDoc *      create                  (PCSTR          encoding);

static  AXXmlDoc *      create                  (PCSTR          source,
                                                 PCSTR          encoding,
                                                 HAXLOG         logSession);
};


#endif //  #ifndef __AXXMLDOMH__

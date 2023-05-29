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

#ifndef __CAN_H__
#define __CAN_H__

#include <arsenal.h>
#include <axthreads.h>

#define CAN_MAX_IFACES      2
#define SINGLETON_UNIQUES_NUMBER    CAN_MAX_IFACES
#include <singleton_template.hpp>
#undef SINGLETON_UNIQUES_NUMBER

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct __tag_CANMODE
{
    UINT            reserved        : 18;
    UINT            baudrateKbps	: 14;
} CANMODE, * PCANMODE;


typedef struct __tag_CANMESSAGE
{
    UINT            id;        
    UINT            dlc;
    U8              data                [ 8 ];
} CANMESSAGE, * PCANMESSAGE;


// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class Can: public SingletonTemplate<Can>
{
        INT             ifaceNo;
        HAXMUTEX        LCK;

        CANMODE         mMode;

protected:


        void            constructClass          ();
        BOOL            initBase                ();
        void            deinitBase              ();

        BOOL            initMsp                 (PVOID          h);

public:

                        Can                     ();
                        ~Can                    ();

        BOOL            init                    (CANMODE        mode);

        void            deinit                  ();

        INT             receive                 (PCANMESSAGE    msg,
                                                 UINT           TO);

        INT             send                    (PCANMESSAGE    msg,
                                                 UINT           TO);

        CANMODE         getMode                 ()
        { return mMode; }

        BOOL            setFilterMask           (U32            id,     
                                                 U32            mask);

};

#endif // #ifndef __CAN_H__

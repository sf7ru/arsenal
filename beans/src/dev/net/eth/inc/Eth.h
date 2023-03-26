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

#ifndef __ETH_H__
#define __ETH_H__

#include <arsenal.h>
#include <axthreads.h>

#define ETH_MAX_IFACES      2
#define SINGLETON_UNIQUES_NUMBER    ETH_MAX_IFACES
#include <singleton_template.hpp>
#undef SINGLETON_UNIQUES_NUMBER

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

typedef enum
{
    ETHIFTYPE_none = 0,
    ETHIFTYPE_phy,
    ETHIFTYPE_mac
} ETHIFTYPE;

typedef enum
{
    ETHIFMODE_none = 0,
    ETHIFMODE_MII,
    ETHIFMODE_RMII,
    ETHIFMODE_GMII,
    ETHIFMODE_RGMII,
    ETHIFMODE_SGMII,
} ETHIFMODE;

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef struct __tag_ETHMODE
{
    UINT            fullDuplex  :  1;
    UINT            speedMbps	: 20;
} ETHMODE, * PETHMODE;

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class EthPhy;

class Eth: public SingletonTemplate<Eth>
{
        INT             ifaceNo;
        HAXMUTEX        LCK;

        ETHMODE         mode;

        EthPhy *        phy;

protected:


        void            constructClass          ();
        BOOL            initBase                ();
        void            deinitBase              ();

        BOOL            initMsp                 ();

public:

                        Eth                     ();
                        ~Eth                    ();

        BOOL            init                    (EthPhy *       phy);

        void            deinit                  (UINT           ifaceNo = 1);

        INT             read                    (UINT           reg,
                                                 PVOID          data,
                                                 INT            len);

        INT             write                   (UINT           reg,
                                                 PVOID          data,
                                                 INT            len);

        ETHMODE         getMode                 ()
        { return mode; }
};

#endif // #ifndef __ETH_H__

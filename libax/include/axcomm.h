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

#ifndef __AXCOMMH__
#define __AXCOMMH__

#include <arsenal.h>


#define AXRDS_IS_OK(a)        ((a) > AXRDS_expired)
#define AXRDS_IS_FINAL(a)     ((a) != AXRDS_enroute)

#define MPRDS_IS_OK(a)        (((a) == MPRDS_enroute) || ((a) == MPRDS_delivered))
#define MPRDS_IS_FINAL(a)     ((a) != MPRDS_enroute)


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// ENUMERATION
//      AXRDS
// PURPOSE
//      Registered Delivery Status
// ***************************************************************************
typedef enum
{
    AXRDS_unsent           = 0,             // Message unsent, treat as error
    AXRDS_dropped,                          // Message dropped
    AXRDS_expired,                          // Message expired
    AXRDS_enroute,                          // Message enrouted
    AXRDS_delivered,                        // Message delivered
    AXRDS_user                              // User defined message status
} AXRDS;

// ***************************************************************************
// ENUMERATION
//      AXMPC
// PURPOSE
//      Message Priority Class
// ***************************************************************************
typedef enum
{
  AXMPC_none            = 0,
  AXMPC_low,
  AXMPC_normal,
  AXMPC_high
} AXMPC;

#endif                                      //  #ifndef __AXCOMMH__

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

#ifndef __AXMACROSH__
#define __AXMACROSH__

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define MAC_MIN(a,b)            ((a) < (b) ? (a) : (b))
#define MAC_MAX(a,b)            ((a) > (b) ? (a) : (b))

#define MAC_TF(a)               ((a) ? "TRUE"   : "FALSE")
#define MAC_tf(a)               ((a) ? "true"   : "false")
#define MAC_YN(a)               ((a) ? "YES"    : "NO")
#define MAC_yn(a)               ((a) ? "yes"    : "no")

#define MAC_ZIS(a,b)            (*a ? a : b)


#define SUBSET_INCLUDE_(a)         <a>
#define SUBSET_INCLUDE2_(a,b)      <a.b>

#define USE_INCLUDE(a)          SUBSET_INCLUDE_(USE_##a.h)
#define SUBSYS_INCLUDE(a)       SUBSET_INCLUDE_(SUBSYS_##a.h)
#define CLASS_INCLUDE(a)        SUBSET_INCLUDE_(CLASS_##a.h)
#define CLASS_INCLUDE(a)        SUBSET_INCLUDE_(CLASS_##a.h)
#define CUSTOMBOARD_INCLUDE     SUBSET_INCLUDE2_(customboard,TARGET_CUSTOMBOARD.h)

#endif                                      //  #ifndef __AXMACROSH__

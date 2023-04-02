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

#ifndef __AXMOTION_H__
#define __AXMOTION_H__

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define MOTFLAG_NO_ACC      0x01       // No acceleration
#define MOTFLAG_NO_DEC      0x02       // No deceleration
#define MOTFLAG_EMG         0x08       // Emergency

#define MOTFLAG_FORCE_LLS   0x40
#define MOTFLAG_FORCE_HLS   0x80
#define MOTFLAG_DIRECT      (MOTFLAG_NO_ACC | MOTFLAG_NO_DEC)


#define MOTFLAG_REACHED_LLS 0x01
#define MOTFLAG_REACHED_HLS 0x02
#define MOTFLAG_REACHED_CLS 0x04


// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------


#define MOTPHASE_idle       MOTPHASE_still

typedef enum
{
    MOTPHASE_unknown            = 0,
    MOTPHASE_still              = 1, // Still standing
    MOTPHASE_accel              = 2, // Acceleration
    MOTPHASE_decel              = 3, // Deceleration
    MOTPHASE_uniform            = 4, // Uniform motion
    MOTPHASE_user               = 5, // user defined phase
    MOTPHASE_COUNT
} MOTPHASE;



#endif // __AXMOTION_H__

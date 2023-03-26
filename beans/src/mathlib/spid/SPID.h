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

#ifndef __SPID_H__
#define __SPID_H__

#include <arsenal.h>

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class SPID
{
public:
        double          _dt;        // Loop interval time
        double          _max;       // Maximum value of manipulated variable
        double          _min;       // Minimum value of manipulated variable
        double          _Kp;        // Proportional gain
        double          _Ki;        // Integral gain
        double          _Kd;        // Derivative gain
        double          _pre_error;
        double          _integral;

                        ~SPID                   ();

                        SPID                    (double         dt,
                                                 double         max,
                                                 double         min,
                                                 double         Kp,
                                                 double         Kd,
                                                 double         Ki);

        double          calculate               (double         setpoint,
                                                 double         pv);
};



#endif // #define __SPID_H__


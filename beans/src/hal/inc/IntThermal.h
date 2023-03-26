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


#ifndef __AX_IntThermal_H__
#define __AX_IntThermal_H__

#include <arsenal.h>
//#include <ThermalSensor.h>

#include <customboard.h>
#include <singleton_template.hpp>

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class IntThermal: public SingletonTemplate<IntThermal>
{
protected:

public:
                        IntThermal              ();

                        ~IntThermal             ();

        BOOL            init                    (UINT           debounceRate);


        float           readTemperature         (UINT           TO);

};

#endif // __AX_IntThermal_H__

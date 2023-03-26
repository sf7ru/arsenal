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

#include <SpatialSensor.h>
#include <SpatialSensorCalib.h>
#include <mathlib.h>
#include <declination.h>
#include <math.h>


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

OrbCalibration::OrbCalibration()
{
    x                   = nil;
    y                   = nil;
    z                   = nil;
}
OrbCalibration::~OrbCalibration()
{
    SAFEFREE(x);
    SAFEFREE(y);
    SAFEFREE(z);
}
// ***************************************************************************
// FUNCTION
//      OrbCalibration::init
// PURPOSE
//
// PARAMETERS
//      UINT    count     --
//      double  latitude  --
//      double  longitude --
//      AXUTIME time      --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL OrbCalibration::init(UINT           count,
                              double         latitude,
                              double         longitude,
                              AXUTIME        time)
{
    BOOL            result          = false;

    ENTER(true);

    /* 45 seconds */
    //uint64_t calibration_interval = 45 * 1000 * 1000;

    if ((x = (float*)MALLOC(sizeof(float) * count)) != nil)
    {
        if ((y = (float*)MALLOC(sizeof(float) * count)) != nil)
        {
            if ((z = (float*)MALLOC(sizeof(float) * count)) != nil)
            {
                /* limit update rate to get equally spaced measurements over time (in ms) */
                //orb_set_interval(sub_mag, (calibration_interval / 1000) / count);

                max     = count;
                counter = 0;
                result  = true;
            }
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      OrbCalibration::Orb
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL OrbCalibration::calculate(PORBCALIB magCalibData)
{
    BOOL            result          = false;
    float           sphere_x;
    float           sphere_y;
    float           sphere_z;
    float           sphere_radius;

    ENTER(true);

    sphere_fit_least_squares(x, y, z, counter, 100, 0.0f, &sphere_x, &sphere_y, &sphere_z, &sphere_radius);

    if (isfinite(sphere_x) && isfinite(sphere_y) && isfinite(sphere_z))
    {
        magCalibData->offset.x    = sphere_x;
        magCalibData->offset.y    = sphere_y;
        magCalibData->offset.z    = sphere_z;

        for (int i = 0; i < 3; i++)
        {
            magCalibData->scale[i].x = 1.0;
            magCalibData->scale[i].y = 1.0;
            magCalibData->scale[i].z = 1.0;
        }


        AXTRACE("new mag calib data: %f %f %f ",
                magCalibData.offsetX,
                magCalibData.offsetY,
                magCalibData.offsetZ);

        for (int i = 0; i < 3; i++)
        {

            AXTRACE("  scale row %d: %f %f %f ",
                    i,
                    magCalibData.scale[i].x,
                    magCalibData.scale[i].y,
                    magCalibData.scale[i].z);
        }

        result                  = true;
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      OrbCalibration::addTillDone
// PURPOSE
//
// PARAMETERS
//      mag_report report --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL OrbCalibration::addTillDone(PSPATIALVECTOR     report)
{
    BOOL            result          = false;

    ENTER(true);

    x[counter] = (float)report->x;
    y[counter] = (float)report->x;
    z[counter] = (float)report->x;

    counter++;

    result      = (counter >= max);

    RETURN(result);
}

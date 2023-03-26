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

#ifndef __SPATIALSENSOR_H__
#define __SPATIALSENSOR_H__

#include <arsenal.h>
#include <customboard.h>
#include <axnavi.h>

//#include <Orientation.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define IMPOSSIBLE_TEMPERATURE      -128

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class SpatialSensorUnit
{
public:
        BOOL            can;

        UINT            errors;

        double          scale;

                        SpatialSensorUnit       ()
                        { can = false; errors = 0; scale = 1.0; }

virtual BOOL            getCalibData            (PVOID          rawData)
        { return false; }

virtual BOOL            setCalibData            (PVOID          rawData)
        { return false; }

virtual BOOL            unsetCalibData          ()
        { return false; }

virtual UINT            getCalibDataSize        ()
        { return false; }

virtual BOOL            readRaw                 (SPATIALVECTORI &report)
        { return false; }

virtual BOOL            read                    (SPATIALVECTOR  &report)
        { return false; }

virtual BOOL            resetCalibOrientation   (ORIAXIS   ori)
        { return false; }

virtual BOOL            reset                   ()
        { return true; }

virtual BOOL            isReady                 ()
        { return false; }

virtual BOOL            setRate                 (UINT           rate)
        { return true; }

virtual BOOL            setRange                (UINT           range)
        { return true; }

virtual double          getScale                ()
        { return scale; }

virtual double          getScale                (ORIAXIS        axis)
        { return scale; }
};

class SpatialSensor
{
protected:

public:
                        SpatialSensor           (): accel(*this), gyro(*this), mag(*this), incl(*this), fused(*this) { }


        class Accel: public SpatialSensorUnit
        {
                friend class    SpatialSensor;

        protected:
                SpatialSensor&  parent;

                                Accel           (SpatialSensor &       x): parent(x)
                                {};
        } accel;

        class Gyro: public SpatialSensorUnit
        {
                friend class    SpatialSensor;

        protected:
                SpatialSensor&  parent;

                                Gyro            (SpatialSensor &       x): parent(x)
                                {};
        } gyro;

        class Mag: public SpatialSensorUnit
        {
                friend class    SpatialSensor;

        protected:
                SpatialSensor&  parent;

                                Mag             (SpatialSensor &       x): parent(x)
                                {};
        } mag;

        class Incl: public SpatialSensorUnit
        {
                friend class    SpatialSensor;

        protected:
                SpatialSensor&  parent;

        public:
                Incl            (SpatialSensor &       x): parent(x)
                {};
        } incl;

        class Fused: public SpatialSensorUnit
        {
                friend class    SpatialSensor;

        protected:
                SpatialSensor&  parent;

        public:
                Fused           (SpatialSensor &       x): parent(x)
                {};
        } fused;


// -----|---------------|-----------------------(|--------------|-------------


virtual BOOL            setRate                 (UINT           rateHz)
        { return gyro.setRate(rateHz) && accel.setRate(rateHz) && mag.setRate(rateHz); }

virtual float           readTemp                ()
        { return IMPOSSIBLE_TEMPERATURE; }

static  BOOL            compareVectors          (PSPATIALVECTOR     r1,
                                                 PSPATIALVECTOR     r2,
                                                 double             diff);

static  BOOL            compareVectors          (PSPATIALVECTORI r1,
                                                 PSPATIALVECTORI r2);


};

#endif // #ifndef __SPATIALSENSOR_H__

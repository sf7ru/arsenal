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

#ifndef __Calibration_H__
#define __Calibration_H__

#include <arsenal.h>
#include <SpatialSensor.h>
#include <axdbg.h>

#define CALIB_MAX_ALLOCATINS        15

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

typedef struct _tag_ORBCALIB
{
    struct
    {
        double              x;
        double              y;
        double              z;

    }                       offset;

    struct
    {
        double              x;
        double              y;
        double              z;

    }                       scale       [ 3 ];

} ORBCALIB, * PORBCALIB;

typedef struct _tag_ORBCALIBDATA
{

    double                  offset      [ 3 ];
    double                  scale       [ 9 ];

} ORBCALIBDATA, * PORBCALIBDATA;


// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------


class MagnetoCalibration
{
protected:
        DBGMCB          mcbs                    [ CALIB_MAX_ALLOCATINS ];
        double *        D;
        int             msCalibrationMeasuresCnt_;

        double          hmXY;
        double          hmZ;

        INT             memNum;
        INT             memPeakNum;
        INT             memCurrent;
        INT             memPeak;
        INT             memLargest;

        INT             mscMeasuresCnt_;

        PVOID           memFree                 (PVOID          mem);

        PVOID           memAlloc                (int            size,
                                                 PCSTR          purpose);


public:
                        ~MagnetoCalibration     ();

                        MagnetoCalibration      ();

        BOOL            init                    (UINT           count,
                                                 double         latitude,
                                                 double         longitude,
                                                 AXUTIME        date);

        void            deinit                  ();

        BOOL            addTillDone             (PSPATIALVECTOR report);

        BOOL            calculate               (PORBCALIB      magCalibData);

        void            memStat                 ();

};

class OrbCalibration
{
protected:
        float *         x;
        float *         y;
        float *         z;

        UINT            counter;
        UINT            max;
public:
                        ~OrbCalibration         ();

                        OrbCalibration          ();

        BOOL            init                    (UINT           count,
                                                 double         latitude,
                                                 double         longitude,
                                                 AXUTIME        date);

        BOOL            addTillDone             (PSPATIALVECTOR report);

        BOOL            calculate               (PORBCALIB      magCalibData);


        void            memStat                 ();
};


#endif // #define __Calibration_H__

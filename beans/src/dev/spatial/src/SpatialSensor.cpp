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
#include <math.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

BOOL SpatialSensor::compareVectors(PSPATIALVECTORI          r1,
                                   PSPATIALVECTORI          r2)
{
    BOOL            result          = false;

    ENTER(true);

    result = ((r1->x == r2->x) && (r1->y == r2->y) && (r1->z == r2->z));

    RETURN(result);
}

BOOL SpatialSensor::compareVectors(PSPATIALVECTOR     r1,
                                   PSPATIALVECTOR     r2,
                                   double             diff)
{
    BOOL            result          = false;

    ENTER(true);

    result = ((fabs(r1->x - r2->x) <= diff) && (fabs(r1->y - r2->y) <= diff) && (fabs(r1->z - r2->z) <= diff));

    RETURN(result);
}

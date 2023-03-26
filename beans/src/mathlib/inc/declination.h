#ifndef DECLINATION_CALCULATOR_H_INCLUDED
#define DECLINATION_CALCULATOR_H_INCLUDED

//#include <string>
//#include <vector>

#include <axtime.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define RAD2DEG (180.0/PI)
#define PI (3.141592654)
#define IEXT 0
#define EXT_COEFF1 (double)0
#define EXT_COEFF2 (double)0
#define EXT_COEFF3 (double)0

#define RECL 81             /* длина записи в файле с данными для модели расчета магнитного склонения -- spherical harmonic coefficients */
#define MAXINBUFF RECL+14
#define MAXREAD MAXINBUFF-2
#define PATH MAXREAD

#define MAXDEG 13
#define MAXCOEFF (MAXDEG*(MAXDEG+2)+1) /* index starts with 1!, (from old Fortran?) */


typedef struct __tag_ModelData
{
    double f1;
    double f2;
    double f3;
    double f4;
    double f5;
    double f6;
    double f7;
} ModelData;

// структура, описывающая модель (в файлах IGRF [International Geomagnetic Reference Field] и WMM [World Magnetic Model])
struct ModelInfo
{
  // содержательные аспекты
  double epoch;             // epoch of model.
  int max1;                 // Main field coefficient.
  int max2;                 // Secular variation coefficient.
  int max3;                 // Acceleration coefficient.
  double yrmin;             // Min year of model.
  double yrmax;             // Max year of model.
  double altmin;            // Minimum height of selected model.
  double altmax;            // Maximum height of model.
  // технические аспекты
  ModelData *   data;
  int           size;
};

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

// структура для результатов расчета магнитного склонения и интенсивности магнитного поля
struct CalcResult
{
        // склонение по азимуту
        double          declination;               // Declination (D) positive east, in degrees and minutes
        double          annualDeclinationDelta;    // Annual change (dD) positive east, in minutes per year
        // склонение по вертикали
        double          inclination;               // Inclination (I) positive down, in degrees and minutes
        double          annualInclinationDelta;    // Annual change (dI) positive down, in minutes per year
        // интенсивности поля в нанотеслах
        double          horizontalIntensity;       // Horizontal Intensity (H), in nanoTesla
        double          annualHorizontalChange;    // Annual change (dH) in nanoTesla per year
        double          xIntensity;                // North Component of H (X), positive north, in nanoTesla
        double          annualChangeOfXIntensity;  // Annual change (dX) in nanoTesla per year
        double          yIntensity;                // East Component of H (Y), positive east, in nanoTesla
        double          annualChangeOfYIntensity;  // Annual change (dY) in nanoTesla per year
        double          zIntensity;                // Vertical Intensity (Z), positive down, in nanoTesla
        double          annualChangeOfZIntensity;  // Annual change (dZ) in nanoTesla per year
        double          totalField;                // Total Field (F), in nanoTesla
        double          annualChangeOfTotalField;  // Annual change (dF) in nanoTesla per year

        // ctor
        CalcResult();
};

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------


class DeclinationCalc
{
public:
                        DeclinationCalc();

  // функция инициализации
        bool            init                    ();

  // функция получения информации о магнитном склонении для заданных координат и даты
        bool            getDeclination          (CalcResult &   result,
                                                 double         latitude,
                                                 double         longitude,
                                                 double         altitude = 0.0,
                                                 PAXDATE        curDate = nil);
private:
        double          gh1[MAXCOEFF];      // Schmidt quasi-normal internal spherical harmonic coeff.
        double          gh2[MAXCOEFF];      // Schmidt quasi-normal internal spherical harmonic coeff.
        double          gha[MAXCOEFF];
        double          ghb[MAXCOEFF];

        int             nmax;

        double          d;
        double          f;
        double          h;
        double          i;
        double          dtemp;
        double          ftemp;
        double          htemp;
        double          itemp;
        double          x;
        double          y;
        double          z;
        double          xtemp;
        double          ytemp;
        double          ztemp;

        int             modelInfosCount;
        ModelInfo *     modelInfos;         // описания моделей для разных интервалов времени
        double          minyr;                      // Min year of all models
        double          maxyr;                      // Max year of all models

        int             getshc                  (ModelData *    modelData,
                                                 int            modelSize,
                                                 int            iflag,
                                                 int            nmax_of_gh,
                                                 int            gh);

        int             shval3                  (int            igdgc,
                                                 double         flat,
                                                 double         flon,
                                                 double         elev,
                                                 int            nmax,
                                                 int            gh,
                                                 int            iext,
                                                 double         ext1,
                                                 double         ext2,
                                                 double         ext3);

        int             dihf                    (int            gh);

        int             interpsh                (double         date,
                                                 double         dte1,
                                                 int            nmax1,
                                                 double         dte2,
                                                 int            nmax2,
                                                 int            gh);

        int             extrapsh                (double         date,
                                                 double         dte1,
                                                 int            nmax1,
                                                 int            nmax2,
                                                 int            gh);

}; // class-definition-end: DeclinationCalc

#endif



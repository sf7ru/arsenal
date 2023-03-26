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
#include <math.h>
#include <stdio.h>
#
// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

MagnetoCalibration::MagnetoCalibration()
{
    mscMeasuresCnt_             = 0;
    msCalibrationMeasuresCnt_   = 0;
    hmZ                         = 0;
    hmXY                        = 0;
    D                           = nil;

    memCurrent                  = 0;
    memLargest                  = 0;
    memPeak                     = 0;
    memPeakNum                  = 0;
    memNum                      = 0;

    for (int i = 0 ; i < CALIB_MAX_ALLOCATINS; i++)
    {
        mcbs[i].ptr = nil;
    }
}
MagnetoCalibration::~MagnetoCalibration()
{
    deinit();
}
void MagnetoCalibration::deinit()
{
    if (D)
        memFree(D);
}
// ***************************************************************************
// FUNCTION
//      MagnetoCalibration::init
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
BOOL MagnetoCalibration::init(UINT           count,
                              double         latitude,
                              double         longitude,
                              AXUTIME        time)
{
    BOOL            result          = false;
    AXDATE          date;

    ENTER(true);

    // считываем данные по силе магнитного поля для расчета "нормали к магнитному полюсу Земли"
    CalcResult declinationCalcResult;
    DeclinationCalc declinationCalculator;

    axdate_from_utime(&date, time);

    if (isnan(latitude) || (declinationCalculator.init() && declinationCalculator.getDeclination(declinationCalcResult, latitude, longitude, 0, &date)))
    {
        if (isnan(latitude))
        {
            hmXY = 0.569;
            hmZ = 0.569;
        }
        else
        {
            double fieldInGausses = declinationCalcResult.totalField * 1e-9 * 1e+4; // nanoTesla -> gauss     (1 тесла [Тл] = 10000 гаусс [Гс])
            // не соответствует AppNote, раздел 3.2 (http://www.pololu.com/file/download/LSM303DLH-compass-app-note.pdf?file_id=0J434)
            //hmXY = fieldInGausses * 1055; // умножаем на LBS/Gauss                         -- такое значение поля (в LBS) должно быть в данной точке и в данное время
            //hmZ = fieldInGausses * 950;   // (по оси Z LBS/Gauss другое судя по AppNote)
            hmXY = fieldInGausses * 1100;
            hmZ = fieldInGausses * 1100;
            //controllerState_->logInfoPoint( MSG_PREFIX + "TotalField (nTl) =" + boost::str(boost::format("%1$+10.2f") % declinationCalcResult.totalField) );
            //controllerState_->logInfoPoint( MSG_PREFIX + "TotalField (Gs)  =" + boost::str(boost::format("%1$+10.6f") % fieldInGausses) );
            //controllerState_->logInfoPoint( MSG_PREFIX + "hmXY=" + boost::str(boost::format("%1$.2f") % hmXY) );
            //controllerState_->logInfoPoint( MSG_PREFIX + "hmZ=" + boost::str(boost::format("%1$.2f") % hmZ) );
        }

        // создадим контейнер для данных, снятых с магнитного сенсора, участвующих в расчетах калибровочных значений
        msCalibrationMeasuresCnt_ = count;

        if ((D = (double*)memAlloc(10 * msCalibrationMeasuresCnt_ * sizeof(double), "D array")) != nil)
        {
            // количество выполненных итераций считывания данных
            mscMeasuresCnt_ = 0;

            result = true;
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      MagnetoCalibration::magneto
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL MagnetoCalibration::calculate(PORBCALIB magCalibData)
{
    BOOL            result          = true;

    ENTER(true);

    // реализация метода калибровки заимствована отсюда
    // https://sites.google.com/site/sailboatinstruments1/c-language-implementation

    //!!!
    // открываем файл для записи калибровочных значений
    //  std::string cvFullName = AppOptions::getProgDir() + "/../../" + MsCalibrationValuesFile;
    //  std::ofstream ofs( cvFullName.c_str() );

    // рабочие переменные
    double *S, *C, *S11, *S12, *S12t, *S22, *S22_1, *S22a, *S22b, *SS, *E, *U, *SSS;
    double *eigen_real, *eigen_imag, *v1, *v2, *v, *Q, *Q_1, *B, *QB, J, hmb, *SSSS;
    //  int *p;
    int i, index;
    double maxval, norm, btqb, *eigen_real3, *eigen_imag3, *Dz, *vdz, *SQ, *A_1, norm1, norm2, norm3;
    //  double x, y, z;

    // расчеты калибровочных матриц
    // allocate memory for matrix S
    S = (double*)memAlloc(10 * 10 * sizeof(double), "S array");
    Multiply_Self_Transpose(S, D, 10, msCalibrationMeasuresCnt_);

    // Create pre-inverted constraint matrix C
    C = (double*)memAlloc(6 * 6 * sizeof(double), "C array");
    C[0] = 0.0; C[1] = 0.5; C[2] = 0.5; C[3] = 0.0;  C[4] = 0.0;  C[5] = 0.0;
    C[6] = 0.5;  C[7] = 0.0; C[8] = 0.5; C[9] = 0.0;  C[10] = 0.0;  C[11] = 0.0;
    C[12] = 0.5;  C[13] = 0.5; C[14] = 0.0; C[15] = 0.0;  C[16] = 0.0;  C[17] = 0.0;
    C[18] = 0.0;  C[19] = 0.0;  C[20] = 0.0;  C[21] = -0.25; C[22] = 0.0;  C[23] = 0.0;
    C[24] = 0.0;  C[25] = 0.0; C[26] = 0.0;  C[27] = 0.0;  C[28] = -0.25; C[29] = 0.0;
    C[30] = 0.0;  C[31] = 0.0; C[32] = 0.0;  C[33] = 0.0;  C[34] = 0.0;  C[35] = -0.25;

    S11 = (double*)memAlloc(6 * 6 * sizeof(double), "S11");
    Get_Submatrix(S11, 6, 6, S, 10, 0, 0);
    S12 = (double*)memAlloc(6 * 4 * sizeof(double), "S12");
    Get_Submatrix(S12, 6, 4, S, 10, 0, 6);
    S12t = (double*)memAlloc(4 * 6 * sizeof(double), "S12t");
    Get_Submatrix(S12t, 4, 6, S, 10, 6, 0);
    S22 = (double*)memAlloc(4 * 4 * sizeof(double), "S22");
    Get_Submatrix(S22, 4, 4, S, 10, 6, 6);

    memFree(S);

    S22_1 = (double*)memAlloc(4 * 4 * sizeof(double), "S22_1");
    for(i = 0; i < 16; i++)
        S22_1[i] = S22[i];
    Choleski_LU_Decomposition(S22_1, 4);
    Choleski_LU_Inverse(S22_1, 4);

    memFree(S22);
    // Calculate S22a = S22_1 * S12t   4*6 = 4x4 * 4x6   C = AB
    S22a = (double*)memAlloc(4 * 6 * sizeof(double), "S22a");
    Multiply_Matrices(S22a, S22_1, 4, 4, S12t, 6);

    memFree(S22_1);
    memFree(S12t);

    // Then calculate S22b = S12 * S22a      ( 6x6 = 6x4 * 4x6)
    S22b = (double*)memAlloc(6 * 6 * sizeof(double), "S22b");
    Multiply_Matrices(S22b, S12, 6, 4, S22a, 6);

    memFree(S12);


    // Calculate SS = S11 - S22b
    SS = (double*)memAlloc(6 * 6 * sizeof(double), "SS");
    for(i = 0; i < 36; i++)
        SS[i] = S11[i] - S22b[i];

    memFree(S22b);
    memFree(S11);

    E = (double*)memAlloc(6 * 6 * sizeof(double), "E");
    Multiply_Matrices(E, C, 6, 6, SS, 6);

    memFree(SS);
    memFree(C);

    SSS = (double*)memAlloc(6 * 6 * sizeof(double), "SSS");
    Hessenberg_Form_Elementary(E, SSS, 6);

    eigen_real = (double*)memAlloc(6 * sizeof(double), "eigen_real");
    eigen_imag = (double*)memAlloc(6 * sizeof(double), "eigen_imag");

    QR_Hessenberg_Matrix(E, SSS, eigen_real, eigen_imag, 6, 100);

    memFree(E);
    memFree(eigen_imag);

    index = 0;
    maxval = eigen_real[0];
    for(i = 1; i < 6; i++)
    {
        if(eigen_real[i] > maxval)
        {
            maxval = eigen_real[i];
            index = i;
        }
    }

    memFree(eigen_real);

    v1 = (double*)memAlloc(6 * sizeof(double), "v1");

    v1[0] = SSS[index];
    v1[1] = SSS[index+6];
    v1[2] = SSS[index+12];
    v1[3] = SSS[index+18];
    v1[4] = SSS[index+24];
    v1[5] = SSS[index+30];

    memFree(SSS);

    // normalize v1
    norm = sqrt(v1[0] * v1[0] + v1[1] * v1[1] + v1[2] * v1[2] + v1[3] * v1[3] + v1[4] * v1[4] + v1[5] * v1[5]);
    v1[0] /= norm;
    v1[1] /= norm;
    v1[2] /= norm;
    v1[3] /= norm;
    v1[4] /= norm;
    v1[5] /= norm;

    if(v1[0] < 0.0)
    {
        v1[0] = -v1[0];
        v1[1] = -v1[1];
        v1[2] = -v1[2];
        v1[3] = -v1[3];
        v1[4] = -v1[4];
        v1[5] = -v1[5];
    }

    // Calculate v2 = S22a * v1      ( 4x1 = 4x6 * 6x1)
    v2 = (double*)memAlloc(4 * sizeof(double), "");
    Multiply_Matrices(v2, S22a, 4, 6, v1, 1);

    memFree(S22a);

    v = (double*)memAlloc(10 * sizeof(double), "");

    v[0] = v1[0];
    v[1] = v1[1];
    v[2] = v1[2];
    v[3] = v1[3];
    v[4] = v1[4];
    v[5] = v1[5];
    v[6] = -v2[0];
    v[7] = -v2[1];
    v[8] = -v2[2];
    v[9] = -v2[3];

    memFree(v1);
    memFree(v2);

    Q = (double*)memAlloc(3 * 3 * sizeof(double), "Q");

    Q[0] = v[0];
    Q[1] = v[5];
    Q[2] = v[4];
    Q[3] = v[5];
    Q[4] = v[1];
    Q[5] = v[3];
    Q[6] = v[4];
    Q[7] = v[3];
    Q[8] = v[2];

    U = (double*)memAlloc(3 * sizeof(double), "U");

    U[0] = v[6];
    U[1] = v[7];
    U[2] = v[8];


    Q_1 = (double*)memAlloc(3 * 3 * sizeof(double), "Q_1");
    for(i = 0; i < 9; i++)
        Q_1[i] = Q[i];
    Choleski_LU_Decomposition(Q_1, 3);
    Choleski_LU_Inverse(Q_1, 3);

    // Calculate B = Q-1 * U   ( 3x1 = 3x3 * 3x1)
    B = (double*)memAlloc(3 * sizeof(double), "B");
    Multiply_Matrices(B, Q_1, 3, 3, U, 1);
    B[0] = -B[0];     // x-axis combined bias
    B[1] = -B[1];     // y-axis combined bias
    B[2] = -B[2];     // z-axis combined bias


    memFree(Q_1);
    memFree(U);

    //!!!
    //for(i = 0; i < 3; i++)
    magCalibData->offset.x = B[0];
    magCalibData->offset.y = B[1];
    magCalibData->offset.z = B[2];
    //   ofs << B[i] << std::endl;
    //printf("%lf\radius\n", B[i]);

    // First calculate QB = Q * B   ( 3x1 = 3x3 * 3x1)
    QB = (double*)memAlloc(3 * sizeof(double), "QB");
    Multiply_Matrices(QB, Q, 3, 3, B, 1);

    // Then calculate btqb = BT * QB    ( 1x1 = 1x3 * 3x1)
    Multiply_Matrices(&btqb, B, 1, 3, QB, 1);

    // Calculate hmb = sqrt(btqb - J).
    J = v[9];
    hmb = sqrt(btqb - J);

    memFree(v);
    memFree(B);
    memFree(QB);

    // Calculate SQ, the square root of matrix Q
    SSSS = (double*)memAlloc(3 * 3 * sizeof(double), "SSSS");
    Hessenberg_Form_Elementary(Q, SSSS, 3);

    eigen_real3 = (double*)memAlloc(3 * sizeof(double), "eigen_real3");
    eigen_imag3 = (double*)memAlloc(3 * sizeof(double), "eigen_imag3");
    QR_Hessenberg_Matrix(Q, SSSS, eigen_real3, eigen_imag3, 3, 100);

    memFree(Q);
    memFree(eigen_imag3);

    // normalize eigenvectors
    norm1 = sqrt(SSSS[0] * SSSS[0] + SSSS[3] * SSSS[3] + SSSS[6] * SSSS[6]);
    SSSS[0] /= norm1;
    SSSS[3] /= norm1;
    SSSS[6] /= norm1;
    norm2 = sqrt(SSSS[1] * SSSS[1] + SSSS[4] * SSSS[4] + SSSS[7] * SSSS[7]);
    SSSS[1] /= norm2;
    SSSS[4] /= norm2;
    SSSS[7] /= norm2;
    norm3 = sqrt(SSSS[2] * SSSS[2] + SSSS[5] * SSSS[5] + SSSS[8] * SSSS[8]);
    SSSS[2] /= norm3;
    SSSS[5] /= norm3;
    SSSS[8] /= norm3;

    Dz = (double*)memAlloc(3 * 3 * sizeof(double), "Dz");
    for(i = 0; i < 9; i++)
        Dz[i] = 0.0;
    Dz[0] = sqrt(eigen_real3[0]);
    Dz[4] = sqrt(eigen_real3[1]);
    Dz[8] = sqrt(eigen_real3[2]);

    memFree(eigen_real3);

    vdz = (double*)memAlloc(3 * 3 * sizeof(double), "vdz");
    Multiply_Matrices(vdz, SSSS, 3, 3, Dz, 3);

    memFree(Dz);

    Transpose_Square_Matrix(SSSS, 3);

    SQ = (double*)memAlloc(3 * 3 * sizeof(double), "SQ");
    Multiply_Matrices(SQ, vdz, 3, 3, SSSS, 3);

    memFree(SSSS);

    // the method parameter now!    hm = 575.70; // mnorm
    A_1 = (double*)memAlloc(3 * 3 * sizeof(double), "A_1");

    for(i = 0; i < 9; i++)
    {
        if ( (i+1) % 3 == 0 )
            A_1[i] = SQ[i] * hmZ / hmb;
        else
            A_1[i] = SQ[i] * hmXY / hmb;
    }

//    0 = x0
//    1 = x1
//    2 = x2
//    3 = y0
//    4 = y1
//    5 = y2
//    6 = z0
//    7 = z1
//    8 = z2


    //printf("MagnetoCalibration.cpp:399 stage hmZ = %f hmXY = %f\n", hmZ, hmXY);

    for(i = 0; i < 3; i++)
    {
        magCalibData->scale[i].x = A_1[i + 0];
        magCalibData->scale[i].y = A_1[i + 3];
        magCalibData->scale[i].z = A_1[i + 6];

        ///!!!
        //   ofs << A_1[i*3] << std::endl;
        //   ofs << A_1[i*3+1] << std::endl;
        //   ofs << A_1[i*3+2] << std::endl;
        //printf("{ %lf, %lf, %lf },\radius\n", A_1[i*3], A_1[i*3+1], A_1[i*3+2]);
    }

    printf("new mag calib data: %f %f %f \n ",
            magCalibData->offset.x,
            magCalibData->offset.y,
            magCalibData->offset.z);

    for (int i = 0; i < 3; i++)
    {

        printf("  scale row %d: %f %f %f\n ",
                i,
                magCalibData->scale[i].x,
                magCalibData->scale[i].y,
                magCalibData->scale[i].z);
    }

    // освобождаем выделенную память

    memFree(vdz);
    memFree(SQ);
    memFree(A_1);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      MagnetoCalibration::addTillDone
// PURPOSE
//
// PARAMETERS
//      mag_report report --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL MagnetoCalibration::addTillDone(PSPATIALVECTOR     report)
{
    BOOL            result          = false;

    ENTER(true);

    D[mscMeasuresCnt_] = report->x * report->x;
    D[msCalibrationMeasuresCnt_+mscMeasuresCnt_] = report->y * report->y;
    D[msCalibrationMeasuresCnt_*2+mscMeasuresCnt_] = report->z * report->z;
    D[msCalibrationMeasuresCnt_*3+mscMeasuresCnt_] = 2.0 * report->y * report->z;
    D[msCalibrationMeasuresCnt_*4+mscMeasuresCnt_] = 2.0 * report->x * report->z;
    D[msCalibrationMeasuresCnt_*5+mscMeasuresCnt_] = 2.0 * report->x * report->y;
    D[msCalibrationMeasuresCnt_*6+mscMeasuresCnt_] = 2.0 * report->x;
    D[msCalibrationMeasuresCnt_*7+mscMeasuresCnt_] = 2.0 * report->y;
    D[msCalibrationMeasuresCnt_*8+mscMeasuresCnt_] = 2.0 * report->z;
    D[msCalibrationMeasuresCnt_*9+mscMeasuresCnt_] = 1.0;
    ++mscMeasuresCnt_;

    if ( mscMeasuresCnt_ == msCalibrationMeasuresCnt_ )
    {
        result = true;
    }

    RETURN(result);
}
PVOID  MagnetoCalibration::memAlloc(int            size,
                                    PCSTR          purpose)
{
    PVOID result = nil;

    if ((result = MALLOC(size)) != nil)
    {
        memCurrent += size;

        if (memPeak < memCurrent)
            memPeak = memCurrent;

        if (memLargest < size)
            memLargest = size;

        for (UINT i = 0; i < sizeof(mcbs) / sizeof(DBGMCB); i++)
        {
            if (nil == mcbs[i].ptr)
            {
                mcbs[i].ptr     = result;
                mcbs[i].purpose = purpose;
                mcbs[i].size    = (UINT)size;

                break;
            }
        }

        memNum++;

        if (memPeakNum < memNum)
            memPeakNum = memNum;
    }
    else
        printf("cannot allocate %d bytes of memory for %s!\n", size, purpose);

    return result;
}
PVOID MagnetoCalibration::memFree(PVOID mem)
{
    if (mem)
    {
        for (UINT i = 0; i < sizeof(mcbs) / sizeof(DBGMCB); i++)
        {
            if (mem == mcbs[i].ptr)
            {
                mcbs[i].ptr = nil;
                memCurrent -= mcbs[i].size;

                FREE(mem);
                break;
            }
        }

        memNum--;
    }

    return nil;
}
void MagnetoCalibration::memStat()
{
    printf("mem current/peak/largest/num: %d %d %d %d\n", memCurrent, memPeak, memLargest, memPeakNum);
}

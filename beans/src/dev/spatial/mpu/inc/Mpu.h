#ifndef ____MPU6XXX_H___
#define ____MPU6XXX_H___

/*
    The MPU-6050 is higher power, lower noise, and larger package versus
    the MPU-6500. Most of your code should port over, but some low power
    features are different and will need to be recoded in. Basic data
    acquisitions shouldn’t have changed.

    The MPU-9150 contains the MPU-6050 and an AK8975 magnetometer from AKM.
    The MPU-9250 contains a MPU-6500 and AK8963. The same differences between
    gyro/accel are the same you see with 6050 v. 6500. The magnetometer on
    the MPU-9250 is a little better across the board.
*/


#include <SpatialSensor.h>
#include <PortPin.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------



// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------


class Mpu: public SpatialSensor
{
protected:
        U8              chipId;

        int             _temp_initialized;
        double          _last_temp_c;

        float           _magScale               [ 3 ];

        UINT            _rateHz;

        HAXHANDLE       int1Handle;

        U8              _clockSource;

        struct
        {
            U8          enabledMag              : 1;
            U8          enableBw                : 1;
            U8          enableMasterI2c         : 1;
            U8          valueReset              : 1;
            U8          modeOverwr              : 1;
            U8          useAccCfg2              : 1;
            U8          accelReady              : 1;
            U8          gyroReady               : 1;
        } flags;


        BOOL            mpuInit                 (PORTPIN        ppInt);

        BOOL            resetMasterI2c          ();

        BOOL            fifoReset               ();

        INT             fifoGetCount            ();

        BOOL            fifoRead                (SPATIALVECTOR   &report);

        BOOL            probe                   ();

        BOOL            resetMag                ();

        BOOL            clearInterrupt          ();

static  void            intCb                   (Mpu *      that,
                                                 UINT           port,
                                                 UINT           pin);

virtual INT             registerWrite           (U8             reg_addr,
                                                 U8             data) = 0;

virtual INT             registerWrite           (U8             reg_addr,
                                                 PVOID          data,
                                                 INT            len) = 0;

virtual INT             registerRead            (U8             reg_addr,
                                                 PVOID          data,
                                                 INT            len) = 0;

        BOOL            readSensor              (UINT           startReg,
                                                 SPATIALVECTORI& report);


        BOOL            writeBits               (U8             regAddr,
                                                 U8             bitStart,
                                                 U8             length,
                                                 U8             data);

        BOOL            writeBit                (U8             regAddr,
                                                 U8             bitNum,
                                                 U8             data);

public:
        UINT            errors;

                        Mpu                     ();
                        ~Mpu                    ();


        BOOL            setRate                 (UINT           rateHz);

        BOOL            reset                   ();


        class Accel: public SpatialSensor::Accel
        {
                friend class    Mpu;

        protected:
                Mpu&        parent;

                            Accel               (Mpu &       x): SpatialSensor::Accel(x), parent(x)
                {};

                U8          getRangeValue       (UINT           rangeG);

        public:

                BOOL        setRange            (UINT           rangeG);

                BOOL        setRate             (UINT           rateHz);

                BOOL        init                ();

                BOOL        readRaw             (SPATIALVECTORI &   report);

                BOOL        read                (SPATIALVECTOR &    report);

                BOOL        isReady             ();

        } accel;

        class Gyro: public SpatialSensor::Gyro
        {
                friend class    Mpu;

        protected:
                Mpu&        parent;

                            Gyro                (Mpu &       x): SpatialSensor::Gyro(x), parent(x)
                {};

                U8          getRangeValue       (UINT           rangeDps);

        public:

                BOOL        setRange            (UINT           rangeDps);

                UINT        getRange            ();

                BOOL        setRate             (UINT           rateHz);

                BOOL        init                ();

                BOOL        readRaw             (SPATIALVECTORI &   report);

//                BOOL        readRaw2            (SPATIALVECTORI &   report);

                BOOL        read                (SPATIALVECTOR &    report);

                BOOL        isReady             ();

        } gyro;


        class Mag: public SpatialSensor::Mag
        {

                float       magCalibration      [ 3 ];
        protected:

                Mpu&        parent;

                            Mag                 (Mpu &       x): SpatialSensor::Mag(x), parent(x)
                {};

                U8          getRangeValue       (UINT           rangeDps);

                friend  class Mpu;

        public:

                BOOL        writeAK8963Register (U8             regNum,
                                                 U8             data);

                BOOL        readAK8963Registers (U8             regNum,
                                                 PVOID          buffer,
                                                 UINT           len);

                BOOL        init                ();

                BOOL        readRawDirect       (SPATIALVECTORI &   report);

                BOOL        readRaw             (SPATIALVECTORI &   report);

                double      getScale            (ORIAXIS            axis)
                { return parent._magScale[axis]; }

        } mag;



        class Fused: public SpatialSensor::Fused
        {
                long        quat[4];

        protected:
                Mpu&        parent;

                Fused                 (Mpu &       x): SpatialSensor::Fused(x), parent(x)
                {};
                friend  class Mpu;

#ifdef MPU_USE_DMP
                unsigned short fifoAvailable    (void);

                int         dmpEnableFeatures   (unsigned short mask);

                BOOL        readFifo            (SPATIALVECTOR & v);

                float       qToFloat            (long           number,
                                                 unsigned char  q);

                void        computeEulerAngles  (SPATIALVECTOR & v,
                                                 bool            degrees);

        public:
                INT         writeMpuRegisters   (U8             regNum,
                                                 PVOID          buffer,
                                                 UINT           len)
                { return parent.registerWrite(regNum, buffer, len); }

                INT         readMpuRegisters    (U8             regNum,
                                                 PVOID          buffer,
                                                 UINT           len)
                { return parent.registerRead(regNum, buffer, len); }

                BOOL        init                ();

                BOOL        read                    (SPATIALVECTOR  &report);

//                BOOL        readRawDirect       (SPATIALVECTORI &   report);
//
//                BOOL        readRaw             (SPATIALVECTORI &   report);
//
//                double      getScale            (ORIAXIS            axis)
//                { return parent._magScale[axis]; }
#endif // #ifdef MPU_USE_DMP


        } fused;


        BOOL            readBulk                (SPATIALVECTORI& accl,
                                                 SPATIALVECTORI& gyro);

        BOOL            readBulk                (SPATIALVECTORI& accl,
                                                 SPATIALVECTORI& gyro,
                                                 SPATIALVECTORI& mag,
                                                 float &         temp);

        void            setDLPFMode             (U8             mode);

        void            setClockSource          (U8             source);

        void            enableSleep             (BOOL           ena);

        void            enableReadyInt          (BOOL           ena);

        BOOL            enableFifo              (BOOL           ena);


        BOOL            fastCalibration         (PSPATIALVECTORI gyroOffsets,
                                                 PSPATIALVECTORI acclOffsets);

        BOOL            fastMagCalibration      (INT *          magBias,
                                                 float *        magScale);
};

#endif // ____MPU6XXX_H___
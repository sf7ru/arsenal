

#include <Mpu.h>
#include <Mpu_pvt.h>

#include <string.h>
#include <math.h>

#include <axtime.h>
#include <Hal.h>

#include <stdio.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

Mpu::Mpu() : accel(*this), gyro(*this), mag(*this), fused(*this)
{
    chipId                  = 0;
    _temp_initialized       = 0;
    _last_temp_c            = 0;
    _clockSource            = MPU6050_CLOCK_PLL_XGYRO;

    int1Handle              = nil;

    errors                  = 0;

    flags.enableBw          = (U8)false;
    flags.enabledMag        = (U8)false;
    flags.valueReset        = (U8)false;
    flags.modeOverwr        = (U8)false;
    flags.enableMasterI2c   = (U8)false;
    flags.useAccCfg2        = (U8)false;
    flags.accelReady        = (U8)false;
    flags.gyroReady         = (U8)false;
}
Mpu::~Mpu()
{

}
// ***************************************************************************
// FUNCTION
//      I2cMpu::probe
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL Mpu::probe()
{
#define MAC_WHO(a,b) case MPU_WHOAMI_##a: who = #a; result = true; b; break
    BOOL            result          = false;
    U8              chipId;
    PCSTR           who             = nil;

    ENTER(true);

    if (registerRead(MPUREG_WHOAMI, &chipId, 1) > 0)
    {
        switch (chipId)
        {
            MAC_WHO(6050,
                    flags.valueReset = true;);

            MAC_WHO(6500,
                    flags.useAccCfg2    = true;
                    flags.modeOverwr    = true);

            MAC_WHO(9250,
                    flags.useAccCfg2    = true;
                    flags.modeOverwr    = true;
                    flags.enabledMag    = true;);

            MAC_WHO(9255,
                    flags.useAccCfg2    = true;
                    flags.modeOverwr    = true;
                    flags.enabledMag    = true;);

            default:
                break;
        }

        if (who)
        {
            printf("MPU: found %s\n", who);
        }
        else
            printf("MPU: unknown chip id %X\n", chipId);
    }
    else
        printf("MPU: cannot read register\n");

    RETURN(result);
}
BOOL Mpu::readSensor(UINT            startReg,
                     SPATIALVECTORI& report)
{
    BOOL            result          = false;
    sensor_packet_t packet;

    ENTER(true);

    //printf(">> Mpu::readSensor\n");

    switch (registerRead(startReg, &packet, sizeof(packet)))
    {
        case 0:
        case -1:
            errors++;
            break;

        default:
            errors      = 0;
            report.x    = (I16)MAC_BE_U16(packet.x);
            report.y    = (I16)MAC_BE_U16(packet.y);
            report.z    = (I16)MAC_BE_U16(packet.z);
            result      = true;
            break;
    }

    //printf("<< Mpu::readSensor\n");

    RETURN(result);
}
BOOL Mpu::readBulk(SPATIALVECTORI& accl,
                   SPATIALVECTORI& gyro)

{
    BOOL                result          = false;
    sensor_packet2_t    packet;

    ENTER(true);

    if (!int1Handle || (flags.gyroReady && flags.accelReady))
    {
        switch (registerRead(MPUREG_ACCEL_XOUT_H, &packet, sizeof(packet)))
        {
            case 0:
            case -1:
                errors++;
                break;

            default:
                errors      = 0;
                accl.x      = (I16)MAC_BE_U16(packet.accl.x);
                accl.y      = (I16)MAC_BE_U16(packet.accl.y);
                accl.z      = (I16)MAC_BE_U16(packet.accl.z);
                gyro.x      = (I16)MAC_BE_U16(packet.gyro.x);
                gyro.y      = (I16)MAC_BE_U16(packet.gyro.y);
                gyro.z      = (I16)MAC_BE_U16(packet.gyro.z);
                result      = true;

                flags.accelReady    = 0;
                flags.gyroReady     = 0;
                break;
        }
    }

    RETURN(result);
}
BOOL Mpu::readBulk(SPATIALVECTORI& accl,
                   SPATIALVECTORI& gyro,
                   SPATIALVECTORI& mag,
                   float &        temp)
{
    BOOL                result          = false;
    sensor_packet3_t packet;

    ENTER(true);

    if (!int1Handle || (flags.gyroReady && flags.accelReady))
    {
        switch (registerRead(MPUREG_ACCEL_XOUT_H, &packet, sizeof(packet)))
        {
            case 0:
            case -1:
                errors++;
                break;

            default:
                errors      = 0;
                accl.x      = (I16)MAC_BE_U16(packet.accl.x);
                accl.y      = (I16)MAC_BE_U16(packet.accl.y);
                accl.z      = (I16)MAC_BE_U16(packet.accl.z);
                gyro.x      = (I16)MAC_BE_U16(packet.gyro.x);
                gyro.y      = (I16)MAC_BE_U16(packet.gyro.y);
                gyro.z      = (I16)MAC_BE_U16(packet.gyro.z);

                if (!(packet.st2 & 0x08))
                {
                    mag.x   = (I16) MAC_LE_U16(packet.mag.x);
                    mag.y   = (I16) MAC_LE_U16(packet.mag.y);
                    mag.z   = (I16) MAC_LE_U16(packet.mag.z);
                }
                else
                {
                    mag.x   = 0;
                    mag.y   = 0;
                    mag.z   = 0;
                }

                // Conversions are straight out of the datahsheet
                // TEMP_degC   = ((TEMP_OUT - RoomTemp_Offset) / Temp_Sensitivity) + 21
                temp = ((MAC_BE_U16(packet.temp) - 21) / 333.87) + 21;

                result              = true;

                flags.accelReady    = 0;
                flags.gyroReady     = 0;


                break;
        }
    }

    RETURN(result);
}
void Mpu::intCb(Mpu *    that,
                    UINT           port,
                    UINT           pin)
{
    that->flags.gyroReady   = true;
    that->flags.accelReady  = true;
}
BOOL Mpu::writeBits(U8             regAddr,
                    U8             bitStart,
                    U8             length,
                    U8             data)
{
    BOOL result = false;

    U8 b;

    if (registerRead(regAddr, &b, 1) != 0)
    {
        U8 mask = (U8)(((1 << length) - 1) << (bitStart - length + 1));
        data <<= (bitStart - length + 1); // shift data into correct position
        data &= mask; // zero all non-important bits in data
        b &= ~(mask); // zero all important bits in existing byte
        b |= data; // combine data with existing byte
        result = registerWrite(regAddr, b) > 1;
    }

    return result;
}
BOOL Mpu::writeBit(U8 regAddr, U8 bitNum, U8 data)
{
    U8 b;
    registerRead(regAddr, &b, 1);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    return registerWrite(regAddr, b) > 0;
}
void Mpu::setDLPFMode(U8             mode)
{
    writeBits(MPUREG_CONFIG, MPU6050_CFG_DLPF_CFG_BIT, MPU6050_CFG_DLPF_CFG_LENGTH, mode);
}
void Mpu::setClockSource(U8 source)
{
    if (_clockSource != source)
    {
        _clockSource = source;
        writeBits(MPUREG_PWR_MGMT_1, MPU6050_PWR1_CLKSEL_BIT, MPU6050_PWR1_CLKSEL_LENGTH, source);
    }
}
BOOL Mpu::setRate(UINT rateHz)
{
    U8 period = (U8)(1000 / rateHz);

    _rateHz = rateHz;

    if (period > 0)
        period--;

    return registerWrite(MPUREG_SMPLRT_DIV, period) > 0;
}
void Mpu::enableSleep(BOOL ena)
{
    writeBit(MPUREG_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, (U8)ena);
}
void Mpu::enableReadyInt(BOOL           ena)
{
    if (ena)
    {
        registerWrite(MPUREG_INT_PIN_CFG, (U8)
                                          (MPU6500_INT_PIN_CFG_LVL_HIGH << MPU6500_INT_PIN_CFG_LVL) |
                                          (MPU6500_INT_PIN_CFG_OPEN_PP << MPU6500_INT_PIN_CFG_OPEN) |
                                          (MPU6500_INT_PIN_CFG_LATCH_PULS << MPU6500_INT_PIN_CFG_LATCH) |
                                          (MPU6500_INT_PIN_CFG_RDCLR_ANY << MPU6500_INT_PIN_CFG_RDCLR) |
                                          (MPU6500_INT_PIN_CFG_FSYNC_LOW << MPU6500_INT_PIN_CFG_FSYNC) |
                                          (MPU6500_INT_PIN_CFG_FSYNCM_NONE << MPU6500_INT_PIN_CFG_FSYNCM));
    }

    writeBit(MPUREG_INT_ENABLE, MPU6050_INT_DATA_RDY_BIT, (U8)(ena ? 1 : 0)) ;
}
BOOL Mpu::resetMag()
{
    BOOL            result          = false;

    ENTER(true);

//    // Initialize the magnetometer inside the IMU, if enabled by the caller.
//    if (_mag_enabled && _mag == nullptr) {
//        if ((_mag = new MPU9250_mag(*this, MPU9250_MAG_SAMPLE_RATE_100HZ))
//            != nullptr) {
//            // Initialize the magnetometer, providing the output data rate for
//            // data read from the IMU FIFO.  This is used to calculate the I2C
//            // delay for reading the magnetometer.
//            result = _mag->initialize(MPU9250_MEASURE_INTERVAL_US);
//
//            if (result != 0) {
//                DF_LOG_ERR("Magnetometer initialization failed");
//            }
//
//        } else {
//            DF_LOG_ERR("Allocation of magnetometer object failed.");
//        }
//    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      I2cMpu::clearInterrupt
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL Mpu::clearInterrupt()
{
    BOOL            result          = false;
    U8              int_status      = 0;

    ENTER(true);

    if (registerRead(MPUREG_INT_STATUS, &int_status, 1) > 0)
    {
        result = true;
    }

    RETURN(result);
}
BOOL Mpu::mpuInit(PORTPIN ppInt)
{
    BOOL        result          = false;

    ENTER(true);

    if (probe())
    {
        reset();

        if (PP_IS_DEFINED(ppInt))
        {
            Hal &hal = Hal::getInstance();
//            hal.gpio.setMode(ppInt);
            if ((int1Handle = hal.gpioint.setVector(ppInt, (PFNGPIOINTCB) &Mpu::intCb, this,
                                                    SETVECTOR_EDGE_RISE)) != nil)
            {
                result = true;
            }
        }
        else
            result = true;

        if (result && flags.enabledMag)
        {
            result= mag.init();
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      I2cMpu::resetBase
// PURPOSE
//
// PARAMETERS
//      U8 value0 --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL Mpu::reset()
{
    BOOL            result          = false;

    ENTER(true);

    if (registerWrite(MPUREG_PWR_MGMT_1, BIT_H_RESET) > 0)
    {
        axudelay(100 * MPUDELAY_INIT);

        if (registerWrite(MPUREG_PWR_MGMT_1, flags.valueReset ? 0 : MPU6050_CLOCK_PLL_ZGYRO) > 0)
        {
            axudelay(MPUDELAY_INIT);

            // Don't set sensors into standby mode
            if (registerWrite(MPUREG_PWR_MGMT_2, 0) > 0)
            {
                axudelay(MPUDELAY_INIT);

                result = true;
            }
        }
    }

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      I2cMpu::resetMasterI2c
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL Mpu::resetMasterI2c()
{
    BOOL            result          = false;

    ENTER(true);

    // Reset I2C master and device
    if (registerWrite(MPUREG_USER_CTRL,
                      BITS_USER_CTRL_I2C_MST_RST |
                      BITS_USER_CTRL_I2C_IF_DIS) > 0)
    {
        axudelay(MPUDELAY_INIT);

        result = true;
    }

    RETURN(result);
}
U8 Mpu::Gyro::getRangeValue(UINT           rangeDps)
{
    U8          result;

    ENTER(true);

    if (rangeDps > 250)
    {
        if (rangeDps > 500)
        {
            if (rangeDps > 1000)
            {
                scale  = 2000.0;
                result = MPU6050_GYRO_FS_2000;
            }
            else
            {
                scale  = 1000.0;
                result = MPU6050_GYRO_FS_1000;
            }
        }
        else
        {
            scale  = 500.0;
            result = MPU6050_GYRO_FS_500;
        }
    }
    else
    {
        scale  = 250.0;
        result = MPU6050_GYRO_FS_250;
    }

    scale  = scale / (65536 / 2);

    RETURN(result);
}

BOOL Mpu::Gyro::setRange(UINT           rangeDps)
{// setFullScaleGyroRange();
    return parent.writeBits(MPUREG_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, getRangeValue(rangeDps));
}
UINT Mpu::Gyro::getRange()
{
    UINT result = 0;
    U8 r;

    if (parent.registerRead(MPUREG_GYRO_CONFIG, &r, 1) > 0)
    {
        result = r;
    }
//    = registerRead(MPUREG_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, getRangeValue(rangeDps));


    return result;
}
BOOL Mpu::Gyro::setRate(UINT           rateHz)
{
    return parent.setRate(rateHz);
}
BOOL Mpu::Gyro::init()
{
    return true;
}
BOOL Mpu::Gyro::readRaw(SPATIALVECTORI &   report)
{
    BOOL            result          = false;

    ENTER(true);

    if (parent.int1Handle)
    {
        int TO = 2000 / parent._rateHz;

        while (!parent.flags.gyroReady && TO--)
        {
            axsleep(1);
        }

        if (!parent.flags.gyroReady)
        {
            errors++;
        }
    }

    if (parent.flags.gyroReady)
    {
        result = parent.readSensor(MPUREG_GYRO_XOUT_H, report);

        parent.flags.gyroReady = 0;

        if (!result)
        {
            errors++;
        }
    }

    RETURN(result);
}
//BOOL Mpu::Gyro::readRaw2(SPATIALVECTORI &   report)
//{
//    BOOL            result          = false;
//
//    ENTER(true);
//
//    if (parent.int1Handle)
//    {
//        int TO = 2000 / parent._rateHz;
//
//        while (!parent.flags.gyroReady && TO--)
//        {
//            axsleep(1);
//        }
//
//        if (!parent.flags.gyroReady)
//        {
//            errors++;
//        }
//    }
//
//    if (parent.flags.gyroReady)
//    {
//        result = parent.readSensor(MPUREG_GYRO_XOUT_H, report);
//
//        parent.flags.gyroReady = 0;
//
//        if (!result)
//        {
//            errors++;
//        }
//    }
//
//    RETURN(result);
//}
BOOL Mpu::Gyro::read(SPATIALVECTOR &    report)
{
    return false;
}
BOOL Mpu::Gyro::isReady()
{
    return parent.int1Handle ? parent.flags.gyroReady : true;
}
U8 Mpu::Accel::getRangeValue(UINT rangeG)
{
    U8          result          = 0;

    ENTER(true);

    if (rangeG > 2)
    {
        if (rangeG > 4)
        {
            if (rangeG > 8)
            {
                scale   = 16;
                result  = MPU6050_ACCEL_FS_16;
            }
            else
            {
                scale   = 8;
                result  = MPU6050_ACCEL_FS_8;
            }

        }
        else
        {
            scale   = 4;
            result  = MPU6050_ACCEL_FS_4;
        }
    }
    else
    {
        scale   = 2;
        result  = MPU6050_ACCEL_FS_2;
    }

    scale = scale / (65536 / 2);

    RETURN(result);
}
BOOL Mpu::Accel::setRange            (UINT           rangeG)
{
    return parent.writeBits(MPUREG_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, getRangeValue(rangeG));
}
BOOL Mpu::Accel::setRate(UINT           rateHz)
{
    return parent.setRate(rateHz);
}
BOOL Mpu::Accel::init()
{
    return true;
}
BOOL Mpu::Accel::readRaw(SPATIALVECTORI &   report)
{
    BOOL            result          = false;

    ENTER(true);

    if (!parent.int1Handle || parent.flags.accelReady)
    {
        result = parent.readSensor(MPUREG_ACCEL_XOUT_H, report);

        parent.flags.accelReady = 0;
    }

    RETURN(result);
}
BOOL Mpu::Accel::read(SPATIALVECTOR &    report)
{
    return false;
}
BOOL Mpu::Accel::isReady()
{

    return parent.int1Handle ? parent.flags.accelReady : true;
}
BOOL Mpu::Mag::readRaw(SPATIALVECTORI & report)
{
    BOOL                result          = false;
    magnet_packet_t     packet;

    ENTER(true);

    parent.registerRead(MPUREG_EXT_SENS_DATA_00, &packet, sizeof(packet));        // Read the x-, y-, and z-axis calibration values

    // Check if magnetic sensor overflow set, if not then report data
    if (!(packet.st2 & 0x08))
    {
        report.x    = (I16)MAC_LE_U16(packet.x);
        report.y    = (I16)MAC_LE_U16(packet.y);
        report.z    = (I16)MAC_LE_U16(packet.z);

        result      = true;
    }

    RETURN(result);
}
BOOL Mpu::Mag::readRawDirect(SPATIALVECTORI &report)
{
    BOOL                result          = false;
    magnet_packet_t     packet;

    ENTER(true);

    parent.registerRead(MPUREG_EXT_SENS_DATA_00, &packet, sizeof(packet));        // Read the x-, y-, and z-axis calibration values

    // Check if magnetic sensor overflow set, if not then report data
    if (!(packet.st2 & 0x08))
    {
        report.x    = (I16)MAC_LE_U16(packet.x);
        report.y    = (I16)MAC_LE_U16(packet.y);
        report.z    = (I16)MAC_LE_U16(packet.z);

        result      = true;
    }

    RETURN(result);
}
BOOL Mpu::Mag::writeAK8963Register(U8 regNum, U8 data)
{
    BOOL        result;

    ENTER(true);

    result = (parent.registerWrite(MPUREG_I2C_SLV0_ADDR, AK8963_ADDRESS) > 0) &&    // Set the I2C slave address of AK8963 and set for write.
             (parent.registerWrite(MPUREG_I2C_SLV0_REG, regNum) > 0)          &&    // I2C slave 0 register address from where to begin data transfer
             (parent.registerWrite(MPUREG_I2C_SLV0_DO, data) > 0)             &&
             (parent.registerWrite(MPUREG_I2C_SLV0_CTRL, BITS_I2C_SLV0_EN | 1) > 0)           ;   // Enable I2C and write 1 byte

    RETURN(result);
}
BOOL Mpu::Mag::readAK8963Registers (U8             regNum,
                                    PVOID          buffer,
                                    UINT           len)
{
    BOOL        result = true;

    ENTER(true);

    parent.registerWrite(MPUREG_I2C_SLV0_ADDR, AK8963_ADDRESS | I2C_READ_FLAG);           // Set the I2C slave address of AK8963 and set for read.
    parent.registerWrite(MPUREG_I2C_SLV0_REG, regNum);                                    // I2C slave 0 register address from where to begin data transfer
    parent.registerWrite(MPUREG_I2C_SLV0_CTRL, BITS_I2C_SLV0_EN | len);                   // Enable I2C and read N bytes
    axsleep(1);
    parent.registerRead(MPUREG_EXT_SENS_DATA_00,buffer, len);

    RETURN(result);
}
BOOL Mpu::Mag::init()
{
    BOOL        result          = false;
    U8          _buffer         [ 7 ];

    ENTER(true);

    // First extract the factory calibration for each magnetometer axis
    U8 rawData[3];  // x/y/z gyro calibration data stored here

    // enable I2C master mode
    parent.registerWrite(MPUREG_USER_CTRL, MPU6050_USER_CTRL_I2C_MST_EN);

    // set the I2C bus speed to 400 kHz
    parent.registerWrite(MPUREG_I2C_MST_CTRL,BITS_I2C_MST_CLK_400_KHZ);

    readAK8963Registers(WHO_AM_I_AK8963, _buffer, 1);

    if (_buffer[0] == WHO_AM_I_AK8963_VAL)
    {
        // set AK8963 to Power Down
        writeAK8963Register(AK8963_CNTL, AK8963_POWER_DOWN);
        axdelay(100);

        // reset the AK8963
        writeAK8963Register(AK8963_CNTL2, AK8963_RESET);
        axdelay(100);

        // set AK8963 to FUSE ROM access
        writeAK8963Register(AK8963_CNTL, AK8963_FUSE_ROM);
        axdelay(100); // long wait between AK8963 mode changes

        readAK8963Registers(AK8963_ASAX, _buffer, 3);
        parent._magScale[0] = (((((float) _buffer[0]) - 128.0f) / 256.0f) + 1.0f) ; // micro Tesla
        parent._magScale[1] = (((((float) _buffer[1]) - 128.0f) / 256.0f) + 1.0f) ; // micro Tesla
        parent._magScale[2] = (((((float) _buffer[2]) - 128.0f) / 256.0f) + 1.0f) ; // micro Tesla

        // set AK8963 to Power Down
        writeAK8963Register(AK8963_CNTL, AK8963_POWER_DOWN);
        axdelay(100);

        // set AK8963 to 16 bit resolution, 100 Hz update rate
        writeAK8963Register(AK8963_CNTL, AK8963_CNT_MEAS2);
        axdelay(100); // long wait between AK8963 mode changes

        scale  = 10 ; // Proper scale to return milliGauss (from microtesla)

        parent.setClockSource(parent._clockSource);

        // instruct the MPU9250 to get 7 bytes of data from the AK8963 at the sample rate
        readAK8963Registers(AK8963_XOUT_L, _buffer, sizeof(_buffer));

        result = true;
    }

    RETURN(result);
}
BOOL Mpu::fastCalibration(PSPATIALVECTORI gyroOffsets,
                          PSPATIALVECTORI acclOffsets)

{
    BOOL        result          = false;

    SPATIALVECTORI      gyro;
    SPATIALVECTORI      accl;
    SPATIALVECTORI      mag;
    float               temp;

    INT                 totalCount      = 0;

    INT                 gyroAccumX      = 0;
    INT                 gyroAccumY      = 0;
    INT                 gyroAccumZ      = 0;

    INT                 acclAccumX      = 0;
    INT                 acclAccumY      = 0;
    INT                 acclAccumZ      = 0;

//    INT                 magAccumX      = 0;
//    INT                 acclAccumY      = 0;
//    INT                 acclAccumZ      = 0;

    ENTER(true);

    for (int i = 0; i < 4000; i++)
    {
//        if (readBulk(accl, gyro, mag, temp))
        if (readBulk(accl, gyro))
        {
            gyroAccumX  += gyro.x;
            gyroAccumY  += gyro.y;
            gyroAccumZ  += gyro.z;

            acclAccumX  += accl.x;
            acclAccumY  += accl.y;
            acclAccumZ  += accl.z;

            totalCount++;
        }

        axsleep(1);
    }

    if (gyroOffsets)
    {
        gyroOffsets->x  = gyroAccumX / totalCount;
        gyroOffsets->y = gyroAccumY / totalCount;
        gyroOffsets->z   = gyroAccumZ / totalCount;
    }

    if (acclOffsets)
    {
        int accelsensitivity = 16384;  // = 16384 LSB/g

        acclOffsets->x  = acclAccumX / totalCount;
        acclOffsets->y = acclAccumY / totalCount;
        // Remove gravity from the z-axis accelerometer bias calculation
        acclOffsets->z   = (acclAccumZ / totalCount) - (acclAccumZ > 0 ? accelsensitivity : -accelsensitivity );

        printf("ACCL X: %d, Y: %d, Z: %d\n", acclOffsets->x, acclOffsets->y, acclOffsets->z);
    }

    RETURN(result);
}
BOOL Mpu::fastMagCalibration(INT *          magBias,
                             float *        magScale)
{
    BOOL            result          = false;
    SPATIALVECTORI  vec;
    U8              rawData         [ 3 ];
    float           magCalibration  [ 3 ];

    ENTER(true);

    UINT ii = 0, sample_count = 0;
    int32_t mag_bias[3] = {0, 0, 0}, mag_scale[3] = {0, 0, 0};
    int16_t mag_max[3] = {-32767, -32767, -32767}, mag_min[3] = {32767, 32767, 32767}, mag_temp[3] = {0, 0, 0};

//    mag.readAK8963Registers(AK8963_ASAX, rawData, sizeof(rawData));// Read the x-, y-, and z-axis calibration values
//    magCalibration[0] =  (float)(rawData[0] - 128)/256.0f + 1.0f;   // Return x-axis sensitivity adjustment values, etc.
//    magCalibration[1] =  (float)(rawData[1] - 128)/256.0f + 1.0f;
//    magCalibration[2] =  (float)(rawData[2] - 128)/256.0f + 1.0f;
//    _magCalibration[0] = magCalibration[0];
//    _magCalibration[1] = magCalibration[1];
//    _magCalibration[2] = magCalibration[2];


    sample_count = 2500;  // at 100 Hz ODR, new mag data is available every 10 ms

    for(ii = 0; ii < sample_count; ii++)
    {
        mag.readRaw(vec);  // Read the mag data
        mag_temp[0] = vec.x;
        mag_temp[1] = vec.y;
        mag_temp[2] = vec.z;

        if (mag_temp)
        for (int jj = 0; jj < 3; jj++)
        {
            if(mag_temp[jj] > mag_max[jj]) mag_max[jj] = mag_temp[jj];
            if(mag_temp[jj] < mag_min[jj]) mag_min[jj] = mag_temp[jj];
        }
        axsleep(10);  // at 100 Hz ODR, new mag data is available every 10 ms
    }

//    Serial.println("mag x min/max:"); Serial.println(mag_max[0]); Serial.println(mag_min[0]);
//    Serial.println("mag y min/max:"); Serial.println(mag_max[1]); Serial.println(mag_min[1]);
//    Serial.println("mag z min/max:"); Serial.println(mag_max[2]); Serial.println(mag_min[2]);

    // Get hard iron correction
    mag_bias[0]  = (mag_max[0] + mag_min[0])/2;  // get average x mag bias in counts
    mag_bias[1]  = (mag_max[1] + mag_min[1])/2;  // get average y mag bias in counts
    mag_bias[2]  = (mag_max[2] + mag_min[2])/2;  // get average z mag bias in counts

    //float _mRes = 10.*4912./32760.0; // Proper scale to return milliGauss

    magBias[0] = mag_bias[0]; // *_mRes*_magCalibration[0];  // save mag biases in G for main program
    magBias[1] = mag_bias[1]; // *_mRes*_magCalibration[1];
    magBias[2] = mag_bias[2]; // *_mRes*_magCalibration[2];

    // Get soft iron correction estimate
    mag_scale[0]  = (mag_max[0] - mag_min[0])/2;  // get average x axis max chord length in counts
    mag_scale[1]  = (mag_max[1] - mag_min[1])/2;  // get average y axis max chord length in counts
    mag_scale[2]  = (mag_max[2] - mag_min[2])/2;  // get average z axis max chord length in counts

    float avg_rad = mag_scale[0] + mag_scale[1] + mag_scale[2];
    avg_rad /= 3.0;

    magScale[0] = avg_rad/((float)mag_scale[0]);
    magScale[1] = avg_rad/((float)mag_scale[1]);
    magScale[2] = avg_rad/((float)mag_scale[2]);

    RETURN(result);
}

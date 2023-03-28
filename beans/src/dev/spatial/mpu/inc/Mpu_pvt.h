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

#ifndef ____MPU6XXX_PVT_H__
#define ____MPU6XXX_PVT_H__

#include <arsenal.h>

// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define MPUDELAY_INIT       1000


#define DEFAULT_I2C_ADDR_MPU6XXX    0x68

#define MPU_WHOAMI_6050             0x68
#define MPU_WHOAMI_6500             0x70
#define MPU_WHOAMI_9250             0x71
#define MPU_WHOAMI_9255             0x73

//#define I2C_MST_CTRL     0x24
//#define I2C_SLV0_ADDR    0x25
//#define I2C_SLV0_REG     0x26
//#define I2C_SLV0_CTRL    0x27

#define MPUREG_WHOAMI           0x75
#define MPUREG_SMPLRT_DIV       0x19
#define MPUREG_CONFIG           0x1A
#define MPUREG_GYRO_CONFIG      0x1B
#define MPUREG_ACCEL_CONFIG     0x1C
#define MPUREG_ACCEL_CONFIG2    0x1D
#define MPUREG_LPACCEL_ODR      0x1E
#define MPUREG_WOM_THRESH       0x1F
#define MPUREG_FIFO_EN          0x23
#define MPUREG_I2C_MST_CTRL     0x24
#define MPUREG_I2C_SLV0_ADDR    0x25
#define MPUREG_I2C_SLV0_REG     0x26
#define MPUREG_I2C_SLV0_CTRL    0x27
#define MPUREG_I2C_SLV1_ADDR    0x28
#define MPUREG_I2C_SLV1_REG     0x29
#define MPUREG_I2C_SLV1_CTRL    0x2A
#define MPUREG_I2C_SLV2_ADDR    0x2B
#define MPUREG_I2C_SLV2_REG     0x2C
#define MPUREG_I2C_SLV2_CTRL    0x2D
#define MPUREG_I2C_SLV3_ADDR    0x2E
#define MPUREG_I2C_SLV3_REG     0x2F
#define MPUREG_I2C_SLV3_CTRL    0x30
#define MPUREG_I2C_SLV4_ADDR    0x31
#define MPUREG_I2C_SLV4_REG     0x32
#define MPUREG_I2C_SLV4_DO      0x33
#define MPUREG_I2C_SLV4_CTRL    0x34
#define MPUREG_I2C_SLV4_DI      0x35
#define MPUREG_I2C_MST_STATUS   0x36
#define MPUREG_INT_PIN_CFG      0x37
#define MPUREG_INT_ENABLE       0x38
#define MPUREG_INT_STATUS       0x3A
#define MPUREG_ACCEL_XOUT_H     0x3B
#define MPUREG_ACCEL_XOUT_L     0x3C
#define MPUREG_ACCEL_YOUT_H     0x3D
#define MPUREG_ACCEL_YOUT_L     0x3E
#define MPUREG_ACCEL_ZOUT_H     0x3F
#define MPUREG_ACCEL_ZOUT_L     0x40
#define MPUREG_TEMP_OUT_H       0x41
#define MPUREG_TEMP_OUT_L       0x42
#define MPUREG_GYRO_XOUT_H      0x43
#define MPUREG_GYRO_XOUT_L      0x44
#define MPUREG_GYRO_YOUT_H      0x45
#define MPUREG_GYRO_YOUT_L      0x46
#define MPUREG_GYRO_ZOUT_H      0x47
#define MPUREG_GYRO_ZOUT_L      0x48
#define MPUREG_EXT_SENS_DATA_00 0x49
#define MPUREG_EXT_SENS_DATA_01 0x4A
#define MPUREG_EXT_SENS_DATA_02 0x4B
#define MPUREG_EXT_SENS_DATA_03 0x4C
#define MPUREG_EXT_SENS_DATA_04 0x4D
#define MPUREG_EXT_SENS_DATA_05 0x4E
#define MPUREG_EXT_SENS_DATA_06 0x4F
#define MPUREG_EXT_SENS_DATA_07 0x50
#define MPUREG_EXT_SENS_DATA_08 0x51
#define MPUREG_EXT_SENS_DATA_09 0x52
#define MPUREG_EXT_SENS_DATA_10 0x53
#define MPUREG_EXT_SENS_DATA_11 0x54
#define MPUREG_EXT_SENS_DATA_12 0x55
#define MPUREG_EXT_SENS_DATA_13 0x56
#define MPUREG_EXT_SENS_DATA_14 0x57
#define MPUREG_EXT_SENS_DATA_15 0x58
#define MPUREG_EXT_SENS_DATA_16 0x59
#define MPUREG_EXT_SENS_DATA_17 0x5A
#define MPUREG_EXT_SENS_DATA_18 0x5B
#define MPUREG_EXT_SENS_DATA_19 0x5C
#define MPUREG_EXT_SENS_DATA_20 0x5D
#define MPUREG_EXT_SENS_DATA_21 0x5E
#define MPUREG_EXT_SENS_DATA_22 0x5F
#define MPUREG_EXT_SENS_DATA_23 0x60
#define MPUREG_MOT_DETECT_STATUS    0x61
#define MPUREG________________1 0x62
#define MPUREG_I2C_SLV0_DO      0x63
#define MPUREG_I2C_SLV1_DO      0x64
#define MPUREG_I2C_SLV2_DO      0x65
#define MPUREG_I2C_SLV3_DO      0x66
#define MPUREG_I2C_MST_DELAY_CTRL   0x67
#define MPUREG_SIGNAL_PATH_RESET    0x68
#define MPUREG_MOT_DETECT_CTRL  0x69
#define MPUREG_USER_CTRL        0x6A
#define MPUREG_PWR_MGMT_1       0x6B
#define MPUREG_PWR_MGMT_2       0x6C
#define MPUREG_FIFO_COUNTH      0x72
#define MPUREG_FIFO_COUNTL      0x73
#define MPUREG_FIFO_R_W         0x74

// Configuration bits MPU 6050
#define BIT_SLEEP           0x40
#define BIT_H_RESET         0x80
#define BITS_CLKSEL         0x07
#define MPU_EXT_SYNC_GYROX      0x02
#define MPU6050_GYRO_ST_X          0x80
#define MPU6050_GYRO_ST_Y          0x40
#define MPU6050_GYRO_ST_Z          0x20
#define MPU6050_FS_250DPS          0x00
#define MPU6050_FS_500DPS          0x08
#define MPU6050_FS_1000DPS         0x10
#define MPU6050_FS_2000DPS         0x18
#define MPU6050_ACCEL_CONFIG_2G 0x00
#define MPU6050_ACCEL_CONFIG_4G 0x08
#define MPU6050_ACCEL_CONFIG_8G 0x10
#define MPU6050_ACCEL_CONFIG_16G 0x18
#define BIT_INT_ANYRD_2CLEAR        0x10
#define BIT_RAW_RDY_EN          0x01
#define BIT_I2C_IF_DIS          0x10
#define BIT_INT_STATUS_DATA     0x01
#define MPU6050_USER_CTRL_FIFO_RST 0x04
#define MPU6050_USER_CTRL_I2C_MST_EN 0x20
#define MPU6050_USER_CTRL_FIFO_EN 0x40
#define MPU6050_FIFO_ENABLE_TEMP_OUT 0x80
#define MPU6050_FIFO_ENABLE_GYRO_XOUT 0x40
#define MPU6050_FIFO_ENABLE_GYRO_YOUT 0x20
#define MPU6050_FIFO_ENABLE_GYRO_ZOUT 0x10
#define MPU6050_FIFO_ENABLE_ACCEL 0x08
#define MPU6050_INT_STATUS_FIFO_OVERFLOW 0x10
//#define MPU6050_DLPF_CFG_260HZ     0x00
//#define MPU6050_DLPF_CFG_184HZ     0x01
//#define MPU6050_DLPF_CFG_94HZ      0x02
//#define MPU6050_DLPF_CFG_44HZ      0x03
//#define MPU6050_DLPF_CFG_21HZ      0x04
//#define MPU6050_DLPF_CFG_10HZ      0x05
//#define MPU6050_DLPF_CFG_5HZ       0x06

#define MPU6050_CFG_DLPF_CFG_BIT    2
#define MPU6050_CFG_DLPF_CFG_LENGTH 3

#define MPU6050_PWR1_CLKSEL_BIT         2
#define MPU6050_PWR1_CLKSEL_LENGTH      3

#define MPU6050_GCONFIG_FS_SEL_BIT      4
#define MPU6050_GCONFIG_FS_SEL_LENGTH   2

#define MPU6050_ACONFIG_AFS_SEL_BIT         4
#define MPU6050_ACONFIG_AFS_SEL_LENGTH      2

#define MPU6050_PWR1_SLEEP_BIT          6

#define MPU6050_INT_DATA_RDY_BIT        0

#define MPU6500_INT_PIN_CFG_LVL         7
#define MPU6500_INT_PIN_CFG_LVL_LOW     1
#define MPU6500_INT_PIN_CFG_LVL_HIGH    0

#define MPU6500_INT_PIN_CFG_OPEN        6
#define MPU6500_INT_PIN_CFG_OPEN_OD     1
#define MPU6500_INT_PIN_CFG_OPEN_PP     0

#define MPU6500_INT_PIN_CFG_LATCH       5
#define MPU6500_INT_PIN_CFG_LATCH_CLR   1
#define MPU6500_INT_PIN_CFG_LATCH_PULS  0

#define MPU6500_INT_PIN_CFG_RDCLR       4
#define MPU6500_INT_PIN_CFG_RDCLR_ANY   1
#define MPU6500_INT_PIN_CFG_RDCLR_STAT  0

#define MPU6500_INT_PIN_CFG_FSYNC       3
#define MPU6500_INT_PIN_CFG_FSYNC_INT   1
#define MPU6500_INT_PIN_CFG_FSYNC_NONE  0

#define MPU6500_INT_PIN_CFG_FSYNC       3
#define MPU6500_INT_PIN_CFG_FSYNC_LOW   1
#define MPU6500_INT_PIN_CFG_FSYNC_HIGH  0

#define MPU6500_INT_PIN_CFG_FSYNCM      2
#define MPU6500_INT_PIN_CFG_FSYNCM_INT  1
#define MPU6500_INT_PIN_CFG_FSYNCM_NONE 0



#define MPU6050_CLOCK_INTERNAL          0x00
#define MPU6050_CLOCK_PLL_XGYRO         0x01
#define MPU6050_CLOCK_PLL_YGYRO         0x02
#define MPU6050_CLOCK_PLL_ZGYRO         0x03
#define MPU6050_CLOCK_PLL_EXT32K        0x04
#define MPU6050_CLOCK_PLL_EXT19M        0x05
#define MPU6050_CLOCK_KEEP_RESET        0x07


#define MPU6050_GYRO_FS_250         0x00
#define MPU6050_GYRO_FS_500         0x01
#define MPU6050_GYRO_FS_1000        0x02
#define MPU6050_GYRO_FS_2000        0x03

#define MPU6050_ACCEL_FS_2          0x00
#define MPU6050_ACCEL_FS_4          0x01
#define MPU6050_ACCEL_FS_8          0x02
#define MPU6050_ACCEL_FS_16         0x03


#define MPU6050_DLPF_BW_256         0x00
#define MPU6050_DLPF_BW_188         0x01
#define MPU6050_DLPF_BW_98          0x02
#define MPU6050_DLPF_BW_42          0x03
#define MPU6050_DLPF_BW_20          0x04
#define MPU6050_DLPF_BW_10          0x05
#define MPU6050_DLPF_BW_5           0x06

#define MPU6050_RATE_1000HZ         0
#define MPU6050_RATE_500HZ          1
#define MPU6050_RATE_333HZ          2
#define MPU6050_RATE_250HZ          3
#define MPU6050_RATE_200HZ          4


// Length of the FIFO used by the sensor to buffer unread
// sensor data.
#define MPU_MAX_LEN_FIFO_IN_BYTES 512

// Configuration bits MPU 9250
#define BIT_SLEEP           0x40
#define BIT_H_RESET         0x80
#define MPU_CLK_SEL_AUTO    0x01

#define BITS_USER_CTRL_FIFO_EN      0x40
#define BITS_USER_CTRL_FIFO_RST     0x04
#define BITS_USER_CTRL_I2C_MST_EN   0x20
#define BITS_USER_CTRL_I2C_IF_DIS   0x10
#define BITS_USER_CTRL_I2C_MST_RST      0x02

#define BITS_CONFIG_FIFO_MODE_OVERWRITE 0x00
#define BITS_CONFIG_FIFO_MODE_STOP  0x40

#define BITS_GYRO_ST_X          0x80
#define BITS_GYRO_ST_Y          0x40
#define BITS_GYRO_ST_Z          0x20
#define BITS_FS_250DPS          0x00
#define BITS_FS_500DPS          0x08
#define BITS_FS_1000DPS         0x10
#define BITS_FS_2000DPS         0x18
#define BITS_FS_MASK            0x18
// This is FCHOICE_B which is the inverse of FCHOICE
#define BITS_BW_3600HZ          0x02
// The FCHOICE bits are the same for all Bandwidths below 3600 Hz.
#define BITS_BW_LT3600HZ        0x00

#define BITS_DLPF_CFG_250HZ     0x00
#define BITS_DLPF_CFG_184HZ     0x01
#define BITS_DLPF_CFG_92HZ      0x02
#define BITS_DLPF_CFG_41HZ      0x03
#define BITS_DLPF_CFG_20HZ      0x04
#define BITS_DLPF_CFG_10HZ      0x05
#define BITS_DLPF_CFG_5HZ       0x06
#define BITS_DLPF_CFG_3600HZ    0x07
#define BITS_DLPF_CFG_MASK      0x07

#define BITS_FIFO_ENABLE_TEMP_OUT   0x80
#define BITS_FIFO_ENABLE_GYRO_XOUT  0x40
#define BITS_FIFO_ENABLE_GYRO_YOUT  0x20
#define BITS_FIFO_ENABLE_GYRO_ZOUT  0x10
#define BITS_FIFO_ENABLE_ACCEL      0x08
#define BITS_FIFO_ENABLE_SLV2       0x04
#define BITS_FIFO_ENABLE_SLV1       0x02
#define BITS_FIFO_ENABLE_SLV0       0x01

#define BITS_ACCEL_CONFIG_16G       0x18

// This is ACCEL_FCHOICE_B which is the inverse of ACCEL_FCHOICE
#define BITS_ACCEL_CONFIG2_BW_1130HZ    0x08

#define BITS_I2C_SLV0_EN    0x80
#define BITS_I2C_SLV0_READ_8BYTES 0x08
#define BITS_I2C_SLV1_EN    0x80
#define BITS_I2C_SLV1_DIS   0x00
#define BITS_I2C_SLV2_EN    0x80
#define BITS_I2C_SLV4_EN    0x80
#define BITS_I2C_SLV4_DONE  0x40

#define BITS_SLV4_DLY_EN    0x10
#define BITS_SLV3_DLY_EN    0x08
#define BITS_SLV2_DLY_EN    0x04
#define BITS_SLV1_DLY_EN    0x02
#define BITS_SLV0_DLY_EN    0x01

#define BIT_RAW_RDY_EN      0x01
#define BIT_INT_ANYRD_2CLEAR    0x10

#define I2C_READ_FLAG       0x80


#define BITS_INT_STATUS_FIFO_OVERFLOW   0x10

#define BITS_I2C_MST_CLK_400_KHZ    0x0D

#ifndef M_PI_F
#define M_PI_F 3.14159265358979323846f
#endif

#if defined(__DF_EDISON)
// update frequency 250 Hz
#define MPU9250_MEASURE_INTERVAL_US 4000
#else
// update frequency 1000 Hz
#define MPU9250_MEASURE_INTERVAL_US 1000
#endif

// -2000 to 2000 degrees/s, 16 bit signed register, deg to rad conversion
#define GYRO_RAW_TO_RAD_S    (2000.0f / 32768.0f * M_PI_F / 180.0f)

// TODO: include some common header file (currently in drv_sensor.h).
#define DRV_DF_DEVTYPE_MPU9250 0x41

#define AK8963_ADDRESS   0x0C
#define WHO_AM_I_AK8963  0x00 // should return 0x48
#define INFO             0x01
#define AK8963_ST1       0x02  // data ready status bit 0
#define AK8963_XOUT_L    0x03  // data
#define AK8963_XOUT_H    0x04
#define AK8963_YOUT_L    0x05
#define AK8963_YOUT_H    0x06
#define AK8963_ZOUT_L    0x07
#define AK8963_ZOUT_H    0x08
#define AK8963_ST2       0x09  // Data overflow bit 3 and data read error status bit 2
#define AK8963_CNTL      0x0A  // Power down (0000), single-measurement (0001), self-test (1000) and Fuse ROM (1111) modes on bits 3:0
#define AK8963_CNTL2     0x0B  // Reset
#define AK8963_ASTC      0x0C  // Self test control
#define AK8963_I2CDIS    0x0F  // I2C disable
#define AK8963_ASAX      0x10  // Fuse ROM x-axis sensitivity adjustment value
#define AK8963_ASAY      0x11  // Fuse ROM y-axis sensitivity adjustment value
#define AK8963_ASAZ      0x12  // Fuse ROM z-axis sensitivity adjustment value

#define WHO_AM_I_AK8963_VAL 72

#define AK8963_POWER_DOWN   0
#define AK8963_RESET        1

#define AK8963_FUSE_ROM     0x0F

#define AK8963_CNT_MEAS1    0x12
#define AK8963_CNT_MEAS2    0x16


#pragma pack(1)
typedef AXPACKED(struct) __tag_fifo_packet
{
    U16 accel_x;
    U16 accel_y;
    U16 accel_z;
    U16 temp;
    U16 gyro_x;
    U16 gyro_y;
    U16 gyro_z;
} fifo_packet_t;
#pragma pack(0)


#pragma pack(1)
typedef AXPACKED(struct) __tag_sensor_packet
{
    U16 x;
    U16 y;
    U16 z;
} sensor_packet_t;
#pragma pack(0)

#pragma pack(1)
typedef AXPACKED(struct) __tag_sensor_packet2
{
    sensor_packet_t accl;
    U16 temp;
    sensor_packet_t gyro;
} sensor_packet2_t;
#pragma pack(0)

#pragma pack(1)
typedef AXPACKED(struct) __tag_sensor_packet3
{
    sensor_packet_t accl;
    U16 temp;
    sensor_packet_t gyro;
    sensor_packet_t mag;
    U8              st2;
} sensor_packet3_t;
#pragma pack(0)

#pragma pack(1)
typedef AXPACKED(struct) __tag_magnet_packet
{
    U16 x;
    U16 y;
    U16 z;
    U8  st2;
} magnet_packet_t;
#pragma pack(0)

#pragma pack(1)
typedef AXPACKED(struct) __tag_big_packet
{
    U16 gx;
    U16 gy;
    U16 gz;
    U16 ax;
    U16 ay;
    U16 az;
    U16 mx;
    U16 my;
    U16 mz;
    U8  st2;
} big_packet_t;
#pragma pack(0)


//typedef struct fp_vector
//{
//    float X;
//    float Y;
//    float Z;
//} t_fp_vector_def;

//typedef union
//{
//    float   A[3];
//    t_fp_vector_def V;
//} t_fp_vector;


#endif // #ifndef ____MPU6XXX_PVT_H__


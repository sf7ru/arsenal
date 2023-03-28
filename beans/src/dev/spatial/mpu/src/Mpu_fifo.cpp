

#include <Mpu.h>
#include <Mpu_pvt.h>

#include <string.h>
#include <math.h>

#include <stdio.h>

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

BOOL Mpu::fifoRead(SPATIALVECTOR   &report)
{
    BOOL            result          = false;
    U8 int_status = 0;
    int  _packets_per_cycle_filtered;

    ENTER(true);

    if (registerRead(MPUREG_INT_STATUS, &int_status, 1) > 0)
    {
        if (!(int_status & BITS_INT_STATUS_FIFO_OVERFLOW))
        {
            int size_of_fifo_packet = sizeof(fifo_packet_t);

            // Get FIFO byte count to read and floor it to the report size.
            int bytes_to_read = (fifoGetCount() / size_of_fifo_packet) * size_of_fifo_packet;
            _packets_per_cycle_filtered = (0.95f * _packets_per_cycle_filtered) + (0.05f * (bytes_to_read / size_of_fifo_packet));

            if (bytes_to_read > 0)
            {
                // Allocate a buffer large enough for n complete packets, read from the
                // sensor FIFO.
                const unsigned buf_len = (MPU_MAX_LEN_FIFO_IN_BYTES / size_of_fifo_packet) * size_of_fifo_packet;
                U8 fifo_read_buf[buf_len];

                const unsigned read_len = MAC_MIN((unsigned)bytes_to_read, buf_len);
                memset(fifo_read_buf, 0x0, buf_len);

                if (registerRead(MPUREG_FIFO_R_W, fifo_read_buf, read_len) > 0)
                {
                    for (unsigned packet_index = 0; packet_index < (read_len / size_of_fifo_packet); ++packet_index) {

                        fifo_packet_t *rpt = (fifo_packet_t *)(&fifo_read_buf[packet_index   * size_of_fifo_packet]);

                        rpt->accel_x = MAC_BE_U16(rpt->accel_x);
                        rpt->accel_y = MAC_BE_U16(rpt->accel_y);
                        rpt->accel_z = MAC_BE_U16(rpt->accel_z);
                        rpt->temp    = MAC_BE_U16(rpt->temp);
                        rpt->gyro_x  = MAC_BE_U16(rpt->gyro_x);
                        rpt->gyro_y  = MAC_BE_U16(rpt->gyro_y);
                        rpt->gyro_z  = MAC_BE_U16(rpt->gyro_z);


                        // Check if the full accel range of the accel has been used. If this occurs, it is
                        // either a spike due to a crash/landing or a sign that the vibrations levels
                        // measured are excessive.
//                        if (report->accel_x == INT16_MIN || report->accel_x == INT16_MAX ||
//                            report->accel_y == INT16_MIN || report->accel_y == INT16_MAX ||
//                            report->accel_z == INT16_MIN || report->accel_z == INT16_MAX)
//                        {
//                            m_synchronize.lock();
//                            ++m_sensor_data.accel_range_hit_counter;
//                            m_synchronize.unlock();
//                        }

                        // Also check the full gyro range, however, this is very unlikely to happen.
//                        if (report->gyro_x == INT16_MIN || report->gyro_x == INT16_MAX ||
//                            report->gyro_y == INT16_MIN || report->gyro_y == INT16_MAX ||
//                            report->gyro_z == INT16_MIN || report->gyro_z == INT16_MAX) {
//                            m_synchronize.lock();
//                            ++m_sensor_data.gyro_range_hit_counter;
//                            m_synchronize.unlock();
//                        }

                        const float temp_c = float(rpt->temp) / 340.0f + 36.53f;

                        printf("X = %d Y = %d Z = %d temp = %d\n", rpt->gyro_x, rpt->gyro_y, rpt->gyro_z, (int)temp_c);


                        // Use the temperature field to try to detect if we (ever) fall out of sync with
                        // the FIFO buffer. If the temperature changes insane amounts, reset the FIFO logic
                        // and return early.
                        if (!_temp_initialized)
                        {
                            // Assume that the temperature should be in a sane range of -40 to 85 deg C which is
                            // the specified temperature range, at least to initialize.
                            if (temp_c > -40.0f && temp_c < 85.0f)
                            {

                                // Initialize the temperature logic.
                                _last_temp_c = temp_c;
                                printf("IMU temperature initialized to: %d\n", (int) temp_c);
                                _temp_initialized = true;
                            }

                        } else {
                            // Once initialized, check for a temperature change of more than 2 degrees which
                            // points to a FIFO corruption.
                            if (fabsf(temp_c - _last_temp_c) > 2.0f)
                            {
                                printf("FIFO corrupt, temp difference: %d, last temp: %d, current temp: %d",
                                       (int)fabs(temp_c - _last_temp_c), (int)_last_temp_c, (int)temp_c);

                                fifoReset();
                                _temp_initialized = false;
                            }

                            _last_temp_c = temp_c;
                        }

//                        m_sensor_data.accel_m_s2_x = float(report->accel_x)
//                                         * (MPU6050_ONE_G / 2048.0f);
//                        m_sensor_data.accel_m_s2_y = float(report->accel_y)
//                                         * (MPU6050_ONE_G / 2048.0f);
//                        m_sensor_data.accel_m_s2_z = float(report->accel_z)
//                                         * (MPU6050_ONE_G / 2048.0f);
//                        m_sensor_data.temp_c = temp_c;
//                        m_sensor_data.gyro_rad_s_x = float(report->gyro_x) * GYRO_RAW_TO_RAD_S;
//                        m_sensor_data.gyro_rad_s_y = float(report->gyro_y) * GYRO_RAW_TO_RAD_S;
//                        m_sensor_data.gyro_rad_s_z = float(report->gyro_z) * GYRO_RAW_TO_RAD_S;
//
//                        // Pass on the sampling interval between FIFO samples at 1kHz.
//                        m_sensor_data.fifo_sample_interval_us = 1000000 / MPU6050_MEASURE_INTERVAL_US
//                                            / _packets_per_cycle_filtered;
//
//                        // Flag if this is the last sample, and _publish() should wrap up the data it has received.
//                        m_sensor_data.is_last_fifo_sample = ((packet_index + 1) == (read_len / size_of_fifo_packet));
//
//                        ++m_sensor_data.read_counter;

                        // Generate debug output every second, assuming that a sample is generated every
                        // 125 usecs
#ifdef MPU6050_DEBUG

                        if (++m_sensor_data.read_counter % (1000000 / 125) == 0) {

                        DF_LOG_INFO("IMU: accel: [%f, %f, %f]",
                                (double)m_sensor_data.accel_m_s2_x,
                                (double)m_sensor_data.accel_m_s2_y,
                                (double)m_sensor_data.accel_m_s2_z);
                        DF_LOG_INFO("     gyro:  [%f, %f, %f]",
                                (double)m_sensor_data.gyro_rad_s_x,
                                (double)m_sensor_data.gyro_rad_s_y,
                                (double)m_sensor_data.gyro_rad_s_z);
                        DF_LOG_INFO("    temp:  %f C", (double)m_sensor_data.temp_c);
                    }

#endif

//                        _publish(m_sensor_data);
//
//                        m_synchronize.signal();
//                        m_synchronize.unlock();
                    }
                }
            }
        }
        else
            fifoReset();
    }

    RETURN(result);
}

// ***************************************************************************
// FUNCTION
//      I2cMpu::fifoReset
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL Mpu::fifoReset()
{
    BOOL            result          = false;
    U8              bits            = BITS_USER_CTRL_FIFO_RST | BITS_USER_CTRL_FIFO_EN;

    ENTER(true);

    result = (registerWrite(MPUREG_USER_CTRL, bits) > 0);

    RETURN(result);
}
// ***************************************************************************
// FUNCTION
//      I2cMpu::getFifoCount
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      UINT --
// ***************************************************************************
INT Mpu::fifoGetCount()
{
    INT         result     = -1;
    U16         buff;

    if (registerRead(MPUREG_FIFO_COUNTH, (U8 *) &buff, sizeof(buff)) > 0)
    {
        result = MAC_BE_U16(buff);
    }
    else
        result = -1;

    return result;
}

// ***************************************************************************
// FUNCTION
//      I2cMpu::fifoEnable
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      BOOL
//            TRUE:   Ok
//            FALSE:  Error
// ***************************************************************************
BOOL Mpu::enableFifo(BOOL ena)
{
    BOOL            result          = false;
    U8              bits;

    ENTER(true);

    if (registerWrite(MPUREG_USER_CTRL, BITS_USER_CTRL_FIFO_RST | BITS_USER_CTRL_FIFO_EN) > 0)
    {
        axudelay(MPUDELAY_INIT);

        bits = BITS_FIFO_ENABLE_TEMP_OUT | BITS_FIFO_ENABLE_GYRO_XOUT
               | BITS_FIFO_ENABLE_GYRO_YOUT
               | BITS_FIFO_ENABLE_GYRO_ZOUT
               | BITS_FIFO_ENABLE_ACCEL |
               (flags.enabledMag ? BITS_FIFO_ENABLE_SLV0 : 0);

        if (registerWrite(MPUREG_FIFO_EN, bits) > 0)
        {
            axudelay(MPUDELAY_INIT);

            result = true;
        }
    }

    RETURN(result);
}

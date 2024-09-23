/**
 *
 * @license MIT License
 *
 * Copyright (c) 2022 lewis he
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file      SensorQMI8658.hpp
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2022-10-16
 *
 */
#pragma once

#include "REG/QMI8658Constants.h"
#include "SensorCommon.tpp"
#ifndef ARDUINO
#include <math.h>
#include <stdio.h>
#endif
typedef struct __IMUdata {
    float x;
    float y;
    float z;
} IMUdata;

class SensorQMI8658 :
    public SensorCommon<SensorQMI8658>
{
    friend class SensorCommon<SensorQMI8658>;
public:

    typedef void (*EventCallBack_t)(void);

    enum AccelRange {
        ACC_RANGE_2G,
        ACC_RANGE_4G,
        ACC_RANGE_8G,
        ACC_RANGE_16G
    };

    enum GyroRange {
        GYR_RANGE_16DPS,
        GYR_RANGE_32DPS,
        GYR_RANGE_64DPS,
        GYR_RANGE_128DPS,
        GYR_RANGE_256DPS,
        GYR_RANGE_512DPS,
        GYR_RANGE_1024DPS,
    };

    // In 6DOF mode (accelerometer and gyroscope are both enabled),
    // the output data rate is derived from the nature frequency of gyroscope
    enum AccelODR {
        ACC_ODR_1000Hz = 3,
        ACC_ODR_500Hz,
        ACC_ODR_250Hz,
        ACC_ODR_125Hz,
        ACC_ODR_62_5Hz,
        ACC_ODR_31_25Hz,
        ACC_ODR_LOWPOWER_128Hz  = 12,   //The accelerometer low power mode is only available when the gyroscope is disabled
        ACC_ODR_LOWPOWER_21Hz,          //The accelerometer low power mode is only available when the gyroscope is disabled
        ACC_ODR_LOWPOWER_11Hz,          //The accelerometer low power mode is only available when the gyroscope is disabled
        ACC_ODR_LOWPOWER_3Hz            //The accelerometer low power mode is only available when the gyroscope is disabled
    };

    enum GyroODR {
        GYR_ODR_7174_4Hz,
        GYR_ODR_3587_2Hz,
        GYR_ODR_1793_6Hz,
        GYR_ODR_896_8Hz,
        GYR_ODR_448_4Hz,
        GYR_ODR_224_2Hz,
        GYR_ODR_112_1Hz,
        GYR_ODR_56_05Hz,
        GYR_ODR_28_025Hz
    };

    //Low-Pass Filter.
    enum LpfMode {
        LPF_MODE_0,     //2.66% of ODR
        LPF_MODE_1,     //3.63% of ODR
        LPF_MODE_2,     //5.39% of ODR
        LPF_MODE_3,     //13.37% of ODR
    };

    enum MotionEvent {
        MOTION_TAP,
        MOTION_ANT_MOTION,
        MOTION_NO_MOTION,
        MOTION_SIGNIFICANT,
        MOTION_PEDOMETER,
    };

    enum IntPin {
        IntPin1,
        IntPin2,
    };

    enum Fifo_Samples {
        FIFO_SAMPLES_16,
        FIFO_SAMPLES_32,
        FIFO_SAMPLES_64,
        FIFO_SAMPLES_128,
        FIFO_SAMPLES_MAX,
    } ;

    enum FIFO_Mode {
        FIFO_MODE_BYPASS,
        FIFO_MODE_FIFO,
        FIFO_MODE_STREAM,
        FIFO_MODE_MAX,
    };

    enum SampleMode {
        SYNC_MODE,      //Synchronous sampling
        ASYNC_MODE,     //Asynchronous sampling
    };

    enum CommandTable {
        CTRL_CMD_ACK                            = 0x00,
        CTRL_CMD_RST_FIFO                       = 0x04,
        CTRL_CMD_REQ_FIFO                       = 0x05,
        CTRL_CMD_WRITE_WOM_SETTING              = 0x08,
        CTRL_CMD_ACCEL_HOST_DELTA_OFFSET        = 0x09,
        CTRL_CMD_GYRO_HOST_DELTA_OFFSET         = 0x0A,
        CTRL_CMD_CONFIGURE_TAP                  = 0x0C,
        CTRL_CMD_CONFIGURE_PEDOMETER            = 0x0D,
        CTRL_CMD_CONFIGURE_MOTION               = 0x0E,
        CTRL_CMD_RESET_PEDOMETER                = 0x0F,
        CTRL_CMD_COPY_USID                      = 0x10,
        CTRL_CMD_SET_RPU                        = 0x11,
        CTRL_CMD_AHB_CLOCK_GATING               = 0x12,
        CTRL_CMD_ON_DEMAND_CALIBRATION          = 0xA2,
        CTRL_CMD_APPLY_GYRO_GAINS               = 0xAA,
    };

    enum StatusReg {
        EVENT_SIGNIFICANT_MOTION = 128,
        EVENT_NO_MOTION = 64,
        EVENT_ANY_MOTION = 32,
        EVENT_PEDOMETER_MOTION = 16,
        EVENT_WOM_MOTION = 4,
        EVENT_TAP_MOTION = 2,
    };

    enum SensorStatus {
        STATUS_INT_CTRL9_CMD_DONE = _BV(0),
        STATUS_INT_LOCKED = _BV(1),
        STATUS_INT_AVAIL = _BV(2),
        STATUS0_GYRO_DATA_READY = _BV(3),
        STATUS0_ACCEL_DATA_READY = _BV(4),
        STATUS1_SIGNIFICANT_MOTION = _BV(5),
        STATUS1_NO_MOTION = _BV(6),
        STATUS1_ANY_MOTION = _BV(7),
        STATUS1_PEDOMETER_MOTION = _BV(8),
        STATUS1_WOM_MOTION = _BV(9),
        STATUS1_TAP_MOTION = _BV(10),
    };

    enum TagPriority {
        PRIORITY0,      // (X > Y> Z)
        PRIORITY1,      // (X > Z > Y)
        PRIORITY2,      // (Y > X > Z)
        PRIORITY3,      // (Y > Z > X)
        PRIORITY4,      // (Z > X > Y)
        PRIORITY5,      // (Z > Y > X)
    };

#if defined(ARDUINO)
    SensorQMI8658(PLATFORM_WIRE_TYPE &w, int sda = DEFAULT_SDA, int scl = DEFAULT_SCL, uint8_t addr = QMI8658_L_SLAVE_ADDRESS)
    {
        __wire = &w;
        __sda = sda;
        __scl = scl;
        __addr = addr;
    }

    SensorQMI8658(int cs, int mosi = -1, int miso = -1, int sck = -1, PLATFORM_SPI_TYPE &spi = SPI)
    {
        __spi = &spi;
        __cs = cs;
        __miso = miso;
        __mosi = mosi;
        __sck = sck;
    }

#endif

    SensorQMI8658()
    {
#if defined(ARDUINO)
        __wire = &Wire;
        __sda = DEFAULT_SDA;
        __scl = DEFAULT_SCL;
#endif
        __addr = QMI8658_L_SLAVE_ADDRESS;
    }

    ~SensorQMI8658()
    {
        log_i("~SensorQMI8658");
        deinit();
    }

#if defined(ARDUINO)
    bool init(PLATFORM_WIRE_TYPE &w, int sda = DEFAULT_SDA, int scl = DEFAULT_SCL, uint8_t addr = QMI8658_L_SLAVE_ADDRESS)
    {
        return SensorCommon::begin(w, addr, sda, scl);
    }
#endif

    void deinit()
    {
        // end();
    }

    bool reset(bool waitResult = true, uint32_t timeout = 500)
    {
        int val = 0;  // initialize with some value to avoid compilation errors
        writeRegister(QMI8658_REG_RESET, QMI8658_REG_RESET_DEFAULT);
        // Maximum 15ms for the Reset process to be finished
        if (waitResult) {
            uint32_t start = millis();
            while (millis() - start < timeout) {
                val = readRegister(QMI8658_REG_RST_RESULT);
                if (val != DEV_WIRE_ERR && val == QMI8658_REG_RST_RESULT_VAL) {

                    //EN.ADDR_AI
                    setRegisterBit(QMI8658_REG_CTRL1, 6);

                    return true;
                }
                delay(10);
            }
            log_e("Reset chip failed, Response val = %d - 0x%X", val, val);
            return false;
        }

        //EN.ADDR_AI
        setRegisterBit(QMI8658_REG_CTRL1, 6);

        return true;
    }

    uint8_t getChipID()
    {
        return readRegister(QMI8658_REG_REVISION);
    }

    int whoAmI()
    {
        return readRegister(QMI8658_REG_WHOAMI);
    }

    uint32_t getTimestamp()
    {
        uint8_t  buffer[3];
        uint32_t timestamp;
        if (readRegister(QMI8658_REG_TIMESTAMP_L, buffer, 3) != DEV_WIRE_ERR) {
            timestamp = (uint32_t)(((uint32_t)buffer[2] << 16) |
                                   ((uint32_t)buffer[1] << 8) | buffer[0]);
            if (timestamp > lastTimestamp) {
                lastTimestamp = timestamp;
            } else {
                lastTimestamp = (timestamp + 0x1000000 - lastTimestamp);
            }
        }
        return lastTimestamp;
    }


    float getTemperature_C()
    {
        uint8_t buffer[2];
        if (readRegister(QMI8658_REG_TEMPERATURE_L, buffer, 2) !=  DEV_WIRE_ERR) {
            return (float)buffer[1] + ((float)buffer[0] / 256.0);
        }
        return NAN;
    }

    void enableINT(IntPin pin, bool enable = true)
    {
        switch (pin) {
        case IntPin1:
            enable ? setRegisterBit(QMI8658_REG_CTRL1, 3) : clrRegisterBit(QMI8658_REG_CTRL1, 3);
            break;
        case IntPin2:
            enable ? setRegisterBit(QMI8658_REG_CTRL1, 4) : clrRegisterBit(QMI8658_REG_CTRL1, 4);
            break;
        default:
            break;
        }
    }

    uint8_t getIrqStatus()
    {
        return readRegister(QMI8658_REG_STATUS_INT);
    }


    void enableDataReadyINT(bool enable = true)
    {
        enable ? clrRegisterBit(QMI8658_REG_CTRL7, 5) :
        setRegisterBit(QMI8658_REG_CTRL7, 5);
    }

    /**
     * @brief
     * @note
     * @param  range:
     * @param  odr:
     * @param  lpfOdr:
     * @param  lpf:
     * @param  selfTest:
     * @retval
     */
    int configAccelerometer(AccelRange range, AccelODR odr, LpfMode lpfOdr = LPF_MODE_0,
                            bool lpf = true)
    {
        bool en = isEnableAccelerometer();

        if (en) {
            disableAccelerometer();
        }

        //setAccelRange
        if (writeRegister(QMI8658_REG_CTRL2, 0x8F, (range << 4)) != DEV_WIRE_NONE) {
            return DEV_WIRE_ERR;
        }

        switch (range) {
        // Possible accelerometer scales (and their register bit settings) are:
        // 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11).
        // Here's a bit of an algorithm to calculate DPS/(ADC tick) based on that
        // 2-bit value:
        case ACC_RANGE_2G:  accelScales = 2.0 / 32768.0; break;
        case ACC_RANGE_4G:  accelScales = 4.0 / 32768.0; break;
        case ACC_RANGE_8G:  accelScales = 8.0 / 32768.0; break;
        case ACC_RANGE_16G: accelScales = 16.0 / 32768.0; break;
        }

        // setAccelOutputDataRate
        if (writeRegister(QMI8658_REG_CTRL2, 0xF0, odr) != DEV_WIRE_NONE) {
            return DEV_WIRE_ERR;
        }

        // setAccelLowPassFitter
        lpf ? setRegisterBit(QMI8658_REG_CTRL5, 0) : clrRegisterBit(QMI8658_REG_CTRL5, 0);

        // setAccelLowPassFitterOdr
        if (writeRegister(QMI8658_REG_CTRL5, QMI8658_ACCEL_LPF_MASK,  (lpfOdr << 1)) != DEV_WIRE_NONE) {
            return DEV_WIRE_ERR;
        }

        // setAccelSelfTest
        // selfTest ? setRegisterBit(QMI8658_REG_CTRL2, 7) : clrRegisterBit(QMI8658_REG_CTRL2, 7);

        if (en) {
            enableAccelerometer();
        }

        return DEV_WIRE_NONE;
    }

    /**
     * @brief
     * @note
     * @param  range:   Gyro see GyroRange
     * @param  odr(Output data rate): see GyroODR
     * @param  lpfOdr(Low-Pass Filter Output data rate):
     * @param  lpf (Low-Pass Filter Mode): see LpfMode
     * @retval
     */
    int configGyroscope(GyroRange range, GyroODR odr, LpfMode lpfOdr = LPF_MODE_0,
                        bool lpf = true)
    {
        bool en = isEnableGyroscope();

        if (en) {
            disableGyroscope();
        }

        // setGyroRange
        if (writeRegister(QMI8658_REG_CTRL3, 0x8F, (range << 4)) != DEV_WIRE_NONE) {
            return DEV_WIRE_ERR;
        }

        switch (range) {
        // Possible gyro scales (and their register bit settings) are:
        // 250 DPS (00), 500 DPS (01), 1000 DPS (10), and 2000 DPS  (11).
        // Here's a bit of an algorithm to calculate DPS/(ADC tick) based on that
        // 2-bit value:
        case GYR_RANGE_16DPS: gyroScales = 16.0 / 32768.0; break;
        case GYR_RANGE_32DPS: gyroScales = 32.0 / 32768.0; break;
        case GYR_RANGE_64DPS: gyroScales = 64.0 / 32768.0; break;
        case GYR_RANGE_128DPS: gyroScales = 128.0 / 32768.0; break;
        case GYR_RANGE_256DPS: gyroScales = 256.0 / 32768.0; break;
        case GYR_RANGE_512DPS: gyroScales = 512.0 / 32768.0; break;
        case GYR_RANGE_1024DPS: gyroScales = 1024.0 / 32768.0; break;
        }

        // setGyroOutputDataRate
        if (writeRegister(QMI8658_REG_CTRL3, 0xF0, odr) != DEV_WIRE_NONE) {
            return DEV_WIRE_ERR;
        }

        // setGyroLowPassFitter
        lpf ? setRegisterBit(QMI8658_REG_CTRL5, 4) : clrRegisterBit(QMI8658_REG_CTRL5, 4);


        // setGyroLowPassFitterOdr
        if (writeRegister(QMI8658_REG_CTRL5, QMI8658_GYRO_LPF_MASK,  (lpfOdr << 5)) != DEV_WIRE_NONE) {
            return DEV_WIRE_ERR;
        }

        // setGyroSelfTest
        // selfTest ? setRegisterBit(QMI8658_REG_CTRL3, 7) : clrRegisterBit(QMI8658_REG_CTRL3, 7);

        if (en) {
            enableGyroscope();
        }

        return DEV_WIRE_NONE;
    }

    /**
     * @brief
     * @note
     * @param  mode:
     * @param  samples:
     * @param  pin:
     * @param  watermark:
     * @retval
     */
    int configFIFO(FIFO_Mode    mode,
                   Fifo_Samples samples = FIFO_SAMPLES_16, IntPin pin = IntPin2,
                   uint8_t watermark = 8)
    {
        bool enGyro = isEnableGyroscope();
        bool enAccel = isEnableAccelerometer();

        if (enGyro) {
            disableGyroscope();
        }

        if (enAccel) {
            disableAccelerometer();
        }

        /////////////////
        pin == IntPin2 ? clrRegisterBit(QMI8658_REG_CTRL1, 2) : setRegisterBit(QMI8658_REG_CTRL1, 2);

        // set fifo mode and samples len
        fifoMode = (samples << 2) | mode;
        if (writeRegister(QMI8658_REG_FIFO_CTRL, fifoMode) == DEV_WIRE_ERR) {
            return DEV_WIRE_ERR;
        }

        // set watermark
        if (writeRegister(QMI8658_REG_FIFO_WTM_TH, watermark) == DEV_WIRE_ERR) {
            return DEV_WIRE_ERR;
        }

        //reset fifo
        writeCommand(CTRL_CMD_RST_FIFO);
        /////////////////

        if (enGyro) {
            enableGyroscope();
        }

        if (enAccel) {
            enableAccelerometer();
        }

        return DEV_WIRE_NONE;
    }

    uint16_t getFifoNeedBytes()
    {
        uint8_t sam[] = {16, 32, 64, 128};
        uint8_t sensors  = 0;
        if (gyroEn && accelEn) {
            sensors = 2;
        } else if (gyroEn || accelEn) {
            sensors = 1;
        }
        uint8_t samples =  ((fifoMode >> 2) & 0x03) ;
        return sam[samples] * 6 * sensors;
    }

    bool readFromFifo(IMUdata *acc, uint16_t accLength, IMUdata *gyr, uint16_t gyrLength)
    {
        uint16_t bytes = getFifoNeedBytes();
        uint8_t *buffer = new uint8_t [bytes];
        if (!buffer) {
            log_e("No memory!");
            return false;
        }
        if (!readFromFifo(buffer, bytes)) {
            delete buffer;
            return false;
        }

        int counter = 0;
        for (int i = 0; i < bytes; ) {
            if (accelEn) {
                if (counter < accLength) {
                    acc[counter].x = (float)((int16_t)buffer[i]     | (buffer[i + 1] << 8)) * accelScales;
                    acc[counter].y = (float)((int16_t)buffer[i + 2] | (buffer[i + 3] << 8)) * accelScales;
                    acc[counter].z = (float)((int16_t)buffer[i + 4] | (buffer[i + 5] << 8)) * accelScales;
                }
                i += 6;
            }

            if (gyroEn) {
                if (counter < gyrLength) {
                    gyr[counter].x = (float)((int16_t)buffer[i]     | (buffer[i + 1] << 8)) * gyroScales;
                    gyr[counter].y = (float)((int16_t)buffer[i + 2] | (buffer[i + 3] << 8)) * gyroScales;
                    gyr[counter].z = (float)((int16_t)buffer[i + 4] | (buffer[i + 5] << 8)) * gyroScales;
                }
                i += 6;
            }
            counter++;
        }
        delete buffer;
        return true;
    }

    bool readFromFifo(uint8_t *data, size_t length)
    {
        uint8_t  status[2];
        uint8_t  fifo_sensors = 1;
        uint16_t fifo_bytes   = 0;
        uint16_t fifo_level   = 0;

        // get fifo status
        int val = readRegister(QMI8658_REG_FIFO_STATUS);
        if (val == DEV_WIRE_ERR) {
            return false;
        }
        log_d("fifo status:0x%x", val);

        if (val & (1 << 5)) {
            log_e("\t\tFIFO Overflow condition has happened (data dropping happened)");
        }
        if (val & (1 << 6)) {
            log_e("\t\tFIFO Water Mark Level Hit");
        }
        if (val & (1 << 7)) {
            log_e("\t\tFIFO is Full");
        }

        val = readRegister(QMI8658_REG_FIFO_COUNT, status, 2);
        if (val == DEV_WIRE_ERR) {
            return false;
        }

        fifo_bytes = ((status[1] & 0x03)) << 8 | status[0];

        if (accelEn && gyroEn) {
            fifo_sensors = 2;
        } else if (accelEn || gyroEn) {
            fifo_sensors = 1;
        }

        fifo_level = fifo_bytes / (3 * fifo_sensors);
        fifo_bytes = fifo_level * (6 * fifo_sensors);

        log_d("fifo-level : %d fifo_bytes : %d fifo_sensors : %d", fifo_level, fifo_bytes, fifo_sensors);
        if (length < fifo_bytes) {
            writeCommand(CTRL_CMD_RST_FIFO);
            return false;
        }

        if (fifo_level) {
            writeCommand(CTRL_CMD_REQ_FIFO);

            if (readRegister(QMI8658_REG_FIFO_DATA, data, fifo_bytes) ==
                    DEV_WIRE_ERR) {
                log_e("get fifo error !");
                return false;
            }

            val = writeRegister(QMI8658_REG_FIFO_CTRL, fifoMode);
            if (val == DEV_WIRE_ERR) {
                return false;
            }
        }

        writeCommand(CTRL_CMD_RST_FIFO);

        return fifo_level;
    }

    bool enableAccelerometer()
    {
        accelEn = true;
        return setRegisterBit(QMI8658_REG_CTRL7, 0) == DEV_WIRE_NONE;
    }

    bool disableAccelerometer()
    {
        accelEn = false;
        return clrRegisterBit(QMI8658_REG_CTRL7, 0) == DEV_WIRE_NONE;
    }

    bool isEnableAccelerometer()
    {
        accelEn = getRegisterBit(QMI8658_REG_CTRL7, 0);
        return accelEn;
    }

    bool isEnableGyroscope()
    {
        gyroEn = getRegisterBit(QMI8658_REG_CTRL7, 1);
        return gyroEn;
    }

    bool enableGyroscope()
    {
        gyroEn = true;
        return setRegisterBit(QMI8658_REG_CTRL7, 1) == DEV_WIRE_NONE;
    }

    bool disableGyroscope()
    {
        gyroEn = false;
        return clrRegisterBit(QMI8658_REG_CTRL7, 1) == DEV_WIRE_NONE;
    }

    bool getAccelRaw(int16_t *rawBuffer)
    {
        if (!accelEn)return false;
        uint8_t buffer[6] = {0};
        if (readRegister(QMI8658_REG_AX_L, buffer, 6) != DEV_WIRE_ERR) {
            rawBuffer[0] = (int16_t)(buffer[1] << 8) | (buffer[0]);
            rawBuffer[1] = (int16_t)(buffer[3] << 8) | (buffer[2]);
            rawBuffer[2] = (int16_t)(buffer[5] << 8) | (buffer[4]);
        } else {
            return false;
        }
        return true;
    }

    bool getAccelerometer(float &x, float &y, float &z)
    {
        if (!accelEn)return false;
        int16_t raw[3];
        if (getAccelRaw(raw)) {
            x = raw[0] * accelScales;
            y = raw[1] * accelScales;
            z = raw[2] * accelScales;
            return true;
        }
        return false;
    }

    float getAccelerometerScales()
    {
        return accelScales;
    }

    float getGyroscopeScales()
    {
        return gyroScales;
    }

    bool getGyroRaw(int16_t *rawBuffer)
    {
        if (!gyroEn)return false;
        uint8_t buffer[6] = {0};
        if (readRegister(QMI8658_REG_GX_L, buffer, 6) != DEV_WIRE_ERR) {
            rawBuffer[0] = (int16_t)(buffer[1] << 8) | (buffer[0]);
            rawBuffer[1] = (int16_t)(buffer[3] << 8) | (buffer[2]);
            rawBuffer[2] = (int16_t)(buffer[5] << 8) | (buffer[4]);
        } else {
            return false;
        }
        return true;
    }

    int getGyroscope(float &x, float &y, float &z)
    {
        if (!gyroEn)return false;
        int16_t raw[3];
        if (getGyroRaw(raw)) {
            x = raw[0] * gyroScales;
            y = raw[1] * gyroScales;
            z = raw[2] * gyroScales;
            return true;
        }
        return false;
    }

    bool getDataReady()
    {
        switch (sampleMode) {
        case SYNC_MODE:
            return  getRegisterBit(QMI8658_REG_STATUS_INT, 1);
        case ASYNC_MODE:
            //TODO: When Accel and Gyro are configured with different rates, this will always be false
            if (gyroEn && accelEn) {
                return readRegister(QMI8658_REG_STATUS0) & 0x03;
            } else if (gyroEn) {
                return readRegister(QMI8658_REG_STATUS0) & 0x02;
            } else if (accelEn) {
                return readRegister(QMI8658_REG_STATUS0) & 0x01;
            }
            break;
        default:
            break;
        }
        return false;
    }

    int enableSyncSampleMode()
    {
        sampleMode = SYNC_MODE;
        return setRegisterBit(QMI8658_REG_CTRL7, 7);
    }

    int disableSyncSampleMode()
    {
        sampleMode = ASYNC_MODE;
        return clrRegisterBit(QMI8658_REG_CTRL7, 7);
    }

    int enableLockingMechanism()
    {
        enableSyncSampleMode();
        if (writeRegister(QMI8658_REG_CAL1_L, 0x01) != DEV_WIRE_NONE) {
            return DEV_WIRE_ERR;
        }
        return writeCommand(CTRL_CMD_AHB_CLOCK_GATING);

    }

    int disableLockingMechanism()
    {
        disableSyncSampleMode();
        if (writeRegister(QMI8658_REG_CAL1_L, 0x00) != DEV_WIRE_NONE) {
            return DEV_WIRE_ERR;
        }
        return writeCommand(CTRL_CMD_AHB_CLOCK_GATING);
    }


    void dumpCtrlRegister()
    {
        uint8_t buffer[9];
        readRegister(QMI8658_REG_CTRL1, buffer, 9);
        for (int i = 0; i < 9; ++i) {
#if defined(ARDUINO)
            Serial.printf("CTRL%d: REG:0x%02X HEX:0x%02X\n", i + 1, QMI8658_REG_CTRL1 + i, buffer[i]);
#else
            printf("CTRL%d: 0x%02x\n", i + 1, buffer[i]);
#endif
        }

        buffer[0] =  readRegister(QMI8658_REG_FIFO_CTRL);
#if defined(ARDUINO)
        Serial.printf("FIFO_CTRL: REG:0x%02X HEX:0x%02X\n",  QMI8658_REG_FIFO_CTRL, buffer[0]);
#else
        printf("FIFO_CTRL: REG:0x%02X HEX:0x%02X\n",  QMI8658_REG_FIFO_CTRL, buffer[0]);
#endif

    }

    void powerDown()
    {
        disableAccelerometer();
        disableGyroscope();
        setRegisterBit(QMI8658_REG_CTRL1, 1);
    }

    void powerOn()
    {
        clrRegisterBit(QMI8658_REG_CTRL1, 1);
    }

    int getStatusRegister()
    {
        return readRegister(QMI8658_REG_STATUS1);
    }

    int configActivityInterruptMap(IntPin pin)
    {
        return pin == IntPin1 ? setRegisterBit(QMI8658_REG_CTRL8, 6)
               : clrRegisterBit(QMI8658_REG_CTRL8, 6);
    }

    /**
     * @brief configPedometer
     * @note The calculation of the Pedometer Detection is based on the accelerometer ODR defined by CTRL2.aODR,
     *      refer to Table 22 for details.
     * @param  ped_sample_cnt: Indicates the count of sample batch/window for calculation
     * @param  ped_fix_peak2peak:Indicates the threshold of the valid peak-to-peak detection
     *                          E.g., 0x00CC means 200mg
     * @param  ped_fix_peak:Indicates the threshold of the peak detection comparing to average
     *                      E.g., 0x0066 means 100mg
     * @param  ped_time_up:Indicates the maximum duration (timeout window) for a step.
     *                     Reset counting calculation if no peaks detected within this duration.
     *                    E.g., 80 means 1.6s @ ODR = 50Hz
     * @param  ped_time_low:Indicates the minimum duration for a step.
     *                     The peaks detected within this duration (quiet time) is ignored.
     *                    E.g., 12 means 0.25s @ ODR = 50Hz
     * @param  ped_time_cnt_entry:Indicates the minimum continuous steps to start the valid step counting.
     *                           If the continuously detected steps is lower than this count and timeout,
     *                           the steps will not be take into account;
     *                           if yes, the detected steps will all be taken into account and
     *                           counting is started to count every following step before timeout.
     *                           This is useful to screen out the fake steps detected by non-step vibrations
     *                           The timeout duration is defined by ped_time_up.
     *                           E.g., 10 means 10 steps entry count
     * @param  ped_fix_precision:0 is recommended
     * @param  ped_sig_count: The amount of steps when to update the pedometer output registers.
     *                      E.g., ped_sig_count = 4, every 4 valid steps is detected, update the registers once (added by 4).
     * @retval
     */
    int configPedometer(uint16_t ped_sample_cnt, uint16_t ped_fix_peak2peak, uint16_t ped_fix_peak,
                        uint16_t ped_time_up, uint8_t ped_time_low = 0x14, uint8_t ped_time_cnt_entry = 0x0A, uint8_t ped_fix_precision = 0x00,
                        uint8_t ped_sig_count = 0x04)
    {
        // The Pedometer can only work in Non-SyncSample mode
        disableSyncSampleMode();

        bool enGyro = isEnableGyroscope();
        bool enAccel = isEnableAccelerometer();

        if (enGyro) {
            disableGyroscope();
        }

        if (enAccel) {
            disableAccelerometer();
        }

        writeRegister(QMI8658_REG_CAL1_L, ped_sample_cnt & 0xFF);
        writeRegister(QMI8658_REG_CAL1_H, (ped_sample_cnt >> 8) & 0xFF);
        writeRegister(QMI8658_REG_CAL2_L, ped_fix_peak2peak & 0xFF);
        writeRegister(QMI8658_REG_CAL2_H, (ped_fix_peak2peak >> 8) & 0xFF);
        writeRegister(QMI8658_REG_CAL3_L, ped_fix_peak & 0xFF);
        writeRegister(QMI8658_REG_CAL3_H, (ped_fix_peak >> 8) & 0xFF);
        writeRegister(QMI8658_REG_CAL4_H, 0x01);
        writeRegister(QMI8658_REG_CAL4_L, 0x02);

        writeCommand(CTRL_CMD_CONFIGURE_PEDOMETER);

        writeRegister(QMI8658_REG_CAL1_L, ped_time_up & 0xFF);
        writeRegister(QMI8658_REG_CAL1_H, (ped_time_up >> 8) & 0xFF);
        writeRegister(QMI8658_REG_CAL2_L, ped_time_low);
        writeRegister(QMI8658_REG_CAL2_H, ped_time_cnt_entry);
        writeRegister(QMI8658_REG_CAL3_L, ped_fix_precision);
        writeRegister(QMI8658_REG_CAL3_H, ped_sig_count);
        writeRegister(QMI8658_REG_CAL4_H, 0x02);
        writeRegister(QMI8658_REG_CAL4_L, 0x02);

        writeCommand(CTRL_CMD_CONFIGURE_PEDOMETER);

        if (enGyro) {
            enableGyroscope();
        }

        if (enAccel) {
            enableAccelerometer();
        }
        return 0;
    }

    uint32_t getPedometerCounter()
    {
        uint8_t buffer[3];
        if (readRegister(QMI8658_REG_STEP_CNT_LOW, buffer, 3) != DEV_WIRE_ERR) {
            return (uint32_t)(((uint32_t)buffer[2] << 16) | ((uint32_t)buffer[1] << 8) | buffer[0]);
        }
        return 0;
    }

    int clearPedometerCounter()
    {
        return writeCommand(CTRL_CMD_RESET_PEDOMETER);
    }

    // The Pedometer can only work in Non-SyncSample mode
    int enablePedometer()
    {
        return setRegisterBit(QMI8658_REG_CTRL8, 4);
    }

    int disablePedometer()
    {
        return clrRegisterBit(QMI8658_REG_CTRL8, 4);
    }



    /**
     * @brief   configTap
     * @note    The calculation of the Tap Detection is based on the accelerometer ODR defined by CTRL2.aODR, refer to Table 22 for details.
     * @param  priority: Priority definition between the x, y, z axes of acceleration. Only
                         Priority[2:0] bits are used.
                         The axis that output the first peak of Linear Acceleration in a valid
                         Tap detection, will be consider as the Tap axis. However, there is
                         possibility that two or three of the axes shows same Linear
                         Acceleration at exactly same time when reach (or be higher than)
                         the PeakMagThr. In this case, the defined priority is used to judge
                         and select the axis as Tap axis.
     * @param  peakWindow:Defines the maximum duration (in sample) for a valid peak. In a
                        valid peak, the linear acceleration should reach or be higher than
                        the PeakMagThr and should return to quiet (no significant
                        movement) within UDMThr, at the end of PeakWindow.
                        E.g., 20 @500Hz ODR
     * @param  tapWindow:Defines the minimum quiet time before the second Tap happen.
                        After the first Tap is detected, there should be no significant
                        movement (defined by UDMThr) during the TapWindow. The valid
                        second tap should be detected after TapWindow and before
                        DTapWindow.
                        E.g., 50 @500Hz ODR
     * @param  dTapWindow:Defines the maximum time for a valid second Tap for Double Tap,
                        count start from the first peak of the valid first Tap.
                        E.g., 250 @500Hz ODR
     * @param  alpha:Defines the ratio for calculation the average of the acceleration.
                    The bigger of Alpha, the bigger weight of the latest data.
                    E.g., 0.0625
     * @param  gamma:Defines the ratio for calculating the average of the movement
                    magnitude. The bigger of Gamma, the bigger weight of the latest
                    data.
                    E.g., 0.25
     * @param  peakMagThr:Threshold for peak detection.
                        E.g, 0.8g2 (0x0320)
     * @param  UDMThr:Undefined Motion threshold. This defines the threshold of the
                    Linear Acceleration for quiet status.
                    E.g., 0.4g2 (0x0190)
     * @retval
     */
    int configTap(uint8_t priority, uint8_t peakWindow, uint16_t tapWindow, uint16_t dTapWindow,
                  uint8_t alpha, uint8_t gamma, uint16_t peakMagThr, uint16_t UDMThr)
    {
        bool enGyro = isEnableGyroscope();
        bool enAccel = isEnableAccelerometer();

        if (enGyro) {
            disableGyroscope();
        }

        if (enAccel) {
            disableAccelerometer();
        }
        writeRegister(QMI8658_REG_CAL1_L, peakWindow);
        writeRegister(QMI8658_REG_CAL1_H, priority);
        writeRegister(QMI8658_REG_CAL2_L, tapWindow & 0xFF);
        writeRegister(QMI8658_REG_CAL2_H, (tapWindow >> 8) & 0xFF);
        writeRegister(QMI8658_REG_CAL3_L, dTapWindow & 0xFF);
        writeRegister(QMI8658_REG_CAL3_H, (dTapWindow >> 8) & 0xFF);
        // writeRegister(QMI8658_REG_CAL4_L, 0x02);
        writeRegister(QMI8658_REG_CAL4_H, 0x01);

        writeCommand(CTRL_CMD_CONFIGURE_TAP);

        writeRegister(QMI8658_REG_CAL1_L, alpha);
        writeRegister(QMI8658_REG_CAL1_H, gamma);
        writeRegister(QMI8658_REG_CAL2_L, peakMagThr & 0xFF);
        writeRegister(QMI8658_REG_CAL2_H, (peakMagThr >> 8) & 0xFF);
        writeRegister(QMI8658_REG_CAL3_L, UDMThr & 0xFF);
        writeRegister(QMI8658_REG_CAL3_H, (UDMThr >> 8) & 0xFF);
        // writeRegister(QMI8658_REG_CAL4_L, 0x02);
        writeRegister(QMI8658_REG_CAL4_H, 0x02);

        writeCommand(CTRL_CMD_CONFIGURE_TAP);

        if (enGyro) {
            enableGyroscope();
        }

        if (enAccel) {
            enableAccelerometer();
        }

        return 0;
    }

    int enableTap()
    {
        return setRegisterBit(QMI8658_REG_CTRL8, 0);
    }

    int disableTap()
    {
        return clrRegisterBit(QMI8658_REG_CTRL8, 0);
    }

    void getTapStatus()
    {
        int val = readRegister(QMI8658_REG_TAP_STATUS);
        if (val & _BV(7)) {
            log_i("Tap was detected on the negative direction of the Tap axis");
        } else {
            log_i("Tap was detected on the positive direction of the Tap axis");
        }
        uint8_t t = (val >> 4) & 0x03;
        switch (t) {
        case 0:
            log_i("No Tap was detected");
            break;
        case 1:
            log_i("Tap was detected on X axis");
            break;
        case 2:
            log_i("Tap was detected on Y axis");
            break;
        case 3:
            log_i("Tap was detected on Z axis");
            break;

        default:
            break;
        }
        t = val & 0x03;
        switch (t) {
        case 0:
            log_i("No Tap was detected");
            break;
        case 1:
            log_i("Single-Tap was detected");
            break;
        case 2:
            log_i("Double-Tap was detected");
            break;
        default:
            break;
        }
    }


    /**
     * @brief
     * @note
     * @param  AnyMotionXThr:
     * @param  AnyMotionYThr:
     * @param  AnyMotionZThr:
     * @param  NoMotionXThr:
     * @param  NoMotionYThr:
     * @param  NoMotionZThr:
     * @param  modeCtrl:
     * @param  AnyMotionWindow:
     * @param  NoMotionWindow:
     * @param  SigMotionWaitWindow:
     * @param  SigMotionConfirmWindow:
     * @retval
     */
    int configMotion(uint8_t AnyMotionXThr, uint8_t AnyMotionYThr, uint8_t AnyMotionZThr,
                     uint8_t NoMotionXThr, uint8_t NoMotionYThr, uint8_t NoMotionZThr, uint8_t modeCtrl,
                     uint8_t AnyMotionWindow, uint8_t NoMotionWindow,
                     uint16_t SigMotionWaitWindow, uint16_t SigMotionConfirmWindow)
    {
        bool enGyro = isEnableGyroscope();
        bool enAccel = isEnableAccelerometer();

        if (enGyro) {
            disableGyroscope();
        }

        if (enAccel) {
            disableAccelerometer();
        }
        writeRegister(QMI8658_REG_CAL1_L, AnyMotionXThr);
        writeRegister(QMI8658_REG_CAL1_H, AnyMotionYThr);
        writeRegister(QMI8658_REG_CAL2_L, AnyMotionZThr);
        writeRegister(QMI8658_REG_CAL2_H, NoMotionXThr);
        writeRegister(QMI8658_REG_CAL3_L, NoMotionYThr);
        writeRegister(QMI8658_REG_CAL3_H, NoMotionZThr);
        writeRegister(QMI8658_REG_CAL4_L, modeCtrl);
        writeRegister(QMI8658_REG_CAL4_H, 0x01);

        writeCommand(CTRL_CMD_CONFIGURE_MOTION);

        writeRegister(QMI8658_REG_CAL1_L, AnyMotionWindow);
        writeRegister(QMI8658_REG_CAL1_H, NoMotionWindow);
        writeRegister(QMI8658_REG_CAL2_L, SigMotionWaitWindow & 0xFF);
        writeRegister(QMI8658_REG_CAL2_H, (SigMotionWaitWindow >> 8) & 0xFF);
        writeRegister(QMI8658_REG_CAL3_L, SigMotionConfirmWindow & 0xFF);
        writeRegister(QMI8658_REG_CAL3_H, (SigMotionConfirmWindow >> 8) & 0xFF);
        // writeRegister(QMI8658_REG_CAL4_L, 0x02);
        writeRegister(QMI8658_REG_CAL4_H, 0x02);

        writeCommand(CTRL_CMD_CONFIGURE_MOTION);

        if (enGyro) {
            enableGyroscope();
        }

        if (enAccel) {
            enableAccelerometer();
        }
        return 0;
    }

    int enableMotionDetect()
    {
        setRegisterBit(QMI8658_REG_CTRL8, 1);
        setRegisterBit(QMI8658_REG_CTRL8, 3);
        return setRegisterBit(QMI8658_REG_CTRL8, 2);
    }

    int disableMotionDetect()
    {
        return clrRegisterBit(QMI8658_REG_CTRL8, 2);
    }


    /**
     * @brief  configWakeOnMotion
     * @note   Configuring Wom will reset the sensor, set the function to Wom, and there will be no data output
     * @param  WoMThreshold: Resolution = 1mg ,default 200g
     * @param  odr: Accelerometer output data rate  ,default low power 128Hz
     * @param  pin: Interrupt Pin( 1 or 2 ) ,default use pin2
     * @param  defaultPinValue: WoM Interrupt Initial Value select: ,default pin high
     *  01 – INT2 (with initial value 0)
     *  11 – INT2 (with initial value 1)
     *  00 – INT1 (with initial value 0)
     *  10 – INT1 (with initial value 1)
     * @param  blankingTime: Interrupt Blanking Time
     *  (in number of accelerometer samples), the
     *  number of consecutive samples that will be ignored after
     *  enabling the WoM, to screen out unwanted fake detection
     * @retval
     */
    int configWakeOnMotion(uint8_t WoMThreshold = 200,
                           AccelODR odr = ACC_ODR_LOWPOWER_128Hz,
                           IntPin pin = IntPin2,
                           uint8_t defaultPinValue = 1,
                           uint8_t blankingTime = 0x20
                          )
    {

        uint8_t val = 0;

        //Reset default value
        if (!reset()) {
            return DEV_WIRE_ERR;
        }

        // Disable sensors
        clrRegisterBit(QMI8658_REG_CTRL7, 0);

        //setAccelRange
        if (writeRegister(QMI8658_REG_CTRL2, 0x8F, (ACC_RANGE_8G << 4)) != DEV_WIRE_NONE) {
            return DEV_WIRE_ERR;
        }

        // setAccelOutputDataRate
        if (writeRegister(QMI8658_REG_CTRL2, 0xF0, odr) != DEV_WIRE_NONE) {
            return DEV_WIRE_ERR;
        }

        //set wom
        if (writeRegister(QMI8658_REG_CAL1_L, WoMThreshold) != DEV_WIRE_NONE) {
            return DEV_WIRE_ERR;
        }

        if ( pin == IntPin1) {
            val = defaultPinValue ? 0x02 : 0x00;
        } else if (pin == IntPin2) {
            val = defaultPinValue ? 0x03 : 0x01;
        }

        val <<= 6;
        val |= (blankingTime & 0x3F);
        if (writeRegister(QMI8658_REG_CAL1_H, val) != DEV_WIRE_NONE) {
            return DEV_WIRE_ERR;
        }

        if (writeCommand(CTRL_CMD_WRITE_WOM_SETTING) != DEV_WIRE_NONE) {
            return DEV_WIRE_ERR;
        }

        enableAccelerometer();

        enableINT(pin);

        return DEV_WIRE_NONE;
    }



    void getChipUsid(uint8_t *buffer, uint8_t length)
    {
        if (length > 6) {
            length = 6;
        }
        memcpy(buffer, usid, length);
    }


    uint32_t getChipFirmwareVersion()
    {
        return revisionID;
    }

    /**
     * @brief readSensorStatus
     * @note  Get the interrupt status and status 0, status 1 of the sensor
     * @retval  Return SensorStatus
     */
    uint16_t readSensorStatus()
    {
        uint16_t result = 0;
        // STATUSINT 0x2D
        // STATUS0 0x2E
        // STATUS1 0x2F
        uint8_t status[3];
        if (readRegister(QMI8658_REG_STATUS_INT, status, 3) != DEV_WIRE_NONE) {
            return 0;
        }

        // log_i("STATUSINT:0x%X BIN:", status[0]);
        // log_i("STATUS0:0x%X BIN:", status[1]);
        // log_i("STATUS1:0x%X BIN:", status[2]);
        // log_i("------------------\n");

        // Ctrl9 CmdDone
        // Indicates CTRL9 Command was done, as part of CTRL9 protocol
        // 0: Not Completed
        // 1: Done
        if (status[0] & 0x80) {
            result |= STATUS_INT_CTRL9_CMD_DONE;
        }
        // If syncSample (CTRL7.bit7) = 1:
        //      0: Sensor Data is not locked.
        //      1: Sensor Data is locked.
        // If syncSample = 0, this bit shows the same value of INT1 level
        if (status[0] & 0x02) {
            result |= STATUS_INT_LOCKED;
        }
        // If syncSample (CTRL7.bit7) = 1:
        //      0: Sensor Data is not available
        //      1: Sensor Data is available for reading
        // If syncSample = 0, this bit shows the same value of INT2 level
        if (status[0] & 0x01) {
            result |= STATUS_INT_AVAIL;
            // if (eventGyroDataReady)eventGyroDataReady();
            // if (eventAccelDataReady)eventAccelDataReady();
        }

        //Locking Mechanism Can reading..
        if ((status[0] & 0x03) == 0x03) {
            if (eventDataLocking)eventDataLocking();
        }

        //=======================================
        // Valid only in asynchronous mode
        if (sampleMode == ASYNC_MODE) {
            // Gyroscope new data available
            // 0: No updates since last read.
            // 1: New data available
            if (status[1] & 0x02) {
                result |= STATUS0_GYRO_DATA_READY;
                if (eventGyroDataReady)eventGyroDataReady();
                __gDataReady = true;
            }
            // Accelerometer new data available
            // 0: No updates since last read.
            // 1: New data available.
            if (status[1] & 0x01) {
                result |= STATUS0_ACCEL_DATA_READY;
                if (eventAccelDataReady)eventAccelDataReady();
                __aDataReady = true;
            }
        }

        //=======================================
        // Significant Motion
        // 0: No Significant-Motion was detected
        // 1: Significant-Motion was detected
        if (status[2] & 0x80) {
            result |= STATUS1_SIGNIFICANT_MOTION;
            if (eventSignificantMotion)eventSignificantMotion();
        }
        // No Motion
        // 0: No No-Motion was detected
        // 1: No-Motion was detected
        if (status[2] & 0x40) {
            result |= STATUS1_NO_MOTION;
            if (eventNoMotionEvent)eventNoMotionEvent();
        }
        // Any Motion
        // 0: No Any-Motion was detected
        // 1: Any-Motion was detected
        if (status[2] & 0x20) {
            result |= STATUS1_ANY_MOTION;
            if (eventAnyMotionEvent)eventAnyMotionEvent();
        }
        // Pedometer
        // 0: No step was detected
        // 1: step was detected
        if (status[2] & 0x10) {
            result |= STATUS1_PEDOMETER_MOTION;
            if (eventPedometerEvent)eventPedometerEvent();
        }
        // WoM
        // 0: No WoM was detected
        // 1: WoM was detected
        if (status[2] & 0x04) {
            result |= STATUS1_WOM_MOTION;
            if (eventWomEvent)eventWomEvent();
        }
        // TAP
        // 0: No Tap was detected
        // 1: Tap was detected
        if (status[2] & 0x02) {
            result |= STATUS1_TAP_MOTION;
            if (eventTagEvent)eventTagEvent();
        }
        return result;
    }

    void setWakeupMotionEventCallBack(EventCallBack_t cb)
    {
        eventWomEvent = cb;
    }

    void setTapEventCallBack(EventCallBack_t cb)
    {
        eventTagEvent = cb;
    }

    void setPedometerEventCallBack(EventCallBack_t cb)
    {
        eventPedometerEvent = cb;
    }

    void setNoMotionEventCallBack(EventCallBack_t cb)
    {
        eventNoMotionEvent = cb;
    }

    void setAnyMotionEventCallBack(EventCallBack_t cb)
    {
        eventAnyMotionEvent = cb;
    }

    void setSignificantMotionEventCallBack(EventCallBack_t cb)
    {
        eventSignificantMotion = cb;
    }

    void setGyroDataReadyCallBack(EventCallBack_t cb)
    {
        eventGyroDataReady = cb;
    }

    void setAccelDataReadyEventCallBack(EventCallBack_t cb)
    {
        eventAccelDataReady = cb;
    }

    void setDataLockingEventCallBack(EventCallBack_t cb)
    {
        eventDataLocking = cb;
    }


    bool calibration(uint16_t *gX_gain = NULL, uint16_t *gY_gain = NULL, uint16_t *gZ_gain = NULL)
    {
        // 1.Set CTRL7.aEN = 0 and CTRL7.gEN = 0, to disable the accelerometer and gyroscope.
        if (writeRegister(QMI8658_REG_CTRL7, 0x00) != DEV_WIRE_NONE) {
            return false;
        }

        // 2.Issue the CTRL_CMD_ON_DEMAND_CALIBRATION (0xA2) by CTRL9 command.
        if (writeCommand(CTRL_CMD_ON_DEMAND_CALIBRATION, 3000) != DEV_WIRE_NONE) {
            return false;
        }

        // 3.And wait about 1.5 seconds for QMI8658A to finish the CTRL9 command.
        delay(1600);

        // 4.Read the COD_STATUS register (0x46) to check the result/status of the COD implementation.
        int result =  readRegister(QMI8658_REG_COD_STATUS);

        if (result == DEV_WIRE_ERR)return false;

        // During the process, it is recommended to place the device in quiet, otherwise, the COD might fail and report error.

        if (result & _BV(7)) {
            log_e("COD failed for checking low sensitivity limit of X axis of gyroscope");
            return false;
        }
        if (result & _BV(6)) {
            log_e("COD failed for checking high sensitivity limit of X axis of gyroscope");
            return false;
        }
        if (result & _BV(5)) {
            log_e("COD failed for checking low sensitivity limit of Y axis of gyroscope");
            return false;
        }
        if (result & _BV(4)) {
            log_e("COD failed for checking high sensitivity limit of Y axis of gyroscope");
            return false;
        }
        if (result & _BV(3)) {
            log_e("Accelerometer checked failed (significant vibration happened during COD)");
            return false;
        }
        if (result & _BV(2)) {
            log_e("Gyroscope startup failure happened when COD was called");
            return false;
        }
        if (result & _BV(1)) {
            log_e("COD was called while gyroscope was enabled, COD return failure");
            return false;
        }
        if (result & _BV(0)) {
            log_e("COD failed; no COD correction applied");
            return false;
        }
        log_d("All calibrations are completed");

        if (gX_gain && gY_gain && gZ_gain) {
            uint8_t rawBuffer[6] = {0};
            if (readRegister(QMI8658_REG_DVX_L, rawBuffer, 6) != DEV_WIRE_NONE) {
                return false;
            }
            *gX_gain = ((uint16_t)rawBuffer[0]) | (uint16_t)(rawBuffer[1] << 8);
            *gY_gain = ((uint16_t)rawBuffer[2]) | (uint16_t)(rawBuffer[3] << 8);
            *gZ_gain = ((uint16_t)rawBuffer[4]) | (uint16_t)(rawBuffer[5] << 8);
        }

        return true;
    }


    bool writeCalibration(uint16_t gX_gain, uint16_t gY_gain, uint16_t gZ_gain)
    {
        // 1. Disable Accelerometer and Gyroscope by setting CTRL7.aEN = 0 and CTRL7.gEN = 0
        if (writeRegister(QMI8658_REG_CTRL7, 0x00) != DEV_WIRE_NONE) {
            return false;
        }

        uint8_t buffer[] = {

            // 2. write Gyro-X gain (16 bits) to registers CAL1_L and CAL1_H registers (0x0B, 0x0C)
            lowByte(gX_gain),
            highByte(gX_gain),
            // 3. write Gyro-Y gain (16 bits) to registers CAL2_L and CAL2_H registers (0x0D, 0x0E)
            lowByte(gY_gain),
            highByte(gY_gain),
            // 4. write Gyro-Z gain (16 bits) to registers CAL3_L and CAL3_H registers (0x0F, 0x10)
            lowByte(gZ_gain),
            highByte(gZ_gain),
        };

        writeRegister(QMI8658_REG_CAL1_L, buffer, sizeof(buffer));

        // 5. Write 0xAA to CTRL9 and follow CTRL9 protocol
        if (writeCommand(CTRL_CMD_APPLY_GYRO_GAINS, 3000) != DEV_WIRE_NONE) {
            return false;
        }

        return true;
    }


private:
    float accelScales, gyroScales;
    uint32_t lastTimestamp = 0;
    uint8_t sampleMode = ASYNC_MODE;
    bool accelEn, gyroEn;
    uint8_t fifoMode;
    uint32_t revisionID;
    uint8_t  usid[6];
    bool __gDataReady = false;
    bool __aDataReady = false;

    EventCallBack_t eventWomEvent = NULL;
    EventCallBack_t eventTagEvent = NULL;
    EventCallBack_t eventPedometerEvent = NULL;
    EventCallBack_t eventNoMotionEvent = NULL;
    EventCallBack_t eventAnyMotionEvent = NULL;
    EventCallBack_t eventSignificantMotion = NULL;
    EventCallBack_t eventGyroDataReady = NULL;
    EventCallBack_t eventAccelDataReady = NULL;
    EventCallBack_t eventDataLocking = NULL;


    int writeCommand(CommandTable cmd, uint32_t wait_ms = 1000)
    {
        int      val;
        uint32_t startMillis;
        if (writeRegister(QMI8658_REG_CTRL9, cmd) == DEV_WIRE_ERR) {
            return DEV_WIRE_ERR;
        }
        startMillis = millis();
        do {
            val = readRegister(QMI8658_REG_STATUS_INT);
            delay(1);
            if (millis() - startMillis > wait_ms) {
                log_e("wait for ctrl9 command done time out : %d val:%d", cmd, val);
                return DEV_WIRE_TIMEOUT;
            }
        } while (val != DEV_WIRE_ERR && !(val & 0x80));

        if (writeRegister(QMI8658_REG_CTRL9, CTRL_CMD_ACK) == DEV_WIRE_ERR) {
            return DEV_WIRE_ERR;
        }

        startMillis = millis();
        do {
            val = readRegister(QMI8658_REG_STATUS_INT);
            delay(1);
            if (millis() - startMillis > wait_ms) {
                log_e("Clear ctrl9 command done flag timeout : %d val:%d", cmd, val);
                return DEV_WIRE_TIMEOUT;
            }
        } while (val != DEV_WIRE_ERR && (val & 0x80));

        return DEV_WIRE_NONE;
    }

protected:

    bool initImpl()
    {
        uint8_t buffer[6] = {0};

        if (!reset()) {
            return false;
        }

        uint8_t id = whoAmI();
        if (id != QMI8658_REG_WHOAMI_DEFAULT) {
            log_e("ERROR! ID NOT MATCH QMI8658 , Response id is 0x%x", id);
            return false;
        }
        // Enable address auto increment, Big-Endian format
        // writeRegister(QMI8658_REG_CTRL1, 0x60);

        // Little-Endian / address auto increment
        // writeRegister(QMI8658_REG_CTRL1, 0x40);

        // no need . reset function has set
        //EN.ADDR_AI
        // setRegisterBit(QMI8658_REG_CTRL1, 6);

        // Use STATUS_INT.bit7 as CTRL9 handshake
        writeRegister(QMI8658_REG_CTRL8, 0x80);

        // Get firmware version and usid
        writeCommand(CTRL_CMD_COPY_USID);

        if (readRegister(QMI8658_REG_DQW_L, buffer, 3) != DEV_WIRE_ERR) {
            revisionID = buffer[0] | (uint32_t)(buffer[1] << 8) | (uint32_t)(buffer[2] << 16);
            log_d("FW Version :0x%02X%02X%02X", buffer[0], buffer[1], buffer[2]);
        }

        if (readRegister(QMI8658_REG_DVX_L, usid, 6) != DEV_WIRE_ERR) {
            log_d("USID :%02X%02X%02X%02X%02X%02X",
                  usid[0], usid[1], usid[2],
                  usid[3], usid[4], usid[5]);
        }

        return true;
    }

    int getReadMaskImpl()
    {
        return 0x80;
    }

};









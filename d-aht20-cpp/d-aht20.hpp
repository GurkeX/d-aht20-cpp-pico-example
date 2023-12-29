/*
Aosong AHT20/DHT20 sensor library for Raspberry Pi Pico.

Version: 0.0.1
Copyright 2022 Sampsa Penna, Kimi Malkamäki
*/

#ifndef PICO_DHT20_DEFINED
#define PICO_DHT20_DEFINED

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdint.h>
#include "pico/binary_info.h"

#define DHT20_OK 0
#define DHT20_ERROR_CHECKSUM -10
#define DHT20_ERROR_CONNECT -11
#define DHT20_MISSING_BYTES -12
#define DHT20_ERROR_BYTES_ALL_ZERO -13
#define DHT20_ERROR_READ_TIMEOUT -14
#define DHT20_ERROR_LASTREAD -15
#define DHT20_ERROR_NORESET -16
#define DHT20_ERROR_BUSY -17

#ifdef USE_I2C_1
#define I2C_INST i2c1
#else
#define I2C_INST i2c0
#endif

#define DHT20_ADDRESS 0x38

/*
Prep constant i2c messages in program memory
*/
// Resets
static const uint8_t __in_flash() rst_msg_1[3] = {0x1B, 0x00, 0x00};
static const uint8_t __in_flash() rst_msg_2[3] = {0x1C, 0x00, 0x00};
static const uint8_t __in_flash() rst_msg_3[3] = {0x1E, 0x00, 0x00};
// Trigger measurement
static const uint8_t __in_flash() trigger_measurement[3] = {0xAC, 0x33, 0x00};

class DHT20 {
public:
    DHT20(bool auto_init_i2c);
    //~DHT20();

    // Initialize the DHT sensor
    int DHT20_init();

    // Start taking a temperature measurement
    int startMeasurement();

    // Read the finished measurement
    int readMeasurement();

    // Convert the raw data to sensible values
    int convert();

    // Take a complete measurement from the sensor without external timing
    int getMeasurement();

    // Function that can be called every cycle in the loop. Function does not sleep
    int updateMeasurement();

    // Access the converted temperature & humidity
    float getHumidity();
    float getTemperature();

    // Offset getters and setters
    void setHumOffset(float offset);
    void setTempOffset(float offset);
    float getHumOffset();
    float getTempOffset();

    // Set updateInterval for the updateMeasurement function. This interval should be over 1000ms
    void setUpdateInterval(uint8_t time);

    // Check the sensor status word
    uint8_t readStatus();

    // Check sensor calibration status
    bool isCalibrated();

    // Check status from last completed read()
    int internalStatus();

    // Check time since last read() call
    uint32_t lastRead();

    // Check time since last measurement request
    uint32_t lastRequest();

    // Check if sensor is measuring
    bool isMeasuring();

    // Check if sensor is at idle
    bool isIdle();

private:
    float humidity;
    float temperature;
    float humOffset;
    float tempOffset;

    uint8_t status;
    uint32_t var_lastRequest;
    uint32_t var_lastRead;
    uint32_t updateInterval;
    uint8_t bytes[7];
    uint8_t crc;

    int I2C_SDA;
    int I2C_SCL;

    // Calculate checksum value
    static uint8_t _crc8(uint8_t *ptr, uint8_t len);

    // Reset sensor
    void resetSensor();

    // Check if sensor needs a reset
    bool needsReset();

    // init the i2c interface
    void init_i2c();
};

#endif
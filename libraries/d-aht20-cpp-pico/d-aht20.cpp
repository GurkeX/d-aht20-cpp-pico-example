/*
Aosong AHT20/DHT20 object oriented sensor library for Raspberry Pi Pico.
*/

#include <string.h>

#include "d-aht20.hpp"

DHT20::DHT20(bool auto_init_i2c) {
    // Set default i2c instance
    i2c_instance = i2c0;

    // Set default SDA and SCL pins
    this->I2C_SDA = 4;
    this->I2C_SCL = 5;

    // Set up the i2c interface
    if(auto_init_i2c) {
        init_i2c();
    }

    printf("DHT20 initialization\n");

    #ifndef DHT20_EXAMPLE_SKIP_INIT_SLEEP
        sleep_ms(2000);
    #endif
        // Set up temp and humid sensor
        printf("Initialize DHT20.\n");    
        int sensor_ret = DHT20_init();
        if (sensor_ret != DHT20_OK)
        {
            printf("Failed to initialize the sensor.\n");
            printf("Sensor return value %d\n", sensor_ret);
            return;
        }
        printf("Initialized DHT20.\n");

        // Sleep for 10ms after successful sensor initialization
        sleep_ms(10);
}

DHT20::DHT20(int I2C_SDA, int I2C_SCL, bool auto_init_i2c) {
    // Set default i2c instance
    i2c_instance = i2c0;

    // Set default SDA and SCL pins
    this->I2C_SDA = I2C_SDA;
    this->I2C_SCL = I2C_SCL;

    // Set up the i2c interface#
    if(auto_init_i2c) {
        init_i2c();
    }

    printf("DHT20 initialization\n");

    #ifndef DHT20_EXAMPLE_SKIP_INIT_SLEEP
        sleep_ms(2000);
    #endif
        // Set up temp and humid sensor
        printf("Initialize DHT20.\n");    
        int sensor_ret = DHT20_init();
        if (sensor_ret != DHT20_OK)
        {
            printf("Failed to initialize the sensor.\n");
            printf("Sensor return value %d\n", sensor_ret);
            return;
        }
        printf("Initialized DHT20.\n");

        // Sleep for 10ms after successful sensor initialization
        sleep_ms(10);
}


DHT20::DHT20(int I2C_SDA, int I2C_SCL, i2c_inst_t* i2c_instance, bool auto_init_i2c) {
    // Set i2c instance
    this->i2c_instance = i2c_instance;
    
    // Set default SDA and SCL pins
    this->I2C_SDA = I2C_SDA;
    this->I2C_SCL = I2C_SCL;

    // Set up the i2c interface#
    if(auto_init_i2c) {
        init_i2c();
    }

    printf("DHT20 initialization\n");

    #ifndef DHT20_EXAMPLE_SKIP_INIT_SLEEP
        sleep_ms(2000);
    #endif
        // Set up temp and humid sensor
        printf("Initialize DHT20.\n");    
        int sensor_ret = DHT20_init();
        if (sensor_ret != DHT20_OK)
        {
            printf("Failed to initialize the sensor.\n");
            printf("Sensor return value %d\n", sensor_ret);
            return;
        }
        printf("Initialized DHT20.\n");

        // Sleep for 10ms after successful sensor initialization
        sleep_ms(10);
}

int DHT20::DHT20_init() {
    this->humidity = 0;
    this->temperature = 0;
    this->humOffset = 0;
    this->tempOffset = 0;
    this->status = 0;
    this->var_lastRequest = 0;
    this->var_lastRead = 0;
    this->updateInterval = 1500;
    this->crc = 0;
    memset(this->bytes, 0, 7);

    // Ensure 100ms wait after powerup as per sensor datasheet
    sleep_ms(100);

    for (int i = 0; i < 3; i++)
    {
        resetSensor();
        sleep_ms(100);
        if (!needsReset())
        {
            return DHT20_OK;
        }
    }

    return DHT20_ERROR_NORESET;
}


void DHT20::init_i2c() {
    printf("Setting up i2c\n");
    i2c_init(i2c_instance, 100 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(I2C_SDA, I2C_SCL, GPIO_FUNC_I2C));
    printf("i2c setup successful.\n");
    
    return;
}

void DHT20::resetSensor() {
    if (needsReset())
    {
        i2c_write_blocking(i2c_instance, DHT20_ADDRESS, rst_msg_1, 3, false);
        i2c_write_blocking(i2c_instance, DHT20_ADDRESS, rst_msg_2, 3, false);
        i2c_write_blocking(i2c_instance, DHT20_ADDRESS, rst_msg_3, 3, false);
    }
}

bool DHT20::needsReset() {
    return ((readStatus() & 0x18) != 0x18);
}

uint8_t DHT20::readStatus() {
    uint8_t ret;
    i2c_read_blocking(i2c_instance, DHT20_ADDRESS, &ret, 1, false);
    return ret;
}

int DHT20::startMeasurement() {
    if (i2c_write_blocking(i2c_instance, DHT20_ADDRESS, trigger_measurement, 3, false) == PICO_ERROR_GENERIC)
    {
        return DHT20_ERROR_CONNECT;
    }
    return DHT20_OK;
}

int DHT20::readMeasurement() {
    if (i2c_read_blocking(i2c_instance, DHT20_ADDRESS, this->bytes, 7, false) == PICO_ERROR_GENERIC)
    {
        return DHT20_ERROR_CONNECT;
    }

    // Return busy if the measurements weren't ready yet
    if ((this->bytes[0] & 0x80) == 0x80)
    {
        return DHT20_ERROR_BUSY;
    }

    bool allZero = true;
    for (int i = 0; i < 7; i++)
    {
        if (this->bytes[i] > 0)
        {
            allZero = false;
            break;
        }
    }
    if (allZero)
    {
        return DHT20_ERROR_BYTES_ALL_ZERO;
    }

    this->var_lastRead = to_ms_since_boot(get_absolute_time());

    return DHT20_OK;
}

int DHT20::convert() {
    //  CONVERT AND STORE
    this->status = this->bytes[0];
    uint32_t raw = this->bytes[1];
    raw <<= 8;
    raw += this->bytes[2];
    raw <<= 4;
    raw += (this->bytes[3] >> 4);
    this->humidity = raw * 9.5367431640625e-5; // ==> / 1048576.0 * 100%;

    raw = (this->bytes[3] & 0x0F);
    raw <<= 8;
    raw += this->bytes[4];
    raw <<= 8;
    raw += this->bytes[5];
    this->temperature = raw * 1.9073486328125e-4 - 50; //  ==> / 1048576.0 * 200 - 50;

    //  TEST CHECKSUM
    this->crc = _crc8(this->bytes, 6);
    if (this->crc != this->bytes[6])
    {
        return DHT20_ERROR_CHECKSUM;
    }

    return DHT20_OK;
}

int DHT20::getMeasurement() {
    int status;

    if (to_ms_since_boot(get_absolute_time()) - this->var_lastRead < 1000)
    {
        return DHT20_ERROR_LASTREAD;
    }

    status = startMeasurement();
    if (status != DHT20_OK)
    {
        return status;
    }

    sleep_ms(50);

    for (int i = 5; i > 0; i--)
    {
        status = readMeasurement();
        if (status == DHT20_ERROR_BUSY)
        {
            sleep_ms(10);
            continue;
        }
    }

    if (status != DHT20_OK)
    {
        return status;
    }

    return convert();
}

// function that can be called every cycle in the loop. function does not sleep
int DHT20::updateMeasurement() {
    int status;
    if (to_ms_since_boot(get_absolute_time()) - this->var_lastRequest > this->updateInterval) {
        status=startMeasurement();
        this->var_lastRequest=to_ms_since_boot(get_absolute_time());
    }
    
    if (status != DHT20_OK)
    {
        return status;
    }

    if (to_ms_since_boot(get_absolute_time()) - this->var_lastRequest > 100){
        if (to_ms_since_boot(get_absolute_time()) - this->var_lastRead > 1000){
            status=readMeasurement();
            convert();
        }
    }

    if (status != DHT20_OK)
    {
        return status;
    }

    return DHT20_OK;
}

float DHT20::getHumidity() {
    return this->humidity + this->humOffset;
}

float DHT20::getTemperature() {
    return this->temperature + this->tempOffset;
}

void DHT20::setHumOffset(float offset) {
    this->humOffset = offset;
}

void DHT20::setTempOffset(float offset) {
    this->tempOffset = offset;
}

void DHT20::setUpdateInterval(uint8_t time) {
    this->updateInterval = time;
}

float DHT20::getHumOffset() {
    return this->humOffset;
}

float DHT20::getTempOffset() {
    return this->tempOffset;
}

bool DHT20::isCalibrated() {
    return (readStatus() & 0x08) == 0x08;
}

bool DHT20::isMeasuring() {
    return (readStatus() & 0x80) == 0x80;
}

bool DHT20::isIdle() {
    return (readStatus() & 0x80) == 0x00;
}

int DHT20::internalStatus() {
    return this->status;
}

uint32_t DHT20::lastRead() {
    return this->var_lastRead;
}

uint32_t DHT20::lastRequest() {
    return this->var_lastRequest;
}

uint8_t DHT20::_crc8(uint8_t *ptr, uint8_t len) {
    uint8_t crc = 0xFF;
    while (len--)
    {
        crc ^= *ptr++;
        for (uint8_t i = 0; i < 8; i++)
        {
            if (crc & 0x80)
            {
                crc <<= 1;
                crc ^= 0x31;
            }
            else
            {
                crc <<= 1;
            }
        }
    }
    return crc;
}
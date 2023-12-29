#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "d-aht20-cpp/d-aht20.hpp"

#define FAN_PIN 15
DHT20* my_sensor;

void fan_on(bool enable);
void sleep_seconds(int seconds);

int main()
{
    // Set up stdio
    stdio_init_all();
    puts("Running controller setup.");
    // Initialize DHT20 Sensor
    my_sensor = new DHT20(true);

    float upper_humidity_level = 75;
    float target_humidity = 20;

    while (1)
    {
        my_sensor->getMeasurement();
        if (my_sensor->getHumidity() > target_humidity) {
            fan_on(true);
            puts("Fan on\n");
            sleep_seconds(30);
            fan_on(false);
        }
        else {
            while (my_sensor->getHumidity() < upper_humidity_level) {
                my_sensor->getMeasurement();
                puts("Fan off\n");
                sleep_seconds(20);
            }
        }
        sleep_seconds(2);
    }
    return 0;
}

void sleep_seconds(int seconds) {
    sleep_ms(seconds * 1000);
    
    return;
}

void fan_on(bool enable) {
    gpio_init(FAN_PIN);
    gpio_set_dir(FAN_PIN, GPIO_OUT);

    if (enable) {
        gpio_put(FAN_PIN, 1); // Turn the fan on
        printf("Fan on\n");
    } else {
        gpio_put(FAN_PIN, 0); // Turn the fan off
        printf("Fan off\n");
    }

    return;
}
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/gpio.h"
#include "../libraries/d-aht20-cpp-pico/d-aht20.hpp"

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

    float target_humidity = 60;
    float upper_humidity_level = target_humidity + 10;

    while (1)
    {
        my_sensor->getMeasurement();
        if (my_sensor->getHumidity() > upper_humidity_level) {
            fan_on(true);
            puts("Humidity above upper limit, fan is turned on\n");
            sleep_seconds(10);
        }
        else if (my_sensor->getHumidity() < target_humidity){
            fan_on(false);
            puts("Humidity target reached, fan is turned off");
            sleep_seconds(10);
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
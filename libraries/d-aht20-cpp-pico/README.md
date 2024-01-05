# Aosong AHT20/DHT20 – Raspberry Pico SDK port
This is a cpp library for driving the Aosong DHT20/AHT20 sensor, sold by multiple
3rd parties like Adafruit and Seeedstudio. The sensor is connected to using
i2c.

The only dependency for this library is `pico-sdk`, but since it will be
imported by whatever project uses this library as a dependency, it's not
necessary to add as a submodule separately here.

The library is based on [Aosong AHT20/DHT20 – Raspberry Pico SDK port](https://github.com/sampsapenna/dht20-pico), which has been rewritten into an objectoriented cpp library

An example pico program is provided in [DHT20 CPP Pico example repository.](https://github.com/GurkeX/D-AHT20-CPP-Pico-example)

## Usage
The port can be easily integrated into a Pico SDK project as a git submodule.
```bash
git submodule add https://github.com/GurkeX/d-aht20-cpp-pico.git
git submodule update
```
After cloning the repository you need to add the library to `CMakeLists.txt`
used when building the project. Add the following lines before `target_link_libraries`:
```cmake
add_subdirectory("d-aht20-cpp-pico")

target_include_directories(d-aht20cpp PUBLIC "d-aht20-cpp-pico")
```

Additionally you will need to add the required libraries to linking, by
adding `hardware_i2c dht20 pico_binary_info` to `target_link_libraries`.

Libary is able to initialize i2c, when initializing with
```
DHT20 my_sensor(true);
```

Header file to include is `d-aht20.hpp`.

## Bugs
Sensor has been tested for normal operation. DHT20 has not been tested, but
should work just as well as the AHT20 variant, since the only difference
between the two is packaging AFAIK.

If you encounter a bug, go ahead and open an issue.

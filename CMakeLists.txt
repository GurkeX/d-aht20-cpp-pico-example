# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "/opt/pico-sdk")

set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(temp_humid_sensor C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(temp_humid_sensor src/main.cpp)

pico_set_program_name(temp_humid_sensor "temp_humid_sensor")
pico_set_program_version(temp_humid_sensor "0.1")

pico_enable_stdio_uart(temp_humid_sensor 1)
pico_enable_stdio_usb(temp_humid_sensor 0)


add_subdirectory("libraries/d-aht20-cpp-pico")
target_include_directories(d-aht20cpp PUBLIC "d-aht20-cpp-pico")


# Add the standard library to the build
target_link_libraries(temp_humid_sensor
        pico_stdlib
        hardware_i2c
        d-aht20cpp
        pico_binary_info
        )

# Add the standard include files to the build
target_include_directories(temp_humid_sensor PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

# Add any user requested libraries
target_link_libraries(temp_humid_sensor 
        pico_cyw43_arch_none
        )

pico_add_extra_outputs(temp_humid_sensor)


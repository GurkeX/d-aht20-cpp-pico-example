add_library(d-aht20cpp
    STATIC
    d-aht20.cpp
)

target_link_libraries(d-aht20cpp 
                      pico_stdlib 
                      hardware_i2c
                      pico_binary_info)

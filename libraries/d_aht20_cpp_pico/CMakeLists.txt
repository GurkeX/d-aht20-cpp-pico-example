add_library(d_aht20cpp
    STATIC
    d_aht20.cpp
)

target_link_libraries(d_aht20cpp 
                      pico_stdlib 
                      hardware_i2c
                      pico_binary_info)

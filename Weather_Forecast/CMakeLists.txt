cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(weather_forecast_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(weather_forecast
    weather_forecast.c        
)

pico_enable_stdio_usb(weather_forecast 1)
pico_enable_stdio_uart(weather_forecast 0)

pico_add_extra_outputs(weather_forecast)

target_link_libraries(weather_forecast pico_stdlib hardware_i2c)
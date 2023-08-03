cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(weather_forecast_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(LCD
    LCD.c        
)

target_sources(LCD PUBLIC
	${CMAKE_CURRENT_LIST_DIR}
)

target_include_directories(LCD PUBLIC
	${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(LCD PUBLIC 
                    pico_stdlib 
                    hardware_i2c
                    )

pico_enable_stdio_usb(LCD 1)
pico_enable_stdio_uart(LCD 0)

pico_add_extra_outputs(LCD)



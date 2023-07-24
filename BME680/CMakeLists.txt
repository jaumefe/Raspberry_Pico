cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(weather_forecast_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(main
    main.c        
)

target_sources(main PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/BME680.c
)

target_include_directories(main PUBLIC
	${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(main PUBLIC 
                    pico_stdlib 
                    hardware_i2c
                    )

pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)

pico_add_extra_outputs(main)



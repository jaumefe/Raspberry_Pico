cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include($ENV{FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

project(weather_forecast_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_library(tinycbor STATIC IMPORTED)

set_target_properties(tinycbor PROPERTIES
	IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/libtinycbor.a
)

target_include_directories(tinycbor INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib
)


add_executable(weather_forecast
	src/weather_forecast.c  
	src/modules/usb_cdc.c
	src/modules/DPS310.c
	src/modules/SHT4x.c
	src/modules/BME680.c
	src/modules/led.c     
)

target_include_directories(weather_forecast PUBLIC
	"${CMAKE_CURRENT_DIR}/src"
	"${CMAKE_CURRENT_DIR}/src/modules"
	"${CMAKE_SOURCE_DIR}/src/core"
	"${CMAKE_SOURCE_DIR}/src/lib"
)

target_link_libraries(weather_forecast PUBLIC
	pico_stdlib
	hardware_i2c
	hardware_gpio
	FreeRTOS-Kernel
	FreeRTOS-Kernel-Heap4
	tinycbor
)


# CFLAGs quirks
set_source_files_properties(${PICO_SDK_PATH}/src/rp2_common/hardware_flash/flash.c PROPERTIES COMPILE_FLAGS "-Wno-shadow")

pico_enable_stdio_usb(weather_forecast 1)
pico_enable_stdio_uart(weather_forecast 0)

pico_add_extra_outputs(weather_forecast)


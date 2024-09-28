cmake_minimum_required(VERSION 3.13)

set(ENV{PICO_SDK_PATH} "${CMAKE_SOURCE_DIR}/../external/pico-sdk")

include(pico_sdk_import.cmake)

project(weather_forecast_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(weather_forecast
	src/weather_forecast.c       
)

target_sources(weather_forecast PUBLIC
	"${CMAKE_CURRENT_LIST_DIR}/src/modules/basic.c"
	"${CMAKE_CURRENT_LIST_DIR}/src/modules/DPS310.c"
	"${CMAKE_CURRENT_LIST_DIR}/src/modules/SHT4x.c"
	"${CMAKE_CURRENT_LIST_DIR}/src/modules/BME680.c"
)

target_include_directories(weather_forecast PUBLIC
	"${CMAKE_CURRENT_LIST_DIR}/src"
	"${CMAKE_CURRENT_LIST_DIR}/src/modules"
)

target_link_libraries(weather_forecast PUBLIC
			pico_stdlib
			hardware_i2c
			)

#target_compile_options(weather_forecast PRIVATE
#    "-Wall"
#    "-Wextra"
#    "-Werror"
#    "-Wshadow"
#    "-Wtrampolines"
#    "-Wvla"
#    "-Wformat=2"
#    "-Wformat-security"
#    "-Wno-unused-variable"
#)

# CFLAGs quirks
set_source_files_properties(${PICO_SDK_PATH}/src/rp2_common/hardware_flash/flash.c PROPERTIES COMPILE_FLAGS "-Wno-shadow")

pico_enable_stdio_usb(weather_forecast 1)
pico_enable_stdio_uart(weather_forecast 0)

pico_add_extra_outputs(weather_forecast)


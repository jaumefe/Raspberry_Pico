cmake_minimum_required(VERSION 3.13)

set(ENV{PICO_SDK_PATH} "${CMAKE_SOURCE_DIR}/external/pico-sdk")

include(pico_sdk_import.cmake)

project("lcd" C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(LCD
    src/LCD.c        
)

target_sources(LCD PUBLIC
	"${CMAKE_CURRENT_LIST_DIR}/src/modules/pico_liquidcrystal.c"
)

target_include_directories(LCD PUBLIC
	"${CMAKE_CURRENT_LIST_DIR}/src"
	"${CMAKE_CURRENT_LIST_DIR}/src/modules"
)

target_link_libraries(LCD PUBLIC 
                    pico_stdlib 
                    hardware_i2c
                    )
                    
# CFLAGs quirks
set_source_files_properties(${PICO_SDK_PATH}/src/rp2_common/hardware_flash/flash.c PROPERTIES COMPILE_FLAGS "-Wno-shadow")

pico_enable_stdio_usb(LCD 1)
pico_enable_stdio_uart(LCD 0)

pico_add_extra_outputs(LCD)



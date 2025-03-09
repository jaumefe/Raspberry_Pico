cmake_minimum_required(VERSION 3.13)

set(ENV{PICO_SDK_PATH} $ENV{PICO_SDK_PATH})

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project("led_matrix" C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(led_matrix
    src/main.c        
)

target_sources(led_matrix PUBLIC
	"${CMAKE_CURRENT_LIST_DIR}/src/modules/led_matrix_i2c.c"
    "${CMAKE_CURRENT_LIST_DIR}/src/modules/is31fl3731.c"
)

target_include_directories(led_matrix PUBLIC
	"${CMAKE_CURRENT_LIST_DIR}/src"
	"${CMAKE_CURRENT_LIST_DIR}/src/modules"
)

target_link_libraries(led_matrix PUBLIC 
                    pico_stdlib 
                    hardware_i2c
                    )
                    
# CFLAGs quirks
set_source_files_properties(${PICO_SDK_PATH}/src/rp2_common/hardware_flash/flash.c PROPERTIES COMPILE_FLAGS "-Wno-shadow")

pico_enable_stdio_usb(led_matrix 1)
pico_enable_stdio_uart(led_matrix 0)

pico_add_extra_outputs(led_matrix)
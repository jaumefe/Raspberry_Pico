cmake_minimum_required(VERSION 3.13)

set(ENV{PICO_SDK_PATH} $ENV{PICO_SDK_PATH})

include(pico_sdk_import.cmake)

project("RFID" C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(RFID
    src/main.c        
)

target_sources(RFID PUBLIC
	"${CMAKE_CURRENT_LIST_DIR}/src/modules/mfrc522.c"
    "${CMAKE_CURRENT_LIST_DIR}/src/modules/spi.c"
)

target_include_directories(RFID PUBLIC
	"${CMAKE_CURRENT_LIST_DIR}/src"
	"${CMAKE_CURRENT_LIST_DIR}/src/modules"
)

target_link_libraries(RFID PUBLIC 
                    pico_stdlib
                    hardware_spi
                    )
                    
# CFLAGs quirks
set_source_files_properties(${PICO_SDK_PATH}/src/rp2_common/hardware_flash/flash.c PROPERTIES COMPILE_FLAGS "-Wno-shadow")
set_source_files_properties(${PICO_SDK_PATH}/src/rp2_common/2hardware_spi/2include/2hardware_2spi PROPERTIES COMPILE_FLAGS "-Wno-shadow")
pico_enable_stdio_usb(RFID 1)
pico_enable_stdio_uart(RFID 0)

pico_add_extra_outputs(RFID)
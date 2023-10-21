cmake_minimum_required(VERSION 3.13)

set(ENV{PICO_SDK_PATH} "${CMAKE_SOURCE_DIR}/external/pico-sdk")
set(ENV{FREERTOS_KERNEL_PATH} "${CMAKE_SOURCE_DIR}/external/FreeRTOS-Kernel")

include(pico_sdk_import.cmake)
include(FreeRTOS_Kernel_import.cmake)

project("Jaume" C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

#BEGIN RESOLVE GIT REVISION
#https://bravenewmethod.com/2017/07/14/git-revision-as-compiler-definition-in-build-with-cmake/
message(STATUS "Resolving GIT Version...")
find_package(Git)
if(GIT_FOUND)
execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --long --tags --match "[[:digit:]]*" --abbrev=8 --dirty --always
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    OUTPUT_VARIABLE BUILD_VERSION
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    message( STATUS "GIT hash: ${BUILD_VERSION}")
else()
    message(STATUS "GIT not found")
    set(BUILD_VERSION "dirty")
endif()
string(TIMESTAMP _time_stamp)
#END RESOLVE GIT REVISION

pico_sdk_init()

add_executable(prova
	src/main.c)

target_link_libraries(prova
	pico_stdlib
	pico_stdio_usb
	hardware_gpio
	FreeRTOS-Kernel
	FreeRTOS-Kernel-Heap4
)

target_include_directories(prova PUBLIC
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/src/core"
)

target_compile_options(prova PRIVATE
    "-Wall"
    "-Wextra"
    "-Werror"
    "-Wshadow"
    "-Wtrampolines"
    "-Wvla"
    "-Wformat=2"
    "-Wformat-security"
)

target_compile_definitions(prova PUBLIC BUILD_VERSION="${BUILD_VERSION}")

# CFLAGs quirks
set_source_files_properties(${PICO_SDK_PATH}/src/rp2_common/hardware_flash/flash.c PROPERTIES COMPILE_FLAGS "-Wno-shadow")
set_source_files_properties(${FREERTOS_KERNEL_PATH}/stream_buffer.c PROPERTIES COMPILE_FLAGS "-Wno-unused-variable -Wno-error")

pico_enable_stdio_usb(prova 1)
pico_enable_stdio_uart(prova 0)

pico_add_extra_outputs(prova)

# CMake minimum required version
cmake_minimum_required(VERSION 3.12)

set(PICO_TOOLCHAIN_PATH /home/cyp/tools/gcc-arm-none-eabi/)
# Pull in SDK (must be before project)
if(NOT DEFINED PICO_SDK_PATH)
    set(PICO_SDK_PATH ${CMAKE_SOURCE_DIR}/third_party/pico-sdk)
    message(STATUS "PICO_SDK_PATH = ${PICO_SDK_PATH}")
endif()

#set(CMAKE_POSITION_INDEPENDENT_CODE ON)
#SET_PROPERTY(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)

#include(rp2040_v6_hat_c-patch.cmake)
include(pico_sdk_import.cmake)
include(rp2040_v6_hat_c_sdk_version.cmake)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/port)

# Set project name
set(PROJECT_NAME RP2040-v6-HAT-C)

# Set project informations
project(${PROJECT_NAME} C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

# Set ethernet chip
set(WIZNET_CHIP W6100)

if(${WIZNET_CHIP} STREQUAL W6100)
    add_definitions(-D_WIZCHIP_=W6100)
else()
    message(FATAL_ERROR "WIZNET_CHIP is wrong = ${WIZNET_CHIP}")
endif()

message(STATUS "WIZNET_CHIP = ${WIZNET_CHIP}")

# Set the project root directory if it's not already defined, as may happen if
# the tests folder is included directly by a parent project, without including
# the top level CMakeLists.txt.
if(NOT DEFINED FREERTOS_DIR)
    set(FREERTOS_DIR ${CMAKE_SOURCE_DIR}/third_party/FreeRTOS-Kernel)
    message(STATUS "FREERTOS_DIR = ${FREERTOS_DIR}")
endif()

if(NOT DEFINED WIZNET_DIR)
    set(WIZNET_DIR ${CMAKE_SOURCE_DIR}/third_party/io6Library)
    message(STATUS "WIZNET_DIR = ${WIZNET_DIR}")
endif()

if(NOT DEFINED SHA_DIR)
    set(SHA_DIR ${CMAKE_SOURCE_DIR}/third_party/sha1)
    message(STATUS "SHA_DIR = ${SHA_DIR}")
endif()

if(NOT DEFINED PORT_DIR)
    set(PORT_DIR ${CMAKE_SOURCE_DIR}/port)
    message(STATUS "PORT_DIR = ${PORT_DIR}")
endif()

# Sources in subdirectories:
add_subdirectory(src)

# Add libraries in subdirectories
add_subdirectory(${CMAKE_SOURCE_DIR}/third_party)
add_subdirectory(${PORT_DIR})

# Set compile options
add_compile_options(
        -Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

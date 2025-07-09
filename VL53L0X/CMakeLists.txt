# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(VL53L0X C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(VL53L0X main.c
                       libs/vl53l0x/core/src/vl53l0x_api.c
                       libs/vl53l0x/core/src/vl53l0x_api_core.c
                       libs/vl53l0x/core/src/vl53l0x_api_calibration.c
                       libs/vl53l0x/core/src/vl53l0x_api_ranging.c
                       libs/vl53l0x/platform/src/vl53l0x_platform.c
                       )

pico_set_program_name(VL53L0X "VL53L0X")
pico_set_program_version(VL53L0X "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(VL53L0X 0)
pico_enable_stdio_usb(VL53L0X 1)

# Add the standard library to the build
target_link_libraries(VL53L0X
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(VL53L0X PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/vl53l0x/core/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/vl53l0x/platform/inc
)

# Add any user requested libraries
target_link_libraries(VL53L0X 
        hardware_i2c
        
        )

pico_add_extra_outputs(VL53L0X)


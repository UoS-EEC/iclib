cmake_minimum_required(VERSION 2.8)

# Environment paths
set(MSP_GCC_ROOT "/home/sts1u16/ti/msp430-gcc")

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/../msp430-toolchain.cmake")
project(blink)

include_directories(
    ${MSP_GCC_ROOT}/include
    ${CMAKE_SOURCE_DIR}
    )
link_directories(
    ${MSP_GCC_ROOT}/include
    )

add_msp_executable(${PROJECT_NAME}  blink.c)

target_link_libraries(
    ${PROJECT_NAME}
    mul_f5
    )


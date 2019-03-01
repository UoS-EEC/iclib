cmake_minimum_required(VERSION 2.8)

# Environment paths
set(MSP_GCC_ROOT "/home/sts1u16/ti/msp430-gcc")

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/../msp430-toolchain.cmake")
enable_language(C ASM)

project(ic)

include_directories(
    ${MSP_GCC_ROOT}/include
    ${CMAKE_SOURCE_DIR}
    )
link_directories(
    ${MSP_GCC_ROOT}/include
    )

add_msp_library(${CMAKE_PROJECT_NAME}
  ic.c
  ic.h
  ic.S
  memory_management.c
  memory_management.h
  )

cmake_minimum_required(VERSION 2.8)

# Environment paths
set(MSP_GCC_ROOT "/home/sts1u16/ti/msp430-gcc")
set(ICLIB_ROOT "${CMAKE_SOURCE_DIR}/../iclib")

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/../msp430-toolchain.cmake")

project(matmul-tiled)

# CTAGS
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(
    ${MSP_GCC_ROOT}/include
    ${CMAKE_SOURCE_DIR}
    ${ICLIB_ROOT}
    )
link_directories(
    ${MSP_GCC_ROOT}/include
    ${ICLIB_ROOT}
    )

add_msp_executable(
    ${PROJECT_NAME}
    main.c
    input.h
)

target_link_libraries(
  ${PROJECT_NAME}
  ic
  )

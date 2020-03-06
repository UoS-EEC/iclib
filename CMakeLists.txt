cmake_minimum_required(VERSION 3.2)

IF(NOT DEFINED TARGET_ARCH)
  message(FATAL_ERROR "TARGET_ARCH undefined, must be one of {cm0, msp430}")
ENDIF()

IF(${TARGET_ARCH} STREQUAL "cm0")
  set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/cmake/cm0-toolchain.cmake)
ELSEIF(${TARGET_ARCH} STREQUAL "msp430")
  set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/cmake/msp430-toolchain.cmake)
ENDIF()

include(${CMAKE_CURRENT_LIST_DIR}/cmake/common.cmake)

project(ic-examples)

#target_compile_options(aes PUBLIC -finstrument-functions)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(.)
include_directories(cmsis)

add_subdirectory(support)
add_subdirectory(iclib)
add_subdirectory(aes)
#add_subdirectory(matmul)
#add_subdirectory(matmul-tiled)
#add_subdirectory(crc)


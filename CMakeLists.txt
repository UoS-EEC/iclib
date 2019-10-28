cmake_minimum_required(VERSION 3.2)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/msp430-toolchain.cmake)

include(common.cmake)

project(ic-examples)


#target_compile_options(aes PUBLIC -finstrument-functions)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(.)

add_subdirectory(iclib)
add_subdirectory(aes)
add_subdirectory(matmul)
add_subdirectory(matmul-tiled)
add_subdirectory(crc)


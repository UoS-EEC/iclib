cmake_minimum_required(VERSION 3.2)

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

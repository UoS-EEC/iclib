cmake_minimum_required(VERSION 3.0)

include(${CMAKE_CURRENT_LIST_DIR}/../common.cmake)

project(matmul)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${ICLIB_ROOT})
link_directories(${ICLIB_ROOT})

add_executable(
    ${PROJECT_NAME}
    main.c
    input.h
)

add_msp_upload(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} ic mul_f5)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}.elf")

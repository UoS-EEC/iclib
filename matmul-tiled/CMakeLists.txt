cmake_minimum_required(VERSION 3.0)

include(${CMAKE_CURRENT_LIST_DIR}/../common.cmake)

project(matmul-tiled)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${ICLIB_ROOT})
link_directories(${ICLIB_ROOT})

add_executable(
    ${PROJECT_NAME}
    main.c
    input.h
)

add_msp_upload(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} ic mul_f5 gcc c)

set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".elf")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${MSP430-SIZE} "$<TARGET_FILE:${PROJECT_NAME}>"
    COMMENT "Invoking: msp430-elf-size")

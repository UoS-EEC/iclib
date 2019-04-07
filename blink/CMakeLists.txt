cmake_minimum_required(VERSION 3.0)

include(${CMAKE_CURRENT_LIST_DIR}/../common.cmake)

project(blink)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME} blink.c)
add_msp_upload(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".elf")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${MSP430-SIZE} "$<TARGET_FILE:${PROJECT_NAME}>"
    COMMENT "Invoking: msp430-elf-size")

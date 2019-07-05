cmake_minimum_required(VERSION 3.0)

include(${CMAKE_CURRENT_LIST_DIR}/../common.cmake)

project(ic)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(
  ${CMAKE_PROJECT_NAME}
  STATIC
  ic.c
  ic.h
  ic.S
  memory_management.c
  memory_management.h
  )

target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    mul_none
    c
    gcc)

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${MSP430-SIZE} -A -d "$<TARGET_FILE:${PROJECT_NAME}>"
    COMMENT "Invoking: msp430-elf-size")

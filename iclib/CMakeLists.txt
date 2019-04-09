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
    STATIC
    mul_none
    gcc
    c
    )

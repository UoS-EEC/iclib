cmake_minimum_required(VERSION 3.0)

include(${CMAKE_CURRENT_LIST_DIR}/../common.cmake)

FOREACH(METHOD "MS" "AS")
    set(TESTNAME "ic-${METHOD}")
    set(DVDB_TABLE "dvdb-${METHOD}.h")
    add_library(
      ${TESTNAME}
      ic.c
      ic.h
      ic.S
      memory_management.c
      memory_management.h
      common.c
      common.h
      ${DVDB_TABLE}
      )

    IF (${METHOD} STREQUAL "AS")
       target_compile_definitions( ${TESTNAME} PUBLIC -DALLOCATEDSTATE)
   ENDIF()

    target_link_libraries(${TESTNAME} ${SUPPORT_LIBS})

    target_include_directories(${TESTNAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

    add_custom_command(TARGET ${TESTNAME} POST_BUILD
        COMMAND ${MSP430-SIZE} -A -d "$<TARGET_FILE:${TESTNAME}>"
        COMMENT "Invoking: msp430-elf-size")

ENDFOREACH()

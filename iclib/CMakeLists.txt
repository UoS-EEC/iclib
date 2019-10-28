cmake_minimum_required(VERSION 3.0)

FOREACH(METHOD "MS" "AS" "QR")
    string(COMPARE EQUAL ${METHOD} "QR" QUICKRECALL)
    include(${CMAKE_CURRENT_LIST_DIR}/../common.cmake)
    set(TESTNAME "ic-${METHOD}")

    add_library(
      ${TESTNAME}
      ic.c
      ic.h
      ic.S
      memory_management.c
      memory_management.h
      common.c
      common.h
      dvdb.h
      )

    IF (${METHOD} STREQUAL "AS")
       target_compile_definitions( ${TESTNAME} PUBLIC -DALLOCATEDSTATE)
    ELSEIF(${METHOD} STREQUAL "MS")
       target_compile_definitions(${TESTNAME} PUBLIC -DMANAGESTATE)
    ELSEIF(${METHOD} STREQUAL "QR")
       target_compile_definitions(${TESTNAME} PUBLIC -DQUICKRECALL)
    ENDIF()

    target_link_libraries(${TESTNAME} ${SUPPORT_LIBS})

    target_include_directories(${TESTNAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

    add_custom_command(TARGET ${TESTNAME} POST_BUILD
        COMMAND ${MSP430-SIZE} -A -x "$<TARGET_FILE:${TESTNAME}>"
        COMMENT "Invoking: msp430-elf-size")

ENDFOREACH()

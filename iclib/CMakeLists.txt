cmake_minimum_required(VERSION 3.0)

FOREACH(METHOD "MS" "AS" "QR")
    string(COMPARE EQUAL ${METHOD} "QR" QUICKRECALL)
    include(${CMAKE_CURRENT_LIST_DIR}/../cmake/common.cmake)
    set(TESTNAME "ic-${TARGET_ARCH}-${METHOD}")

    IF(${TARGET_ARCH} STREQUAL "cm0")
      add_library(
        ${TESTNAME}
        cm0_ic.c
        cm0_ic.h
        cm0_ic.S
        )
      target_link_libraries(${TESTNAME} support)
    ELSEIF(${TARGET_ARCH} STREQUAL "msp430")
      add_library(
        ${TESTNAME}
        msp430_ic.c
        msp430_ic.h
        msp430_ic.S
        memory_management.c
        memory_management.h
        common.c
        common.h
        dvdb.h
        )
    ENDIF()

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
      COMMAND ${TC-SIZE} -A -x "$<TARGET_FILE:${TESTNAME}>"
      COMMENT "Invoking: ${TC-SIZE}")

ENDFOREACH()

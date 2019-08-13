cmake_minimum_required(VERSION 3.0)

FOREACH(METHOD "MS" "AS")
    set(TESTNAME "matmul-${METHOD}")
    add_executable(
        ${TESTNAME}
        main.c
        input.h
    )
    IF (${METHOD} STREQUAL "AS")
        target_compile_definitions( ${TESTNAME} PUBLIC -DALLOCATEDSTATE)
    ENDIF()
    add_msp_upload(${TESTNAME})
    target_link_libraries(
        ${TESTNAME}
        LINK_PUBLIC ic-${METHOD}
        ${SUPPORT_LIBS})
    set_target_properties(${TESTNAME} PROPERTIES SUFFIX ".elf")
    add_custom_command(TARGET ${TESTNAME} POST_BUILD
        COMMAND ${MSP430-SIZE} -A -d "$<TARGET_FILE:${TESTNAME}>"
        COMMENT "Invoking: msp430-elf-size")
ENDFOREACH()


cmake_minimum_required(VERSION 3.13)
# Commmon function to add linker script and definitions for each target
add_msp_upload(${TESTNAME})
target_link_libraries(
    ${TESTNAME}
    LINK_PUBLIC ic-${METHOD}
    ${SUPPORT_LIBS}
    )

IF (${METHOD} STREQUAL "QR")
    target_link_options(
        ${TESTNAME}
        PRIVATE -T${CMAKE_CURRENT_LIST_DIR}/msp430fr5994-fram-only.ld
	)
ELSE ()
    target_link_options(
        ${TESTNAME}
        PRIVATE -T${CMAKE_CURRENT_LIST_DIR}/msp430fr5994.ld
	)
ENDIF()

set_target_properties(${TESTNAME} PROPERTIES SUFFIX ".elf")
add_custom_command(TARGET ${TESTNAME} POST_BUILD
    COMMAND ${MSP430-SIZE} -A -x "$<TARGET_FILE:${TESTNAME}>"
    COMMENT "Invoking: msp430-elf-size")

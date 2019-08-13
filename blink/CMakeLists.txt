cmake_minimum_required(VERSION 3.0)

add_executable(blink blink.c)
add_msp_upload(blink)

target_link_libraries(blink LINK_PUBLIC ic ${SUPPORT_LIBS})

set_target_properties(blink PROPERTIES SUFFIX ".elf")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${MSP430-SIZE} "$<TARGET_FILE:blink>"
    COMMENT "Invoking: msp430-elf-size")

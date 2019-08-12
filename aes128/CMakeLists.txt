cmake_minimum_required(VERSION 3.0)

include(${CMAKE_CURRENT_LIST_DIR}/../common.cmake)

project(aes-MS)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${ICLIB_ROOT})
link_directories(${ICLIB_ROOT})

add_executable(
    ${PROJECT_NAME}
    main.c
    TI_aes_128_encr_only.c
    TI_aes_128_encr_only.h
    lipsum.h
)

add_msp_upload(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}  ${SUPPORT_LIBS})

set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".elf")

#target_compile_options(${PROJECT_NAME} PUBLIC -finstrument-functions)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${MSP430-SIZE} -A -d "$<TARGET_FILE:${PROJECT_NAME}>"
    COMMENT "Invoking: msp430-elf-size")

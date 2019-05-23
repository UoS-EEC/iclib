# paths
set(ICLIB_ROOT "${CMAKE_CURRENT_LIST_DIR}/iclib")

set(CMAKE_TOOLCHAIN_FILE
    ${CMAKE_CURRENT_LIST_DIR}/msp430-toolchain.cmake)

# Force compiler detection so we can set up flags
enable_language(C ASM)

include_directories($ENV{MSP430_INC}/include) # MSP430 headers
add_compile_options(
    -std=c99
    -mmcu=msp430fr5994
    -msmall
    -fno-common
    #-ffunction-sections
    -Wall
    )

# Linker scripts
set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -T ${CMAKE_SOURCE_DIR}/../msp430fr5994.ld ")

# Add to search path for linker scripts (xx_symbols.ld, included by main linker script)
link_directories(
    ${CMAKE_SOURCE_DIR}/..
    $ENV{MSP430_INC}/include
    )

link_libraries( # Global link flags
    -nostartfiles
    -nodefaultlibs
    )

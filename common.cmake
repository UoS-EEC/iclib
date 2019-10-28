# Force compiler detection so we can set up flags
enable_language(C ASM)

include_directories($ENV{MSP430_INC}/include) # MSP430 headers
add_compile_options(
    -std=c99
    -mcpu=msp430
    -msmall
    -mhwmult=none
    -fno-common
    -Wall
    -fno-zero-initialized-in-bss # We don't want to zero out whole bss on every boot
    )

# Linker scripts
set(LSCRIPTS "-T$ENV{MSP430_INC}/include/msp430fr5994_symbols.ld")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LSCRIPTS}")

# Add to search path for linker scripts (xx_symbols.ld, included by main linker script)
link_directories(
    $ENV{MSP430_GCC_ROOT}/msp430-elf/lib/430/
    $ENV{MSP430_GCC_ROOT}/lib/gcc/msp430-elf/8.2.0/430
    $ENV{MSP430_INC}/include
    )
get_property(test_LINK_DIRECTORIES DIRECTORY PROPERTY LINK_DIRECTORIES)
message(${test_LINK_DIRECTORIES})

link_libraries( # Global link flags
    # Removing standard libs and startup code to prevent
    # unnecessary initialization
    #-nostartfiles
    #-nodefaultlibs
    -nostdlib
    )

# Utility for linking targets to std libs
#set(SUPPORT_LIBS c gcc mul_none)
set(SUPPORT_LIBS mul_none c gcc)

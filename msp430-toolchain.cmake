INCLUDE(CMakeForceCompiler)

# Find toolchain programs
find_program(MSP430-GCC msp430-elf-gcc $ENV{MSP430_GCC_ROOT}/bin)
find_program(MSP430-GXX msp430-elf-g++ $ENV{MSP430_GCC_ROOT}/bin)
find_program(MSP430-OBJCOPY msp430-elf-objcopy $ENV{MSP430_GCC_ROOT}/bin)
find_program(MSP430-SIZE msp430-elf-size $ENV{MSP430_GCC_ROOT}/bin)
find_program(MSP430-OBJDUMP msp430-elf-objdump $ENV{MSP430_GCC_ROOT}/bin)
find_program(MSPDEBUG mspdebug)

# Define toolchain
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_ASM_COMPILER ${MSP430-GCC} CACHE INTERNAL "")
set(CMAKE_C_COMPILER ${MSP430-GCC} CACHE INTERNAL "")
set(CMAKE_CXX_COMPIER ${MSP430-GXX} CACHE INTERNAL "")

# Prevent CMake from testing the compilers
set(CMAKE_C_COMPILER_WORKS 1 CACHE INTERNAL "")
set(CMAKE_CXX_COMPILER_WORKS 1 CACHE INTERNAL "")

#Debug by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

function(add_msp_upload EXECUTABLE)
  add_custom_target(upload_${EXECUTABLE}
    COMMAND ${MSPDEBUG} tilib "erase"
    COMMAND ${MSPDEBUG} tilib "prog ${EXECUTABLE}.elf"
    DEPENDS ${EXECUTABLE})
endfunction(add_msp_upload)

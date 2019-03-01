# Toolchain cmake file for msp430-gcc toolchain
# See https://github.com/AlexanderSidorenko/msp-cmake/blob/master/cmake/gcc/msp430.cmake

INCLUDE(CMakeForceCompiler)

# Generic flags

set(MSP430_CFLAGS
    "-std=c99 -mmcu=msp430 -mcpu=msp430 -msmall -mhwmult=none -g -Wall -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -Wno-discarded-qualifiers"
  CACHE STRING "MSP430 compilation flags")
set(MSP430_LFLAGS
  "-mmcu=msp430fr5994 -mcpu=msp430 -msmall -mhwmult=none"
  CACHE STRING "MSP430 link flags")

# Linker scripts
set(LSCRIPTS "-T${MSP_GCC_ROOT}/include/msp430fr5994_symbols.ld -T${CMAKE_SOURCE_DIR}/../msp430fr5994.ld ")

# Find toolchain programs
find_program(MSP430-GCC msp430-elf-gcc ${MSP_GCC_ROOT})
find_program(MSP430-GXX msp430-elf-g++ ${MSP_GCC_ROOT})
find_program(MSP430-OBJCOPY msp430-objcopy ${MSP_GCC_ROOT})
find_program(MSP430-SIZE msp430-size ${MSP_GCC_ROOT})
find_program(MSP430-OBJDUMP msp430-objdump ${MSP_GCC_ROOT})
find_program(MSPDEBUG mspdebug)

# Define toolchain
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER ${MSP430-GCC})
set(CMAKE_CXX_COMPIER ${MSP430-GXX})

# Prevent CMake from testing the compilers
set(CMAKE_C_COMPILER_WORKS 1 CACHE INTERNAL "")
set(CMAKE_CXX_COMPILER_WORKS 1 CACHE INTERNAL "")

#Debug by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

function(add_msp_executable EXECUTABLE_NAME)
  if (NOT ARGN)
    message(FATAL_ERROR "List of source files for target ${EXECUTABLE_NAME} is empty.")
  endif()
  set(ELF_FILE ${EXECUTABLE_NAME}.elf)

  add_executable(${EXECUTABLE_NAME} ${ARGN})

  set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    OUTPUT_NAME ${ELF_FILE}
    COMPILE_FLAGS "${MSP430_CFLAGS}"
    LINK_FLAGS " ${LSCRIPTS} ${MSP430_LFLAGS}")

  #Display size info after compilation
  add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD COMMAND ${MSP430-SIZE} ${ELF_FILE})
endfunction(add_msp_executable)

function(add_msp_library LIBRARY_NAME)
  if (NOT ARGN)
    message(FATAL_ERROR "List of source files for target ${LIBRARY_NAME} is empty.")
  endif()

  add_library(${LIBRARY_NAME} ${ARGN})

  set_target_properties(
      ${LIBRARY_NAME} PROPERTIES
      OUTPUT_NAME ${LIBRARY_NAME}
      COMPILE_FLAGS "${MSP430_CFLAGS}"
      LINK_FLAGS " ${LSCRIPTS} ${MSP430_LFLAGS}")
endfunction(add_msp_library)

function(msp430_add_executable_upload ${EXECUTABLE})
  add_custom_target(upload_${EXECUTABLE}
    COMMAND ${MSPDEBUG} -q rf2500 "prog ${EXECUTABLE}.elf"
    DEPENDS ${EXECUTABLE})
endfunction(msp430_add_executable_upload)

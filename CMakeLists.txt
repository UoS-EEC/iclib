#
# Copyright (c) 2019-2020, University of Southampton and Contributors.
# All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

cmake_minimum_required(VERSION 3.10)

# ------ Options -------
option(SIMULATION "Enable fused simulation control (via SIMPLE_MONITOR)" ON)

IF(NOT DEFINED TARGET_ARCH)
  message(FATAL_ERROR "TARGET_ARCH undefined, must be one of {cm0, msp430}")
ENDIF()

IF(${SIMULATION})
  IF(NOT EXISTS $ENV{FUSED_ROOT}/include/fused.h)
    message(FATAL_ERROR "SIMULATION set, but can't find "
      "\$ENV{FUSED_ROOT}/include/fused.h, is the environment variable "
      "FUSED_ROOT set correctly? If building without Fused, set SIMULATION=OFF "
      " (-D SIMULATION=OFF)"
      )
  ENDIF()
  add_compile_options(-DSIMULATION)
ENDIF()

# ------

IF(${TARGET_ARCH} STREQUAL "cm0")
  set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/cmake/cm0-toolchain.cmake)
ELSEIF(${TARGET_ARCH} STREQUAL "msp430")
  set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/cmake/msp430-toolchain.cmake)
ENDIF()


include(${CMAKE_CURRENT_LIST_DIR}/cmake/common.cmake)

project(ic-examples)

#target_compile_options(aes PUBLIC -finstrument-functions)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(.)

add_subdirectory(apps)
add_subdirectory(lib)

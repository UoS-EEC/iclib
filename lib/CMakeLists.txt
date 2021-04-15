#
# Copyright (c) 2019-2020, University of Southampton and Contributors.
# All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

IF(${TARGET_ARCH} STREQUAL "cm0")
  include_directories(cmsis)
ENDIF()

add_subdirectory(support)
add_subdirectory(iclib)

# ------ Submodules ------

IF(${TARGET_ARCH} STREQUAL "cm0")
# CMSIS_5
set(ROOT ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS_5)
set(ARM_CPU "cortex-m0")
option(DISABLEFLOAT16 "Disable support for f16 format" ON)
list(APPEND CMAKE_MODULE_PATH ${ROOT}/CMSIS/DSP/Toolchain)
include(GCC)
add_subdirectory(CMSIS_5/CMSIS/DSP/Source)
add_subdirectory(CMSIS_5/CMSIS/NN/Source)

# Supporess some warnings from CMSISNN
target_compile_options(CMSISNNConvolutions PRIVATE "-Wno-unused")
endif()

#
# Copyright (c) 2019-2020, University of Southampton and Contributors.
# All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

cmake_minimum_required(VERSION 3.0)

FOREACH(METHOD "MS" "AS" "QR")
  set(TESTNAME "matmul-tiled-${METHOD}-${TARGET_ARCH}")
  add_executable(${TESTNAME} main.c input.h)
  include(${CMAKE_CURRENT_LIST_DIR}/../cmake/tail.cmake)
ENDFOREACH()

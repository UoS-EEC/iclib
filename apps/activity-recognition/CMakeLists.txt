#
# Copyright (c) 2019-2020, University of Southampton and Contributors.
# All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

cmake_minimum_required(VERSION 3.0)

FOREACH(METHOD "AS" "QR")
  set(TESTNAME "ar-${METHOD}-${TARGET_ARCH}")
  add_executable(
    ${TESTNAME}
    activity.c
  )
  include(${PROJECT_SOURCE_DIR}/cmake/tail.cmake)
ENDFOREACH()



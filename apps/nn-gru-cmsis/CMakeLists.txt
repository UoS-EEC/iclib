# Copyright (c) 2019-2020, University of Southampton and Contributors.
# All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

cmake_minimum_required(VERSION 3.0)

FOREACH(METHOD "AS")
  set(TESTNAME "nn-gru-cmsis-${METHOD}-${TARGET_ARCH}")
  add_executable(${TESTNAME})
  target_sources(${TESTNAME}
    PRIVATE
      benchmark.c
      arm_nnexamples_gru_test_data.h)
  target_link_libraries(${TESTNAME}
    PRIVATE
      CMSISDSP CMSISNN)
  include(${PROJECT_SOURCE_DIR}/cmake/tail.cmake)
ENDFOREACH()

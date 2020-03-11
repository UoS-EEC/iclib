#
# Copyright (c) 2019-2020, University of Southampton and Contributors.
# All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

cmake_minimum_required(VERSION 3.0)

FOREACH(METHOD "MS" "AS" "QR")
  set(TESTNAME "crc-${METHOD}-${TARGET_ARCH}")
  add_executable(
    ${TESTNAME}
    main.c
    crc.h
    crc_32.c
    lipsum.h
    sniptype.h
  )
  include(${CMAKE_CURRENT_LIST_DIR}/../cmake/tail.cmake)
ENDFOREACH()


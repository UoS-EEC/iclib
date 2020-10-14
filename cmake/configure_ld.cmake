#
# Copyright (c) 2019-2020, University of Southampton and Contributors.
# All rights reserved.
#
# SPDX-License-Identifier: Apache-2.0
#

# DESCRIPTION: This file is used to generate different permutations of memory
#              allocations in linker scripts. Generates one linker file per
#              target & intermittent computing method.
#
#              <arch>.ld.in -> <arch>-<ic-method>.ld

cmake_minimum_required(VERSION 3.12)

set(CM0_LD_SRC ${PROJECT_SOURCE_DIR}/lib/support/cm0.ld.in)

# ------ NVM only ------
set(LD_DATA_ALLOC               "dnvm")
set(LD_BOOT_STACK_ALLOC         "dnvm")
set(LD_STACK_ALLOC              "dnvm")
set(LD_HEAP_ALLOC               "dnvm")
set(LD_MMDATA_ALLOC             "dnvm")

configure_file(${CM0_LD_SRC} ${CMAKE_BINARY_DIR}/cm0-QR.ld)
configure_file(${CM0_LD_SRC} ${CMAKE_BINARY_DIR}/cm0-CS.ld)

# ------ Code in NVM, data in SRAM ------
set(LD_DATA_ALLOC               "sram AT> dnvm")
set(LD_BOOT_STACK_ALLOC         "sram")
set(LD_STACK_ALLOC              "sram")
set(LD_HEAP_ALLOC               "sram AT> dnvm")
set(LD_MMDATA_ALLOC             "sram AT> dnvm")

configure_file(${CM0_LD_SRC} ${CMAKE_BINARY_DIR}/cm0-AS.ld)
configure_file(${CM0_LD_SRC} ${CMAKE_BINARY_DIR}/cm0-MS.ld)


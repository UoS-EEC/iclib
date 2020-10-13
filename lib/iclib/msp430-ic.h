/*
 * Copyright (c) 2018-2020, University of Southampton.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include "lib/iclib/config.h"

/**
 * @brief fastmemcpy Hand-crafted faster version of memcpy for msp430. The
 * default implementation is very inefficient.
 */
void fastmemcpy(uint8_t *dst, uint8_t *src, size_t len);

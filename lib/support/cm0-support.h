/*
 * Copyright (c) 2018-2020, University of Southampton.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include "lib/support/cm0.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef SIMULATION
#include <fused.h>
#endif

typedef union {
  volatile uint32_t WORD;
  volatile uint16_t HALFWORD[2];
  volatile uint8_t BYTE[4];
} GPIO_Data_TypeDef;

typedef struct {
  GPIO_Data_TypeDef DATA;
  uint32_t RESERVED_0[255];
  GPIO_Data_TypeDef DIR;
  uint32_t RESERVED_1[3];
  GPIO_Data_TypeDef IE;
} GPIO_TypeDef;

static GPIO_TypeDef *const Gpio = (GPIO_TypeDef *)GPIO_BASE;

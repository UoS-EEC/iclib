/*
 * Copyright (c) 2018-2020, University of Southampton.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* Generate reference output from host machine */

#include <stdint.h>
#include <stdio.h>

#define MMDATA  // Empty macro

#include "input.h"

int16_t output[MATSIZE][MATSIZE] = {0};
typedef unsigned short u16;

void matmult(int m, int n, int16_t a[m][n], int16_t b[m][n],
             int16_t out[m][n]) {
  int row = m, col = n;
  int incr = 5;

  // For each tile
  for (u16 i = 0; i < row; i += incr) {
    for (u16 j = 0; j < col; j += incr) {
      for (u16 k = 0; k < row; k += incr) {
        // Calculate tile product
        for (u16 x = i; x < i + incr; x++) {
          for (u16 y = j; y < j + incr; y++) {
            for (u16 z = k; z < k + incr; z++) {
              out[x][y] += a[x][z] * b[z][y];
            }
          }
        }
      }
    }
  }
}

void main(void) {
  matmult(MATSIZE, MATSIZE, a, b, output);
  for (int i = 0; i < MATSIZE; i++) {
    for (int j = 0; j < MATSIZE; j++) {
      printf("%5d, ", output[i][j]);
    }
    printf("\n");
  }
}

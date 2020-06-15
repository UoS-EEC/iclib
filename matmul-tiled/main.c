/*
 * Copyright (c) 2018-2020, University of Southampton.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>
#include "iclib/ic.h"
#include "input.h"
#include "support/support.h"

int16_t output[MATSIZE][MATSIZE] MMDATA;

// Tiled implementation to improve locality
void matmult(int m, int n, int16_t a[m][n], int16_t b[m][n],
             int16_t out[m][n]) {
  int row = m, col = n;
  int incr = 5;

  for (uint16_t i = 0; i < row; i += incr) {
    for (uint16_t j = 0; j < col; j += incr) {
      // Aqcuire and clear a tile of out
      for (int aq = 0; aq < incr; aq++) {
        mm_acquire_array((uint8_t *)&out[aq + i][j], incr * sizeof(int16_t),
                         MM_READWRITE);
        memset(&out[aq + i][j], 0, incr * sizeof(int16_t));
      }

      for (uint16_t k = 0; k < row; k += incr) {
        // Acquire a tile of A and B
        for (int aq = 0; aq < incr; aq++) {
          mm_acquire_array((uint8_t *)&a[aq + i][k], incr * sizeof(int16_t),
                           MM_READWRITE);
          mm_acquire_array((uint8_t *)&b[aq + k][j], incr * sizeof(int16_t),
                           MM_READWRITE);
        }
        // Calculate tile product
        for (uint16_t x = i; x < i + incr; x++) {
          for (uint16_t y = j; y < j + incr; y++) {
            for (uint16_t z = k; z < k + incr; z++) {
              out[x][y] += a[x][z] * b[z][y];
            }
          }
        }

        // Release a tile of A and B
        for (int aq = 0; aq < incr; aq++) {
          mm_release_array((uint8_t *)&a[aq + i][k], incr * sizeof(int16_t));
          mm_release_array((uint8_t *)&b[aq + k][j], incr * sizeof(int16_t));
        }
      }

      // Release a tile of out
      for (int aq = 0; aq < incr; aq++) {
        mm_release_array((uint8_t *)&out[aq + i][j], incr * sizeof(int16_t));
      }
    }
  }
}

int main(void) {
  for (volatile unsigned i = 0; i < 3; ++i) {
    indicate_workload_begin();
    matmult(MATSIZE, MATSIZE, a, b, output);
    indicate_workload_end();
    mm_flush();
    wait();
  }
  end_experiment();
  return 0;
}

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

int16_t output[MATSIZE][MATSIZE];

void matmult(int m, int n, int16_t a[m][n], int16_t b[m][n],
             int16_t out[m][n]) {
  int i, j, k;

  for (i = 0; i < m; ++i) {
    for (j = 0; j < n; ++j) {
      out[i][j] = 0;
      for (k = 0; k < m; ++k) {
        out[i][j] += a[i][k] * b[k][j];
      }
      printf("%5i, ", out[i][j]);
    }
    printf("\n");
  }
}

int main(void) { matmult(MATSIZE, MATSIZE, a, b, output); }

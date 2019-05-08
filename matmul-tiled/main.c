#include <ic.h>
#include <memory_management.h>
#include <msp430fr5994.h>
#include <string.h>

#include "input.h"

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
                mm_acquire_array((uint8_t *)&out[aq + i][j],
                                 incr * sizeof(int16_t), MM_READWRITE);
                memset(&out[aq + i][j], 0, incr * sizeof(int16_t));
            }

            for (uint16_t k = 0; k < row; k += incr) {
                // Acquire a tile of A and B
                for (int aq = 0; aq < incr; aq++) {
                    mm_acquire_array((uint8_t *)&a[aq + i][k],
                                     incr * sizeof(int16_t), MM_READWRITE);
                    mm_acquire_array((uint8_t *)&b[aq + k][j],
                                     incr * sizeof(int16_t), MM_READWRITE);
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
                    mm_release_array((uint8_t *)&a[aq + i][k],
                                     incr * sizeof(int16_t));
                    mm_release_array((uint8_t *)&b[aq + k][j],
                                     incr * sizeof(int16_t));
                }
            }

            // Release a tile of out
            for (int aq = 0; aq < incr; aq++) {
                mm_release_array((uint8_t *)&out[aq + i][j],
                                 incr * sizeof(int16_t));
            }
        }
    }
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;  // Stop watchdog timer

    while (1) {
        P1OUT |= BIT2;
        matmult(MATSIZE, MATSIZE, a, b, output);
        // Toggle pin to show that computation has finished
        mm_flush();
        P1OUT &= ~BIT2;
        for (int i = 0; i < 100; i++) {
        }
    }
}

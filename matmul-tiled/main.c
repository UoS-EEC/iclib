#include <ic.h>
#include <memory_management.h>
#include <msp430fr5994.h>

#include "input.h"

int16_t output[MATSIZE][MATSIZE] MMDATA;

// Tiled implementation to improve locality
void matmult(int m, int n, int16_t a[m][n], int16_t b[m][n],
             int16_t out[m][n]) {
    int row = m, col = n;
    int incr = 5;

    for (u16 i = 0; i < row; i += incr) {
        for (u16 j = 0; j < col; j += incr) {
            for (int aq = 0; aq < incr; aq++) {
                mm_acquire_array((u8 *)&out[aq + i][j], incr * sizeof(int16_t),
                                 MM_READWRITE);
            }

            for (u16 k = 0; k < row; k += incr) {
                // Acquire a tile of A and B
                for (int aq = 0; aq < incr; aq++) {
                    mm_acquire_array((u8 *)&a[aq + i][k],
                                     incr * sizeof(int16_t), MM_READWRITE);
                    mm_acquire_array((u8 *)&b[aq + k][j],
                                     incr * sizeof(int16_t), MM_READWRITE);
                }
                // Calculate tile product
                for (u16 x = i; x < i + incr; x++) {
                    for (u16 y = j; y < j + incr; y++) {
                        for (u16 z = k; z < k + incr; z++) {
                            out[x][y] += a[x][z] * b[z][y];
                        }
                    }
                }

                // Release a tile of A and B
                for (int aq = 0; aq < incr; aq++) {
                    mm_release_array((u8 *)&a[aq + i][k],
                                     incr * sizeof(int16_t));
                    mm_release_array((u8 *)&b[aq + k][j],
                                     incr * sizeof(int16_t));
                }
            }

            for (int aq = 0; aq < incr; aq++) {
                mm_release_array((u8 *)&out[aq + i][j], incr * sizeof(int16_t));
            }
        }
    }
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;  // Stop watchdog timer
    ic_init();

    while (1) {
        matmult(MATSIZE, MATSIZE, a, b, output);
        // Toggle pin to show that computation has finished
        P1OUT |= BIT2;
        for (int i = 0; i < 100; i++) {
        }
        P1OUT &= ~BIT2;
    }
}

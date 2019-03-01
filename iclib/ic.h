#ifndef IC_SRC
#define IC_SRC

#include <stdint.h>

#include "config.h"

// Global variables
extern uint16_t mmdata_snapshot[];

// Function Declarations
/**
 * @brief Initialise peripherals and sleep until supply voltage recovers beyond
 * restore threshold
 */
void ic_init(void);

/**
 * @brief Update restore and suspend thresholds based on number of bytes to be
 * suspended/restored
 *
 * @param n_suspend
 * @param n_restore
 */
void update_thresholds(uint16_t n_suspend, uint16_t n_restore);

#endif /* IC_SRC */


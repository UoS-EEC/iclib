/*
 * Copyright (c) 2018-2020, University of Southampton.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SRC_MEMORY_MANAGEMENT_H_
#define SRC_MEMORY_MANAGEMENT_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "lib/iclib/config.h"

/***************** Typedefs **************************************************/

#ifdef MSP430_ARCH
typedef uint16_t addr_t;
typedef uint16_t word_t;
typedef uint16_t *wordptr_t;
#elif defined(CM0_ARCH) || defined(TRIFFID_ARCH)
typedef uint32_t addr_t;
typedef uint32_t word_t;
typedef uint32_t *wordptr_t;
#else
#error "Target architecture undefined or invalid"
#endif

/************************** Constant Definitions *****************************/

/** Type definitions *********************************************************/
typedef enum { MM_READONLY, MM_READWRITE } mm_mode;

/************************** Function Prototypes ******************************/

/**
 * @brief Acquire a byte from managed memory.
 * @param Pointer to variable held in static memory
 * @param mm_mode access mode
 */
int mm_acquire(const uint8_t *memPtr, const mm_mode mode);

/**
 * @brief Release a byte from managed memory.
 * @param Pointer to variable held in static memory
 * @return Status: 0=success
 */
int mm_release(const uint8_t *memPtr);

/**
 * @brief Acquire an array from static memory
 * @param memPtr pointer to first element in array
 * @param len size of array
 * @param mm_mode access mode
 * @return
 */
int mm_acquire_array(const uint8_t *memPtr, const int len, const mm_mode mode);

/**
 * @brief Release array
 * @param memPtr pointer to first element in array
 * @param len size of array
 * @return
 */
int mm_release_array(const uint8_t *memPtr, const int len);

/**
 * @brief Aquire data from an array one page at a time. May load two pages if
 * the first element of the array crosses a page boundary.
 * @param memPtr Pointer to first element of array
 * @param len Length of array
 * @param mm_mode access mode
 * @return Number of elements acquired.
 */
int mm_acquire_page(const uint8_t *memPtr, const int nElements,
                    const int elementSize, mm_mode mode);
/**
 * @brief Get the number of currently active pages
 * @return number of currently active pages
 */
int mm_get_n_active_pages(void);

/**
 * @brief Get number of (possibly) dirty pages
 * @return number of (possibly) dirty pages
 */
int mm_get_n_dirty_pages(void);
void mm_init_lru(void);

/**
 * @brief Restore all active pages to memory from FRAM
 */
void mm_restore(void);

/**
 * @brief Save all modified pages to FRAM
 * @return number of bytes saved
 */
int mm_flush(void);

#endif /* SRC_MEMORY_MANAGEMENT_H_ */

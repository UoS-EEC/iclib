/*
 * Copyright (c) 2018-2020, University of Southampton.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/***************************** Include Files *********************************/
#include "lib/iclib/ic.h"
#include "lib/iclib/memory-management.h"
#include "lib/support/support.h"
#include <stdint.h>
#include <string.h>

/*************************** Global Definitions ******************************/

static int mm_n_dirty_pages = 0;
static int mm_n_active_pages = 0;

/************************** Constant Definitions *****************************/
extern uint8_t __mmdata_low, __mmdata_high, __mmdata_loadLow;

struct LRU_candidate {
  const uint8_t pageNumber;
  uint8_t tableIdx;
};

/**************************** Type Definitions *******************************/

/***************** Macros ****************************************************/
#define NPAGES (MMDATA_SIZE / PAGE_SIZE)

#define DUMMY_PAGE 255   //! Used for LRU table
#define REFCNT_MASK 0x3F //! Reference count
#define LOADED 0x80      //! Mask to check if loaded
#define MODIFIED 0x40    //! Mask to check if modified

#if NPAGES >= DUMMY_PAGE
#error Too many pages, increase page size or reduce memory usage
#endif

#ifdef MSP430_ARCH
#define MEMCPY fastmemcpy
#define IRQ_DISABLE                                                            \
  do {                                                                         \
    __disable_interrupt();                                                     \
  } while (0)
#define IRQ_ENABLE                                                             \
  do {                                                                         \
    __enable_interrupt();                                                      \
  } while (0)
#define IRQ_ENABLED __get_SR_register() & GIE
#elif defined(CM0_ARCH)
#define MEMCPY memcpy
#define IRQ_DISABLE                                                            \
  do {                                                                         \
    disable_interrupt();                                                       \
  } while (0)
#define IRQ_ENABLE                                                             \
  do {                                                                         \
    enable_interrupt();                                                        \
  } while (0)
#define IRQ_ENABLED get_interrupt_enable()
#endif

/************************** Function Prototypes ******************************/
static void writePageNvm(const uint8_t pageNumber);
static void loadPage(const uint8_t pageNumber);
static void addLRU(const uint8_t pageNumber);
static void clearLRU(const uint8_t index);
static void clearLRUPage(const uint8_t pageNumber);

/*************************** Extern Functions ********************************/

/************************** Variable Definitions *****************************/

static uint8_t attributeTable[NPAGES] = {0};
static uint8_t lruTable[MAX_DIRTY_PAGES];

/*************************** Function definitions ****************************/

int mm_acquire(const uint8_t *memPtr, const mm_mode mode) {
#if defined(ALLOCATEDSTATE) || defined(QUICKRECALL)
  return 0;
#endif
  if ((&__mmdata_low > memPtr) || (&__mmdata_high < memPtr)) {
    while (1)
      ; // Error: Pointer out of bounds.
  }

  int pageNumber = ((word_t)memPtr - (word_t)&__mmdata_low) / PAGE_SIZE;

  if ((attributeTable[pageNumber] & REFCNT_MASK) >= 64) {
    while (1)
      ; // Error: Too many references to a single page
  }

  if (mode == MM_READWRITE) {
    if (mm_n_dirty_pages >= MAX_DIRTY_PAGES) {
      // Need to write back an inactive and dirty page first
      for (int i = MAX_DIRTY_PAGES - 1; i >= 0; i--) {
        uint8_t candidate = lruTable[i];
        if (candidate != DUMMY_PAGE) {
          if ((attributeTable[candidate] & REFCNT_MASK) == 0 &&
              (attributeTable[candidate] & LOADED) &&
              (attributeTable[candidate] & MODIFIED)) {
            writePageNvm(candidate);
            clearLRU(i);
            break;
          }
        }
      }

      if (mm_n_dirty_pages >= MAX_DIRTY_PAGES) {
        while (1)
          ; // Error: MAX_DIRTY_PAGES exceeded
      }
    }

    if (!(attributeTable[pageNumber] &
          MODIFIED)) { // if page isn't already dirty
      mm_n_dirty_pages++;
      attributeTable[pageNumber] |= MODIFIED;
    }

    // Update LRU with the new dirty page
    addLRU(pageNumber);
  }

  loadPage(pageNumber);

  if ((attributeTable[pageNumber] & REFCNT_MASK) == 0) {
    mm_n_active_pages++;
  }
  attributeTable[pageNumber]++;

  // Update suspend/restore thresholds
  static int oldPageTotal = 0;
  if (oldPageTotal != mm_n_dirty_pages + mm_n_active_pages) {
    ic_update_thresholds(mm_n_dirty_pages * PAGE_SIZE,
                         mm_n_active_pages * PAGE_SIZE);
    oldPageTotal = mm_n_dirty_pages + mm_n_active_pages;
  }

  return 0;
}

int mm_release(const uint8_t *memPtr) {
#ifndef MANAGEDSTATE
  return 0;
#endif
  int pageNumber = ((word_t)memPtr - (word_t)&__mmdata_low) / PAGE_SIZE;
  if ((attributeTable[pageNumber] & REFCNT_MASK) > 0) {
    attributeTable[pageNumber]--;
    if ((attributeTable[pageNumber] & REFCNT_MASK) == 0) {
      mm_n_active_pages--;
    }
  } else {
    while (1)
      ; // Error: Attempt to release inactive page
  }
  return 0;
}

void mm_restore(void) {
#if defined(ALLOCATEDSTATE) || defined(QUICKRECALL)
  MEMCPY(&__mmdata_low, &__mmdata_loadLow, &__mmdata_high - &__mmdata_low);
  return;
#endif

  for (int pageNumber = 0; pageNumber < NPAGES; pageNumber++) {
    // Clear loaded pages (bits are set again when calling loadPage)
    attributeTable[pageNumber] &= ~LOADED;

    if ((attributeTable[pageNumber] & REFCNT_MASK) > 0) {
      loadPage(pageNumber);
    }
  }
}

int mm_flush(void) {
#ifndef MANAGEDSTATE
  // Save entire section
  MEMCPY(&__mmdata_loadLow, &__mmdata_low, &__mmdata_high - &__mmdata_low);
  return ((word_t)&__mmdata_high - (word_t)&__mmdata_low);
#endif
  unsigned pagesSaved = 0;

  for (int pageNumber = 0; pageNumber < NPAGES; pageNumber++) {
    if (mm_n_dirty_pages == 0) {
      break;
    } else if (attributeTable[pageNumber] & MODIFIED) {
      writePageNvm(pageNumber);
      pagesSaved++;
      if ((attributeTable[pageNumber] & REFCNT_MASK) == 0) {
        clearLRUPage(pageNumber);
      }
    }
  }

  ic_update_thresholds(mm_n_dirty_pages * PAGE_SIZE,
                       mm_n_active_pages * PAGE_SIZE);

  return pagesSaved * PAGE_SIZE;
}

/**
 * @brief Write a page to NVM
 * @param pageNumber
 */
static void writePageNvm(const uint8_t pageNumber) {
  word_t pageOffset = pageNumber * PAGE_SIZE;
  int len;

  if (!(attributeTable[pageNumber] & MODIFIED)) {
    return;
  }

  word_t old_gie = IRQ_ENABLED;
  IRQ_DISABLE; // Critical section (attributes get messed up if interrupted)
  word_t srcStart = (word_t)&__mmdata_low + pageOffset;
  word_t dstStart = (word_t)(&__mmdata_loadLow) + pageOffset;
  len = PAGE_SIZE;
  if (srcStart + PAGE_SIZE > (word_t)&__mmdata_high) {
    len = (word_t)&__mmdata_high - srcStart;
  }

  // Save page
  MEMCPY((uint8_t *)dstStart, (uint8_t *)srcStart, len);

  if ((attributeTable[pageNumber] & REFCNT_MASK) == 0) {
    // Page is clean
    attributeTable[pageNumber] &= ~MODIFIED;
    mm_n_dirty_pages--;
  }
  if (old_gie) {
    IRQ_ENABLE;
  }
}

int mm_acquire_array(const uint8_t *memPtr, const int len, const mm_mode mode) {
#if defined(ALLOCATEDSTATE) || defined(QUICKRECALL)
  return 0;
#endif
  int status = 0;
  // Acquire each page referenced
  const uint8_t *ptr = memPtr;
  int remaining = len;
  while (remaining > 0) {
    status = mm_acquire(ptr, mode);
    if (remaining > PAGE_SIZE) {
      ptr += PAGE_SIZE;
      remaining -= PAGE_SIZE;
    } else {
      ptr += remaining;
      remaining = 0;
    }
  }
  return status;
}

int mm_release_array(const uint8_t *memPtr, const int len) {
#if defined(ALLOCATEDSTATE) || defined(QUICKRECALL)
  return 0;
#endif
  int status = 0;

  // Error check
  if ((memPtr > &__mmdata_high) || (memPtr < &__mmdata_low) ||
      (memPtr + len) > &__mmdata_high) {
    while (1)
      ; // Error: access out of bounds
  }

  // Release each page referenced
  const uint8_t *ptr = memPtr;
  int remaining = len;
  while (remaining > 0) {
    status = mm_release(ptr);
    if (remaining > PAGE_SIZE) {
      ptr += PAGE_SIZE;
      remaining -= PAGE_SIZE;
    } else {
      ptr += remaining;
      remaining = 0;
    }
  }
  return status;
}

int mm_get_n_active_pages(void) {
  int nActive = 0;
  for (int i = 0; i < NPAGES; i++) {
    if ((attributeTable[i] & REFCNT_MASK) > 0) {
      nActive++;
    }
  }
  return nActive;
}

int mm_get_n_dirty_pages(void) { return mm_n_dirty_pages; }

int mm_acquire_page(const uint8_t *memPtr, const int nElements,
                    const int elementSize, mm_mode mode) {
#if defined(ALLOCATEDSTATE) || defined(QUICKRECALL)
  return nElements;
#endif
  int bytesAcquired;

  // Check if first element crosses a page boundary
  word_t pageNumberStart = (memPtr - &__mmdata_low) / PAGE_SIZE;
  word_t pageNumberEnd =
      (memPtr + elementSize - 1 // end address of first element
       - &__mmdata_low          // - start address
       ) /
      PAGE_SIZE;

  if (pageNumberStart != pageNumberEnd) {
    // Need to load 2 pages
    mm_acquire(memPtr, mode);
    mm_acquire(memPtr + elementSize - 1, mode);
    bytesAcquired = (pageNumberStart + 1) * PAGE_SIZE -
                    (memPtr - &__mmdata_low) + PAGE_SIZE;
  } else {
    mm_acquire(memPtr, mode);
    bytesAcquired =
        (pageNumberStart + 1) * PAGE_SIZE - (memPtr - &__mmdata_low);
  }

  // Don't report acquiring more elements than requested
  if (bytesAcquired / elementSize >= nElements) {
    return nElements;
  } else {
    return bytesAcquired / elementSize;
  }
}

/**
 * @brief Load page from FRAM if it is not already loaded.
 * @param pageNumber
 */
static void loadPage(const uint8_t pageNumber) {
  if (!(attributeTable[pageNumber] & LOADED)) {
    uint16_t pageOffset = pageNumber * PAGE_SIZE;
    uint8_t *dstStart = &__mmdata_low + pageOffset;     // Memory address
    uint8_t *srcStart = &__mmdata_loadLow + pageOffset; // Snapshot address
    int len = PAGE_SIZE;
    if ((addr_t)dstStart + PAGE_SIZE > (addr_t)&__mmdata_high) {
      len = (addr_t)&__mmdata_high - (addr_t)dstStart;
    }

    MEMCPY(dstStart, srcStart, len);
    attributeTable[pageNumber] |= LOADED;
  }
}

/**
 * @brief Initialise LRU table with dummy pages
 */
void mm_init_lru(void) {
  for (int i = 0; i < MAX_DIRTY_PAGES; i++) {
    lruTable[i] = DUMMY_PAGE;
  }
}

/**
 * @brief Add a new reference to LRU table.
 * @param pageNumber
 */
static void addLRU(const uint8_t pageNumber) {
  uint8_t tmp1, tmp2;

  if (pageNumber > NPAGES) {
    while (1)
      ; // Error: page number out of bounds.
  }

  tmp1 = pageNumber;
  for (int i = 0; i < MAX_DIRTY_PAGES; i++) {
    if (lruTable[i] == pageNumber) {
      lruTable[i] = tmp1;
      for (int j = i + 1; j < MAX_DIRTY_PAGES; j++) {
        if (lruTable[j] == tmp1) {
          lruTable[j] = DUMMY_PAGE;
          break;
        }
      }
      break;
    } else if (lruTable[i] == DUMMY_PAGE) {
      lruTable[i] = tmp1;
      for (int j = i + 1; j < MAX_DIRTY_PAGES; j++) {
        if (lruTable[j] == tmp1) {
          lruTable[j] = DUMMY_PAGE;
          break;
        }
      }
      break;
    } else {
      tmp2 = lruTable[i];
      lruTable[i] = tmp1;
      tmp1 = tmp2;
    }
  }
}

/**
 * @brief Clear a page from the LRU table at index.
 * @param index
 */
static void clearLRU(const uint8_t index) { lruTable[index] = DUMMY_PAGE; }

/**
 * @brief Clear a page from the LRU table.
 * @param pageNumber
 */
static void clearLRUPage(const uint8_t pageNumber) {
  for (int i = MAX_DIRTY_PAGES - 1; i >= 0; i--) {
    if (lruTable[i] == pageNumber) {
      lruTable[i] = DUMMY_PAGE;
      return;
    }
  }
}

/***************************** Include Files *********************************/
#include <msp430fr5994.h>
#include <stdint.h>
#include <string.h>

#include "ic.h"
#include "memory_management.h"

/*************************** Global Definitions ******************************/

static u16 mm_n_dirty_pages __attribute__((section(".data"))) = 0;
static u16 mm_n_active_pages __attribute__((section(".data"))) = 0;

/************************** Constant Definitions *****************************/
extern unsigned char __mmdata_start;
extern unsigned char __mmdata_end;
extern unsigned char __mmdata_loadStart;

struct LRU_candidate {
    u8 pageNumber;
    u8 tableIdx;
};

/**************************** Type Definitions *******************************/

/***************** Macros ****************************************************/
#define NPAGES (MMDATA_SIZE / PAGE_SIZE)

#if NPAGES > 254
#error Too many pages, increase page size or reduce memory usage
#endif

#define DUMMY_PAGE 255    //! Used for LRU table
#define REFCNT_MASK 0x3F  //! Reference count
#define LOADED 0x80       //! Mask to check if loaded
#define MODIFIED 0x40     //! Mask to check if modified

/************************** Function Prototypes ******************************/
static void writePageFRAM(u8 pageNumber);
static void loadPage(u8 pageNumber);
static void addLRU(u8 pageNumber);
static inline void clearLRU(u8 index);
static void clearLRUPage(u8 pageNumber);

/*************************** Extern Functions ********************************/

/************************** Variable Definitions *****************************/

static u8 attributeTable[NPAGES] __attribute__((section(".data"))) = {0};
static u8 lruTable[MAX_DIRTY_PAGES];

/*************************** Function definitions ****************************/

int mm_acquire(const u8 *memPtr, mm_mode mode) {
    if ((&__mmdata_start > (u8 *)memPtr) || (&__mmdata_end < (u8 *)memPtr)) {
        while (1)
            ;  // Error: Pointer out of bounds.
    }

    u16 pageNumber = ((u16)memPtr - (u16)&__mmdata_start) / PAGE_SIZE;

    if ((attributeTable[pageNumber] & REFCNT_MASK) >= 64) {
        while (1)
            ;  // Error: Too many references to a single page
    }

    if (mode ==
        MM_READWRITE) {  // && !(attributeTable[pageNumber] & MODIFIED)){
        if (mm_n_dirty_pages >= MAX_DIRTY_PAGES) {
            // Need to write back an inactive and dirty page first
            for (int i = MAX_DIRTY_PAGES - 1; i >= 0; i--) {
                u8 candidate = lruTable[i];
                if (candidate != DUMMY_PAGE) {
                    if ((attributeTable[candidate] & REFCNT_MASK) == 0 &&
                        (attributeTable[candidate] & LOADED) &&
                        (attributeTable[candidate] & MODIFIED)) {
                        writePageFRAM(candidate);
                        clearLRU(i);
                        break;
                    }
                }
            }

            if (mm_n_dirty_pages >= MAX_DIRTY_PAGES) {
                while (1)
                    ;  // Error: MAX_DIRTY_PAGES exceeded
            }
        }

        if (!(attributeTable[pageNumber] &
              MODIFIED)) {  // if page isn't already dirty
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
        update_thresholds(mm_n_dirty_pages * PAGE_SIZE,
                          mm_n_active_pages * PAGE_SIZE);
        oldPageTotal = mm_n_dirty_pages + mm_n_active_pages;
    }

    return 0;
}

int mm_release(const u8 *memPtr) {
    u16 pageNumber = ((u16)memPtr - (u16)&__mmdata_start) / PAGE_SIZE;
    if ((attributeTable[pageNumber] & REFCNT_MASK) > 0) {
        attributeTable[pageNumber]--;
        if ((attributeTable[pageNumber] & REFCNT_MASK) == 0) {
            mm_n_active_pages--;
        }
    } else {
        while (1)
            ;  // Error: Attempt to release inactive page
    }
    return 0;
}

void mm_restoreStatic(void) {
    for (int pageNumber = 0; pageNumber < NPAGES; pageNumber++) {
        // Clear loaded pages (bits are set again when calling loadPage)
        attributeTable[pageNumber] &= ~LOADED;

        if ((attributeTable[pageNumber] & REFCNT_MASK) > 0) {
            loadPage(pageNumber);
        }
    }
}

unsigned mm_suspendStatic(void) {
    unsigned pagesSaved = 0;

    for (int pageNumber = 0; pageNumber < NPAGES; pageNumber++) {
        if (mm_n_dirty_pages == 0) {
            break;
        } else if (attributeTable[pageNumber] & MODIFIED) {
            writePageFRAM(pageNumber);
            pagesSaved++;
            if ((attributeTable[pageNumber] & REFCNT_MASK) == 0) {
                clearLRUPage(pageNumber);
            }
        }
    }

    update_thresholds(mm_n_dirty_pages * PAGE_SIZE,
                      mm_n_active_pages * PAGE_SIZE);

    return pagesSaved * PAGE_SIZE;
}

/**
 * @brief Write a page to FRAM
 * @param pageNumber
 */
static void writePageFRAM(u8 pageNumber) {
    u16 dstStart;
    u16 srcStart;
    u16 pageOffset = pageNumber * PAGE_SIZE;
    u16 len;

    if (!(attributeTable[pageNumber] & MODIFIED)) {
        return;
    }

    u16 old_gie = __get_SR_register() & GIE;
    __disable_interrupt();  // Critical section (attributes get messed up if
                            // interrupted)
    srcStart = (u16)&__mmdata_start + pageOffset;
    dstStart = (u16)(&__mmdata_loadStart) + pageOffset;
    len = PAGE_SIZE;
    if (srcStart + PAGE_SIZE > (u16)&__mmdata_end) {
        len = (u16)&__mmdata_end - srcStart;
    }

    // Save page
    memcpy((u8 *)dstStart, (u8 *)srcStart, len);

    if ((attributeTable[pageNumber] & REFCNT_MASK) == 0) {
        // Page is clean
        attributeTable[pageNumber] &= ~MODIFIED;
        mm_n_dirty_pages--;
    }
    if (old_gie) {
        __enable_interrupt();
    }
}

int mm_acquire_array(const u8 *memPtr, size_t len, mm_mode mode) {
    int status;
    // Acquire each page referenced
    const u8 *ptr = memPtr;
    size_t remaining = len;
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

int mm_release_array(const u8 *memPtr, size_t len) {
    int res;

    // Error check
    if ((memPtr > &__mmdata_end) || (memPtr < &__mmdata_start) ||
        (memPtr + len) > &__mmdata_end) {
        while (1)
            ;  // Error: access out of bounds
    }

    // Release each page referenced
    const u8 *ptr = memPtr;
    size_t remaining = len;
    while (remaining > 0) {
        res = mm_release(ptr);
        if (remaining > PAGE_SIZE) {
            ptr += PAGE_SIZE;
            remaining -= PAGE_SIZE;
        } else {
            ptr += remaining;
            remaining = 0;
        }
    }
    return res;
}

size_t mm_get_n_active_pages(void) {
    size_t nActive = 0;
    for (int i = 0; i < NPAGES; i++) {
        if ((attributeTable[i] & REFCNT_MASK) > 0) {
            nActive++;
        }
    }
    return nActive;
}

size_t mm_get_n_dirty_pages(void) { return mm_n_dirty_pages; }

int mm_acquire_page(const u8 *memPtr, size_t nElements, size_t elementSize,
                    mm_mode mode) {
    int bytesAcquired;

    // Check if first element crosses a page boundary
    u16 pageNumberStart = (memPtr - &__mmdata_start) / PAGE_SIZE;
    u16 pageNumberEnd =
        (memPtr + elementSize - 1  // end address of first element
         - &__mmdata_start         // - start address
         ) /
        PAGE_SIZE;

    if (pageNumberStart != pageNumberEnd) {
        // Need to load 2 pages
        mm_acquire(memPtr, mode);
        mm_acquire(memPtr + elementSize - 1, mode);
        bytesAcquired = (pageNumberStart + 1) * PAGE_SIZE -
                        (memPtr - &__mmdata_start) + PAGE_SIZE;
    } else {
        mm_acquire(memPtr, mode);
        bytesAcquired =
            (pageNumberStart + 1) * PAGE_SIZE - (memPtr - &__mmdata_start);
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
static void loadPage(u8 pageNumber) {
    if (!(attributeTable[pageNumber] & LOADED)) {
        u8 *srcStart;
        u8 *dstStart;
        u16 len;
        u16 pageOffset = pageNumber * PAGE_SIZE;

        dstStart = &__mmdata_start + pageOffset;      // Memory address
        srcStart = &__mmdata_loadStart + pageOffset;  // Snapshot address
        len = PAGE_SIZE;
        if ((u16)dstStart + PAGE_SIZE > (u16)&__mmdata_end) {
            len = (u16)&__mmdata_end - (u16)dstStart;
        }

        memcpy((u8 *)dstStart, (u8 *)srcStart, len);
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
static void addLRU(u8 pageNumber) {
    u8 tmp1, tmp2;

    if (pageNumber > NPAGES) {
        while (1)
            ;  // Error: page number out of bounds.
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
static void clearLRU(u8 index) { lruTable[index] = DUMMY_PAGE; }

/**
 * @brief Clear a page from the LRU table.
 * @param pageNumber
 */
static void clearLRUPage(u8 pageNumber) {
    for (int i = MAX_DIRTY_PAGES - 1; i >= 0; i--) {
        if (lruTable[i] == pageNumber) {
            lruTable[i] = DUMMY_PAGE;
            return;
        }
    }
}


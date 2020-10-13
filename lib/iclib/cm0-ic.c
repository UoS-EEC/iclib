/*
 * Copyright (c) 2018-2020, University of Southampton.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>
#include "lib/cmsis/core_cm0.h"
#include "lib/iclib/config.h"
#include "lib/iclib/ic.h"
#include "lib/iclib/memory-management.h"
#include "lib/support/support.h"

#ifdef SIMULATION
#include <fused.h>
#endif

// ------------- CONSTANTS -----------------------------------------------------
extern uint8_t __stack_low, __stack_high, __stack_size;
extern uint8_t __data_low, __data_high, __data_loadLow;
extern uint8_t __mmdata_low, __mmdata_high, __mmdata_loadLow;
extern uint8_t __boot_stack_high;

// ------------- Globals -------------------------------------------------------

// ------------- PERSISTENT VARIABLES ------------------------------------------

// Snapshots
uint32_t saved_stack_pointer PERSISTENT;
uint32_t stack_snapshot[STACK_SIZE / sizeof(uint32_t)] PERSISTENT;
int suspending PERSISTENT;         /*! Flag to determine whether returning from
                                  suspend() or restore() */
int snapshotValid PERSISTENT = 0;  //! Flag: whether snapshot is valid

/* ------ Function Prototypes -----------------------------------------------*/

/* ------ ASM functions ---------------------------------------------------- */
extern void suspend_stack_and_regs(uint32_t *saved_sp, int *snapshotValid,
                                   uint32_t *stackSnapshot);
extern void suspend_regs(uint32_t *saved_sp, int *snapshotValid);
extern void restore_registers(uint32_t *saved_sp);

/* ------ Function Declarations ---------------------------------------------*/

void __attribute__((optimize(1))) _start() {
#ifdef SIMULATION
  OUTPORT = (1u << 0);  // Set keep-alive high
#endif

#if defined(ALLOCATEDSTATE) || defined(MANAGEDSTATE)
  memcpy(&__data_low, &__data_loadLow, &__data_high - &__data_low);
  const uint32_t mmdata_size = &__mmdata_high - &__mmdata_low;
  ic_update_thresholds(mmdata_size, mmdata_size);
  mm_init_lru();
  mm_restore();
#endif

  // Enable suspend interrupt
  NVIC_SetPriority(0, 0);
  NVIC_EnableIRQ(0);

  if (snapshotValid) {  // Restore stack from snapshot and continue execution

#if defined(ALLOCATEDSTATE) || defined(MANAGEDSTATE)
    // Restore from saved SP to stack_high
    uint8_t *sp = (uint8_t *)saved_stack_pointer;
    int len = &__stack_high - (uint8_t *)sp;
    uint8_t *src = (uint8_t *)&stack_snapshot + ((uint32_t)&__stack_size - len);
    memcpy(sp, src, len);
#endif
    restore_registers(&saved_stack_pointer);  // Returns to suspend()
  }

  // First power-up: set SP and start execution
  __set_MSP((uint32_t)&__stack_high);
  void main();  // Suppress implicit decl. warning
  main();

  NVIC_SystemReset();  // Should never get here
}

// Suspend interrupt
void Interrupt0_Handler() {
  snapshotValid = 0;
  suspending = 1;
#if defined(ALLOCATEDSTATE) || defined(MANAGEDSTATE)
  // Save data, mmdata & stack
  mm_flush();
  memcpy(&__data_loadLow, &__data_low, &__data_high - &__data_low);
  suspend_stack_and_regs(&saved_stack_pointer, &snapshotValid, stack_snapshot);
#elif defined(QUICKRECALL)
  // Save registers only
  suspend_regs(&saved_stack_pointer, &snapshotValid);
#endif
}

void ic_update_thresholds(unsigned n_suspend, unsigned n_restore) {
  // Do nothing
}

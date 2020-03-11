#include <stdint.h>
#include <string.h>
#include "support/support.h"
#include <fused.h>

void indicate_begin() {
   SIMPLE_MONITOR = SIMPLE_MONITOR_INDICATE_BEGIN;
;}

void indicate_end() {
  SIMPLE_MONITOR = SIMPLE_MONITOR_INDICATE_END;
}

void fused_end_simulation() {
  SIMPLE_MONITOR = SIMPLE_MONITOR_KILL_SIM;
  while(1); // Just in case
}

void wait() {
  for (volatile uint32_t i = 0; i < 10000ll; i++)
    ; // delay
}

void fused_assert (bool c) {
  if (!c) {
    SIMPLE_MONITOR = SIMPLE_MONITOR_TEST_FAIL;
    while(1);
  }
}

void target_init() { return; }

#pragma once

#include <stdbool.h>

void target_init();
void wait();
void indicate_begin();
void indicate_end();
void fused_assert(bool c);
void fused_end_simulation();
void _start();

#pragma once

#include <stddef.h>
#include <stdint.h>

/**
 * Hand-written fast version of memcpy
 * */
void fastmemcpy(uint8_t *dst, uint8_t *src, size_t len);

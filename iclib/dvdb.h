/*
 * Copyright (c) 2018-2020, University of Southampton.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Voltage dop per byte saved/restored to/from FRAM when no energy is supplied.
// Generated by generate-dvdb-table.py
#include <stdint.h>

#ifdef ALLOCATEDSTATE
const uint16_t vdrop[] = {
    1,   2,   3,   5,   6,   7,   9,   10,  11,  12,  14,  15,  16,  18,  19,
    20,  21,  23,  24,  25,  27,  28,  29,  30,  32,  33,  34,  36,  37,  38,
    39,  41,  42,  43,  45,  46,  47,  48,  50,  51,  52,  54,  55,  56,  57,
    59,  60,  61,  63,  64,  65,  66,  68,  69,  70,  72,  73,  74,  75,  77,
    78,  79,  81,  82,  83,  84,  86,  87,  88,  90,  91,  92,  93,  95,  96,
    97,  99,  100, 101, 102, 104, 105, 106, 108, 109, 110, 111, 113, 114, 115,
    117, 118, 119, 120, 122, 123, 124, 126, 127, 128, 129, 131, 132, 133, 135,
    136, 137, 138, 140, 141, 142, 144, 145, 146, 147, 149, 150, 151, 153, 154,
    155, 156, 158, 159, 160, 162, 163, 164,
};

#else

const uint16_t vdrop[] = {
    2,   4,   7,   9,   11,  14,  16,  18,  21,  23,  25,  28,  30,  32,  35,
    37,  39,  42,  44,  46,  49,  51,  53,  56,  58,  60,  63,  65,  67,  70,
    72,  74,  77,  79,  82,  84,  86,  89,  91,  93,  96,  98,  100, 103, 105,
    107, 110, 112, 114, 117, 119, 121, 124, 126, 128, 131, 133, 135, 138, 140,
    142, 145, 147, 149, 152, 154, 156, 159, 161, 164, 166, 168, 171, 173, 175,
    178, 180, 182, 185, 187, 189, 192, 194, 196, 199, 201, 203, 206, 208, 210,
    213, 215, 217, 220, 222, 224, 227, 229, 231, 234, 236, 238, 241, 243, 246,
    248, 250, 253, 255, 257, 260, 262, 264, 267, 269, 271, 274, 276, 278, 281,
    283, 285, 288, 290, 292, 295, 297, 299,
};

#endif

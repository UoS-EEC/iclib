/* Generate reference output from host machine */

#include <stdint.h>
#include <stdio.h>

#define NOLIBIC
#include "crc.h"

#define MMDATA
#include "lipsum.h"

typedef unsigned char u8;

int main(int argc, char **argv) {
    printf("%08x\n", (unsigned)crc32buf(input, sizeof(input)));
}

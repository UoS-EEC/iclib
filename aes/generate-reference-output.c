/* Generate reference output from host machine */

#include <stdint.h>
#include <stdio.h>

#include "TI_aes_128_encr_only.h"

#define MMDATA  // Empty macro

#include "lipsum.h"

#define AES_BLOCK_SIZE 16u
typedef unsigned char u8;
u8 key[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

void dump_block(u8 *ptr) {
    for (int i = 0; i < AES_BLOCK_SIZE; i++) {
        printf("0x%02x, ", *ptr);
        ptr += AES_BLOCK_SIZE;
    }
    printf("\n");
}

int main(int argc, char **argv) {
    // AES128 in Cipher Block Chaining mode
    u8 *prevBlock;
    u8 *ptr = input;

    // Encrypt first block
    aes_encrypt(ptr, key);
    dump_block(ptr);
    prevBlock = ptr;
    ptr += AES_BLOCK_SIZE;

    // Encrypt remaining blocks
    while (ptr < input + sizeof(input)) {
        // CBC - Cipher Block Chaining mode
        for (int i = 0; i < AES_BLOCK_SIZE; i++) {
            ptr[i] = ptr[i] ^ prevBlock[i];
        }

        // Encrypt current block
        aes_encrypt(ptr, key);
        dump_block(ptr);

        prevBlock = ptr;
        ptr += AES_BLOCK_SIZE;
    }
}

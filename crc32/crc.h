/*
**  CRC.H - header file for SNIPPETS CRC and checksum functions
*/

#ifndef CRC__H
#define CRC__H

#include <stdlib.h> /* For size_t                 */

//#include <config.h>
//#include <memory_management.h>

#include "sniptype.h" /* For BYTE, WORD, DWORD      */

/*
**  File: CRC_32.C
*/

#define UPDC32(octet, crc) \
    (crc_32_tab[((crc) ^ ((BYTE)octet)) & 0xff] ^ ((crc) >> 8))

DWORD updateCRC32(unsigned char ch, DWORD crc);
Boolean_T crc32file(char *name, DWORD *crc, long *charcnt);
DWORD crc32buf(char *buf, size_t len);

#endif /* CRC__H */

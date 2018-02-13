#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define BITS_IN_BYTE 8

const char *hexvals = "0123456789ABCDEF";

const char hexval (const char c) {
  const char *hptr = strchr(hexvals, c);
  if (hptr == NULL) {
    printf("bad hex char: %c\n", c);
    exit(EXIT_FAILURE);
  }
  return hptr - hexvals;
}

void hex2bin (const char *hexstr, const size_t hexlen, char *buffer, const size_t bufferlen) {
  size_t i = 0;
  memset(buffer, 0, bufferlen);
  for (; i < bufferlen; i++) {
    const size_t p1 = i * 2;
    const size_t p2 = i * 2 + 1;

    if (p1 < hexlen) {
      const char n1 = hexval(hexstr[p1]);
      buffer[i] = (n1 << 4) & 0xF0;
    }

    if (p2 < hexlen) {
      const char n2 = hexval(hexstr[p2]);
      buffer[i] |= (n2 & 0x0F);
    }
  }
}

void bitoffset (const char *from, const size_t len, char *dest, const size_t offset) {
  memset(dest, 0, len);
  
  size_t i = 0;
  for (; i < len; i++) {
    dest[i] = from[i] << offset;
    if (i < len - 1) {
      dest[i] |= from[i + 1] >> (BITS_IN_BYTE - offset);
    }
  }
}

void printhex (const char *buffer, const size_t bufferlen) {
  size_t i = 0;
  for (; i < bufferlen; i++) {
    printf("%c", hexvals[(buffer[i] >> 4) & 0x0F]);
    printf("%c", hexvals[buffer[i] & 0x0F]);
  }
  printf("\n");
}

/*argv[1] should be the buffer input as a hex string*/
/*argv[2] should be the offset into the buffer for realignment*/
int main (int argc, char **argv) {
  const char *hexstr = argv[1];
  const size_t pos = strtol(argv[2], NULL, 10);

  const size_t hexlen = strlen(hexstr);
  const size_t bufferlen = hexlen % 2 == 0 ? hexlen / 2 : hexlen / 2 + 1;
  char buffer[bufferlen];
  hex2bin(hexstr, hexlen, buffer, bufferlen);

  char offset_buffer[bufferlen];
  bitoffset(buffer, bufferlen, offset_buffer, pos);

  printhex(buffer, bufferlen);
  printhex(offset_buffer, bufferlen);

  return EXIT_SUCCESS;
}

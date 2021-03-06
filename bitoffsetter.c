#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define BASE_10 10
#define BITS_IN_BYTE 8

const char *hexvals = "0123456789ABCDEF";

void usage (const char *exename, const char *msg) {
  printf("%s\n", msg);
  printf("usage: %s <offset> <hex_string>\n", exename);
}

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

  const char right_mask = (0x1 << offset) - 1;
  const char left_mask = right_mask ^ 0xFF;

  size_t i = 0;
  for (; i < len; i++) {
    dest[i] = (from[i] << offset) & left_mask;
    if (i < len - 1) {
      dest[i] |= (from[i + 1] >> (BITS_IN_BYTE - offset)) & right_mask;
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

/*argv[2] should be the buffer input as a hex string*/
/*argv[1] should be the offset into the buffer for realignment*/
int main (int argc, char **argv) {

  if (argc < 3) {
    usage(argv[0], "missing args\n");
    return EXIT_FAILURE;
  }

  char *endptr;
  const char *hexstr = argv[2];
  const size_t pos = strtoul(argv[1], &endptr, BASE_10);

  if (*endptr != 0) {
    printf("bad offset: %s, ", argv[1]);
    usage(argv[0], "should be 0-8");
    return EXIT_FAILURE;
  }

  if (pos < 0 || pos > 8) {
    printf("bad offset: %d, ", (int)pos);
    usage(argv[0], "should be 0-8");
    return EXIT_FAILURE;
  }

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
